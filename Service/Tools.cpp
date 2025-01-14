/*
 Soubor:  Tools.cpp
 Popis:  Tøída spravující nástroje pro službu.
 Autor:  Akutapi
 Datum:  6-12-2024
*/
#include "Tools.h"
#include <format>
#include <iostream>

// Konstanty
#define SERVICE_NAME L"storagenode"
#define SERVICE_UPDATE_NAME L"updater"
#define LOG_FILE_NAME L"storagenode.log"
#define EXE_SERVICE_NAME L"storagenode.exe"

Tools::Tools(Logger& _logger) : logger(_logger), logReducer(_logger), scManager(_logger), config(_logger), serviceUpdater(_logger)
{
}

Tools::~Tools()
{
}

std::vector<Tools::TimerInfo> Tools::GetTimers()
{	
	// aktualizuj konfiguraèní soubor
	config.UpdateConfig();
	// nastavit custom timery
	std::vector<TimerInfo> timers;
	timers.push_back({NULL, false, config.GetReduceLogTimeInHours(), [this]() { this->LogMaintenance(); }, [this](const ConfigFileManager& config) -> int { return config.GetReduceLogTimeInHours(); }});
	timers.push_back({NULL, false, config.GetCheckStorjNodesTimeInHours(), [this]() { this->CheckStorjNodesStatus(); }, [this](const ConfigFileManager& config) -> int { return config.GetCheckStorjNodesTimeInHours(); }});
	timers.push_back({NULL, false, config.GetCheckStorjNodeUpdateTimeInHours(), [this]() { this->CheckStorjNodeUpdate(); }, [this](const ConfigFileManager& config) -> int { return config.GetCheckStorjNodeUpdateTimeInHours(); }});
	return timers;
}

ConfigFileManager& Tools::UpdateConfigFromFile()
{
	config.UpdateConfig();
	return config;
}

bool Tools::LogMaintenance()
{
	// Naète konfiguraèní soubor
	config.UpdateConfig();

	// Získá seznam služeb bez StorJ Update Nodù
	std::vector<std::wstring> services;
	for (auto& service : scManager.GetServicesByName(SERVICE_NAME))
	{
		if (service.find(SERVICE_UPDATE_NAME) == std::wstring::npos)
		{
			services.push_back(service);
		}
	}
	if (services.empty())
	{
		logger.LogWarning(L"No StroJ Nodes found.");
		return true;
	}

	// kontrola existence souboru a jeho velikosti
	std::vector<std::wstring> servicesToMaintain;
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
		servicesToMaintain.push_back(service);
	}
	// Pokud nejsou žádné StroJ Nody k redukci log souborù, skonèí
	if (servicesToMaintain.empty())
	{
		logger.LogInfo(L"No StroJ Nodes for log file reduction.");
		return true;
	}

	// Pro každý StroJ Nod provede redukci log souboru do temp souboru
	std::vector<std::wstring> failedServices;
	for (auto& service : servicesToMaintain)
	{
		// Redukce log souboru
		std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / LOG_FILE_NAME;
		if (!logReducer.ReduceLogToTemp(logPath, config.GetMaxLogSize(), config.GetReduceLogSize()))
		{
			logger.LogError(std::format(L"Log file reduction failed: {}", logPath)); 
			failedServices.push_back(service);
		}
	}

	// Odstranìní servisù , které se nepodaøilo redukovat log soubory

	if (!failedServices.empty())
	{
		logger.LogError(L"Some log files could not be reduced.");

		for (auto& service : failedServices)
		{
			servicesToMaintain.erase(std::remove(servicesToMaintain.begin(), servicesToMaintain.end(), service), servicesToMaintain.end());
		}
	}

	if (servicesToMaintain.empty())
	{
		logger.LogError(L"No log file could be reduced.");
		return false;
	}

	// Pro každý StroJ Nod pøepíše log soubor temp souborem
	for (auto& service : servicesToMaintain)
	{
		// Stop service
		scManager.CustomStopServiceWithWait(service);
		// Nahrazení log souboru temp souborem
		std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / LOG_FILE_NAME;
		if (!logReducer.ReplaceLogWithTempFile(logPath))
		{
			logger.LogError(std::format(L"Log file replacement failed: {}", logPath)); 
		}
		// Start service
		scManager.CustomStartService(service);
	}
	logger.LogInfo(L"Log reduction completed.");
	return true;
}

bool Tools::CheckStorjNodesStatus()
{
	// Naète konfiguraèní soubor
	config.UpdateConfig();

	// Získá seznam služeb bez StorJ Update Nodù
	std::vector<std::wstring> services;
	for (auto& service : scManager.GetServicesByName(SERVICE_NAME))
	{
		if (service.find(SERVICE_UPDATE_NAME) == std::wstring::npos)
		{
			services.push_back(service);
		}
	}

	if (services.empty())
	{
		logger.LogInfo(L"No StroJ Nodes found.");
		return true;
	}

	bool result = true;
	// Kontrola bìhu služeb
	for (auto& service : services)
	{
		DWORD status = scManager.GetServiceStatus(service);
		if (status == 0)
		{
			logger.LogError(std::format(L"Error getting service status: {}", service));
			result = false;
		}
		else if (status == SERVICE_STOPPED)
		{
			logger.LogWarning(std::format(L"Service stopped: {}", service));
			result = false;
		}
		else if (status == SERVICE_RUNNING)
		{
			//logger.LogInfo(std::format(L"Service running: {}", service));
		}
		else
		{
			logger.LogWarning(std::format(L"Service status unknown: {}", service));
			result = false;
		}
	}

	if (result)
	{
		logger.LogInfo(L"Services checked: All services are running.");
		return true;
	}

	//TODO : metody na informování o stavu služeb

	logger.LogWarning(L"Service check: Some services failed.");
	return true;
}

bool Tools::CheckStorjNodeUpdate()
{
	// Naète konfiguraèní soubor
	config.UpdateConfig();

	// Získá seznam služeb bez StorJ Update Nodù
	std::vector<std::wstring> allServices;
	for (auto& service : scManager.GetServicesByName(SERVICE_NAME))
	{
		if (service.find(SERVICE_UPDATE_NAME) == std::wstring::npos)
		{
			allServices.push_back(service);
		}
	}

	if (allServices.empty())
	{
		logger.LogWarning(L"No StroJ Nodes found.");
		return true;
	}

	// Získá seznam updaters služeb
	std::vector<std::wstring> updaterServices;
	for (auto& service : scManager.GetServicesByName(SERVICE_NAME))
	{
		if (service.find(SERVICE_UPDATE_NAME) != std::wstring::npos)
		{
			updaterServices.push_back(service);
		}
	}

	// Pokud nejsou žádné StroJ Nody k aktualizaci, skonèí
	if (updaterServices.empty())
	{
		logger.LogError(L"No StroJ Updater Nodes.");
		return false;
	}

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
		return false;
	}

	// Získá nejnovìjší verzi StorJ Nodu
	std::wstring sourceServicePath = sourcesSrevicesPaths[0];
	for (auto& servicePath : sourcesSrevicesPaths)
	{
		if (serviceUpdater.IsVersionGreater(servicePath,sourceServicePath))
		{
			sourceServicePath = servicePath;
		}
	}

	// Create shadow copy
	if(!serviceUpdater.CreateShadowCopy(sourceServicePath))
	{
		logger.LogError(L"Error while creating shadow copy.");
		return false;
	}

	// Update Storj Node
	bool result = true;
	for (auto& service : allServices)
	{
		std::wstring servicePath = scManager.GetServicePathByName(service);
		if (serviceUpdater.NeedUpdate(sourceServicePath, servicePath))
		{
			// Stop service
			scManager.CustomStopServiceWithWait(service);
			// Update service
			if (!serviceUpdater.UpdateService(sourceServicePath, servicePath))
			{
				logger.LogError(std::format(L"Service update failed: {}", service));
				result = false;
			}
			// Start service
			scManager.CustomStartService(service);
		}
	}

	// Delete shadow copy
	if (!serviceUpdater.DeleteShadowCopy(sourceServicePath))
	{
		logger.LogError(L"Error while deleting shadow copy.");
		return false;
	}

	if (!result)
	{
		logger.LogError(L"Service update failed.");
		return false;
	}

	logger.LogInfo(L"Check and update completed.");
	return true;
}
