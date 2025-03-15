/*
 Soubor:  Tools.cpp
 Popis:  Tøída spravující nástroje pro službu.
 Autor:  Akutapi
 Datum:  6-12-2024
*/
#include "Tools.h"
#include <format>
#include <iostream>
#include <algorithm>

// Konstanty
#define SERVICE_NAME L"storagenode"
#define SERVICE_UPDATE_NAME L"updater"
#define LOG_FILE_NAME L"storagenode.log"
#define EXE_SERVICE_NAME L"storagenode.exe"
#define SLEEP_TIME 250

Tools::Tools(Logger& _logger) : logger(_logger), logReducer(_logger), scManager(_logger), config(_logger), serviceUpdater(_logger), discordManager(_logger)
{
	SetDiscordManager();
}

Tools::~Tools()
{
	discordManager.sendDM(L"Service stopped.");
}

std::vector<Tools::TimerInfo> Tools::GetTimers()
{	
	// aktualizuj konfiguraèní soubor
	config.UpdateConfig();
	// nastavit custom timery
	std::vector<TimerInfo> timers;
	timers.push_back({NULL, false, config.GetReduceLogTimeInHours(), [this]() { this->LogMaintenance(); }, [this](const ConfigFileManager& config) -> float { return config.GetReduceLogTimeInHours(); }});
	timers.push_back({NULL, false, config.GetCheckStorjNodesTimeInHours(), [this]() { this->CheckStorjNodesStatus(); }, [this](const ConfigFileManager& config) -> float { return config.GetCheckStorjNodesTimeInHours(); }});
	timers.push_back({NULL, false, config.GetCheckStorjNodeUpdateTimeInHours(), [this]() { this->CheckStorjNodeUpdate(); }, [this](const ConfigFileManager& config) -> float { return config.GetCheckStorjNodeUpdateTimeInHours(); }});
	return timers;
}

ConfigFileManager& Tools::UpdateConfigFromFile()
{
	config.UpdateConfig();
	return config;
}

bool Tools::LogMaintenance()
{
	// Nastavení DiscordManager
	SetDiscordManager();

	// Získá seznam služeb bez StorJ Update Nodù
	std::vector<std::wstring> services = GetServices(SERVICE_SET::ALL_TO_MAINTENANCE);

	// Pokud nejsou žádné StroJ Nody k redukci log souborù, skonèí
	if (services.empty())
	{
		//logger.LogInfo(L"No StroJ Nodes for log file reduction.");
		return true;
	}

	std::vector<Tools::MaintenanceStatus> servicesToMaintenance = SetServiceStatusForMaintenance(services);

	int serviceMaintenanceCount = 3;
	while( needsMaintenance(servicesToMaintenance) && serviceMaintenanceCount > 0 )
	for (auto& service : servicesToMaintenance)
	{
		std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service.serviceName)).parent_path() / LOG_FILE_NAME;

		// Redukce log souboru do temp souboru
		if (service.status == INIT)
		{
			if (!logReducer.ReduceLogToTemp(logPath, config.GetMaxLogSize(), config.GetReduceLogSize()))
			{
				logger.LogError(std::format(L"Log file reduction failed: {}", logPath));
				continue;
			}
			service.status = TEMPLATED;
		}
		
		// Stop service
		if (service.status == TEMPLATED)
		{
			if (!scManager.CustomStopServiceWithWait(service.serviceName))
			{
				logger.LogError(std::format(L"Service stop failed: {}", service.serviceName));
				continue;
			}
			service.status = STOPPED;
		}

		// Nahrazení log souboru temp souborem
		if (service.status == STOPPED)
		{
			if (!logReducer.ReplaceLogWithTempFile(logPath))
			{
				logger.LogError(std::format(L"Log file replacement failed: {}", logPath));
				continue;
			}
			service.status = REPLACED;
		}

		// Start service
		if (service.status == REPLACED)
		{
			if (!scManager.CustomStartServiceWithWait(service.serviceName))
			{
				logger.LogError(std::format(L"Service start failed: {}", service.serviceName));
				continue;
			}
			service.status = STARTED;
		}
		Sleep(SLEEP_TIME); // Poèkáme pøed dalším pokusem
		serviceMaintenanceCount--;
	}


	if (!needsMaintenance(servicesToMaintenance))
	{
		//logger.LogInfo(L"Log maintenance completed.");
		return true;
	}

	std::wstring message = L"Log maintenance failed: ";
	for (auto& service : servicesToMaintenance)
	{
		if (service.status != STARTED)
		{
			message.append(service.serviceName).append(L" : ").append(StatusServiceToString(service.status)).append(L", ");
		}
	}
	message.pop_back();
	message.pop_back();

	if (!discordManager.sendDM(message))
	{
		logger.LogError(L"Error sending message to Discord.");
	}
	logger.LogError(message);
	return false;
}

bool Tools::CheckStorjNodesStatus()
{
	// Nastavení DiscordManager
	SetDiscordManager();

	// Získá seznam služeb bez StorJ Update Nodù
	std::vector<std::wstring> services = GetServices(SERVICE_SET::ALL_TO_CHECK);
	if (services.empty())
	{
		//logger.LogInfo(L"No StroJ Nodes found.");
		return true;
	}

	std::vector<ServiceStatus> servicesStatus = SetServiceStatusForCheck(services);
	// Pokus o nastartování služeb, které selhaly
	int serviceStartCount = 3;
	while ( needsStart(servicesStatus) && serviceStartCount > 0)
	{
		for (auto& service : servicesStatus)
		{
			if (!scManager.CustomStartServiceWithWait(service.serviceName))
			{
				logger.LogWarning(std::format(L"Service start failed: {}", service.serviceName));
			}
			service.status = RUNNING_UPDATED;
		}
		Sleep(SLEEP_TIME);
		serviceStartCount--;
	}


	if (!needsStart(servicesStatus))
	{
		//logger.LogInfo(L"Services checked: All services are running.");
		return true;
	}

	// Odeslání zprávy na Discord
	std::wstring message = L"StorJ Nodes are not running: ";
	for (auto& service : servicesStatus)
	{
		if (service.status != RUNNING_UPDATED)
		{
			message.append(service.serviceName).append(L", ");
		}
		
	}
	message.pop_back();
	message.pop_back();
	
	if(!discordManager.sendDM(message))
	{
		logger.LogError(L"Error sending message to Discord.");
	}
	logger.LogError(message);
	return false;
}

bool Tools::CheckStorjNodeUpdate()
{
	// Nastavení DiscordManager
	SetDiscordManager();

	// Získá seznam služeb bez StorJ Update Nodù
	std::vector<std::wstring> services = GetServices(SERVICE_SET::ALL_TO_UPDATE);
	std::vector<std::wstring> updaterServices = GetServices(SERVICE_SET::UPDATERS);

	if (services.empty())
	{
		logger.LogWarning(L"No StroJ Nodes for update found.");
		return true;
	}
	
	if (updaterServices.empty())
	{
		logger.LogError(L"No StroJ Updater Nodes.");
		return false;
	}

	std::wstring sourceServicePath = GetSorceServicePath(updaterServices);
	std::vector<ServiceStatus> servicesToUpdate = SetServiceStatusForUpdate(services);
	// Create shadow copy
	if(!serviceUpdater.CreateShadowCopy(sourceServicePath))
	{
		logger.LogError(L"Error while creating shadow copy.");
		return false;
	}

	// Update Storj Node

	int serviceUpdateCount = 5;
	while( needsUpdate(servicesToUpdate) && serviceUpdateCount > 0)
	{
		for (auto& service : servicesToUpdate)
		{
			std::wstring servicePath = scManager.GetServicePathByName(service.serviceName);

			// Stop service
			if (service.status == RUNNING_OLD)
			{
				if (!scManager.CustomStopServiceWithWait(service.serviceName))
				{
					logger.LogWarning(std::format(L"Service stop failed: {}", service.serviceName));
					continue;
				}
				service.status = STOPED_OLD;
			}

			// Update service
			if (service.status == STOPED_OLD)
			{
				if (!serviceUpdater.UpdateService(sourceServicePath, servicePath))
				{
					logger.LogError(std::format(L"Service update failed: {}", service.serviceName));
					continue;
				}
				service.status = STOPED_UPDATED;
			}

			// Start service
			if (service.status == STOPED_UPDATED)
			{
				if (!scManager.CustomStartServiceWithWait(service.serviceName))
				{
					logger.LogWarning(std::format(L"Service start failed: {}", service.serviceName));
					continue;
				}
				service.status = RUNNING_UPDATED;
			}

		}
		Sleep(SLEEP_TIME); // Poèkáme pøed dalším pokusem
		serviceUpdateCount--;
	}

	// Delete shadow copy
	if (!serviceUpdater.DeleteShadowCopy(sourceServicePath))
	{
		logger.LogError(L"Error while deleting shadow copy.");
		return false;
	}

	if (!needsUpdate(servicesToUpdate))
	{
		//logger.LogInfo(L"Check and update completed.");
		return true;
	}

	std::wstring message = L"Check and update failed: ";
	for (auto& service : servicesToUpdate)
	{
		if (service.status != RUNNING_UPDATED)
		{
			message.append(service.serviceName).append(L" : ").append(StatusServiceToString(service.status)).append(L", ");
		}
	}
	message.pop_back();
	message.pop_back();

	if (!discordManager.sendDM(message))
	{
		logger.LogError(L"Error sending message to Discord.");
	}
	logger.LogError(message);
	return false;
}

std::vector<std::wstring> Tools::GetServices(const SERVICE_SET serviceSet)
{
	std::vector<std::wstring> services = scManager.GetServicesByName(SERVICE_NAME);

	if (serviceSet == SERVICE_SET::ALL)
	{
		return services;
	}

	std::vector<std::wstring> tempServices;
	for (auto& service : services)
	{
		if (service.find(SERVICE_UPDATE_NAME) == std::wstring::npos)
		{
			tempServices.push_back(service);
		}
	}
	services = tempServices;
	tempServices.clear();
	if (serviceSet == SERVICE_SET::ALL_TO_CHECK)
	{
		return services;
	}

	if (serviceSet == SERVICE_SET::ALL_TO_MAINTENANCE)
	{
		// kontrola existence souboru a jeho velikosti
		for (auto& service : services)
		{
			std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / LOG_FILE_NAME;
			if (!std::filesystem::exists(logPath))
			{
				logger.LogWarning(std::format(L"Log file does not exist: {}", logPath));
				continue;
			}
			if (std::filesystem::file_size(logPath) <= config.GetMaxLogSize())
			{
				//Není tøeba redukovat log soubor
				continue;
			}
			tempServices.push_back(service);
		}
		return tempServices;
	}

	tempServices.clear();
	std::vector<std::wstring> updaterServices;
	for (auto& service : scManager.GetServicesByName(SERVICE_NAME))
	{
		if (service.find(SERVICE_UPDATE_NAME) != std::wstring::npos)
		{
			updaterServices.push_back(service);
		}
	}

	if (serviceSet == SERVICE_SET::UPDATERS)
	{
		return updaterServices;
	}

	std::wstring sourceServicePath = GetSorceServicePath(updaterServices);
	tempServices.clear();
	if (serviceSet == SERVICE_SET::ALL_TO_UPDATE)
	{
		for (auto& service : services)
		{
			std::wstring servicePath = scManager.GetServicePathByName(service);
			if (serviceUpdater.NeedUpdate(sourceServicePath, servicePath) && std::find(updaterServices.begin(), updaterServices.end(), service) == updaterServices.end())
			{
				tempServices.push_back(service);
			}
		}
		return tempServices;
	}
	return services;
}

bool Tools::needsUpdate(const std::vector<ServiceStatus>& servicesToUpdate) const
{
	return std::any_of(servicesToUpdate.begin(), servicesToUpdate.end(), [](const ServiceStatus& service) { return service.status != RUNNING_UPDATED; });
}

bool Tools::needsStart(const std::vector<ServiceStatus>& services) const
{
	return needsUpdate(services);
}

bool Tools::needsMaintenance(const std::vector<MaintenanceStatus>& servicesToMaintenance) const
{
	return std::any_of(servicesToMaintenance.begin(), servicesToMaintenance.end(), [](const MaintenanceStatus& service) { return service.status != STARTED; });
}

std::vector<Tools::ServiceStatus> Tools::SetServiceStatusForCheck(const std::vector<std::wstring> services)
{
	std::vector<ServiceStatus> servicesToCheck;
	if (services.empty())
	{
		logger.LogWarning(L"No services to check.");
		return servicesToCheck;
	}
	for (auto& service : services)
	{
		DWORD serviceStatus = scManager.GetServiceStatus(service);
		if (serviceStatus == SERVICE_RUNNING)
		{
			servicesToCheck.push_back({ service, RUNNING_UPDATED });
		}
		else
		{
			servicesToCheck.push_back({ service, STOPED_UPDATED });
		}
	}

	return servicesToCheck;
}

std::vector<Tools::ServiceStatus> Tools::SetServiceStatusForUpdate(const std::vector<std::wstring> services)
{
	std::vector<ServiceStatus> servicesToUpdate;
	if (services.empty())
	{
		logger.LogWarning(L"No services to update.");
		return servicesToUpdate;
	}
	for (auto& service : services)
	{
		DWORD serviceStatus = scManager.GetServiceStatus(service);
		if (serviceStatus == SERVICE_STOPPED)
		{
			servicesToUpdate.push_back({ service, STOPED_OLD });
		}
		else
		{
			servicesToUpdate.push_back({ service, RUNNING_OLD });
		}
	}

	return servicesToUpdate;
}

std::wstring Tools::StatusServiceToString(const UPDATE_STATUS status)
{
	switch (status)
	{
	case RUNNING_OLD: return L"RUNNING_OLD";
	case STOPED_OLD: return L"STOPED_OLD";
	case STOPED_UPDATED: return L"STOPED_UPDATED";
	case RUNNING_UPDATED: return L"RUNNING_UPDATED";
	default: return L"UNKNOWN";
	}
}

std::wstring Tools::StatusServiceToString(const MAINTENANCE_STATUS status)
{
	switch (status)
	{
	case INIT: return L"INIT";
	case TEMPLATED: return L"TEMPLATED";
	case STOPPED: return L"STOPPED";
	case REPLACED: return L"REPLACED";
	case STARTED: return L"STARTED";
	default: return L"UNKNOWN";
	}
}

std::vector<Tools::MaintenanceStatus> Tools::SetServiceStatusForMaintenance(const std::vector<std::wstring> services)
{
	std::vector<MaintenanceStatus> servicesToMaintenance;
	for (auto& service : services)
	{
		servicesToMaintenance.push_back({ service, INIT });
	}
	return servicesToMaintenance;
}

std::wstring Tools::GetSorceServicePath(const std::vector<std::wstring>& updaterServices)
{
	// Získá cestu k souboru StorJ Nodu a automatickou aktualizaci
	std::vector<std::wstring> sourcesSrevicesPaths;
	for (auto& updater : updaterServices)
	{
		// Cesta ke službì nody s automatickou aktualizací
		std::wstring nodeExePath = std::filesystem::path(scManager.GetServicePathByName(updater)).parent_path() / EXE_SERVICE_NAME;
		// pokud opravdu existuje pøidá ji do seznamu
		if (std::filesystem::exists(nodeExePath))
		{
			sourcesSrevicesPaths.push_back(nodeExePath);
		}
	}

	if (sourcesSrevicesPaths.empty())
	{
		logger.LogError(L"No source file StorJ Nodes found.");
		return L"";
	}

	// Získá nejnovìjší verzi StorJ Nodu
	std::wstring sourceServicePath = sourcesSrevicesPaths[0];
	for (auto& servicePath : sourcesSrevicesPaths)
	{
		if (serviceUpdater.IsVersionGreater(servicePath, sourceServicePath))
		{
			sourceServicePath = servicePath;
		}
	}
	return sourceServicePath;
}

void Tools::SetDiscordManager()
{
	// Naète konfiguraèní soubor
	config.UpdateConfig();
	// Vytvoøení Discord bota
	if (!discordManager.SetDiscordBot(config.GetDiscordBotToken(), config.GetDiscordUserID()))
	{
		logger.LogError(L"Error creating Discord bot.");
		return;
	}
	discordManager.sendDM(L"The Storj service has started.");
}
