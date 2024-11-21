#include "Tools.h"

Tools::Tools(Logger& _logger) : logger(_logger) , logReducer(_logger), scManager(_logger), config(_logger)
{
}

Tools::~Tools()
{
}

std::vector<Tools::TimerInfo> Tools::GetTimers()
{	
	// aktualizuj konfigura�n� soubor
	config.UpdateConfig();
	// nastavit custom timery
	std::vector<TimerInfo> timers;
	timers.push_back({NULL, false, config.GetReduceLogTimeInHours(), [this]() { this->LogMaintenance(); }, [this](const ConfigFileManager& config) -> int { return config.GetReduceLogTimeInHours(); }});
	timers.push_back({NULL, false, config.GetCheckStrojNodesTimeInHours(), [this]() { this->CheckStrojNodesStatus(); }, [this](const ConfigFileManager& config) -> int { return config.GetCheckStrojNodesTimeInHours(); }});
	timers.push_back({NULL, false, config.GetCheckStrojNodeUpdateTimeInHours(), [this]() { this->CheckStrojNodeUpdate(); }, [this](const ConfigFileManager& config) -> int { return config.GetCheckStrojNodeUpdateTimeInHours(); }});
	return timers;
}

ConfigFileManager& Tools::UpdateConfigFromFile()
{
	config.UpdateConfig();
	return config;
}

bool Tools::LogMaintenance()
{

		// Na�te konfigura�n� soubor
		config.UpdateConfig();

		// Z�sk� seznam slu�eb
		std::vector<std::wstring> servicesAll = scManager.GetServicesByName(L"storagenode");
		if (servicesAll.empty())
		{
			logger.LogWarning(L"StroJ Nody nenalezeny.");
			return;
		}

		// Z�sk� seznam slu�eb bez StorJ Update Nod�
		std::vector<std::wstring> services;
		for (auto& service : servicesAll)
		{
			if (service.find(L"updater") == std::wstring::npos)
			{
				services.push_back(service);
			}
		}
		logger.LogInfo(std::format(L"Nalezeno StroJ Nod�: {}", services.size()));

		// kontrola existence souboru a jeho velikosti
		std::vector<std::wstring> servicesToMaintain;
		for (auto& service : services)
		{
			std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / L"storagenode.log";
			size_t currentSize = std::filesystem::file_size(logPath);
			if (std::filesystem::exists(logPath) && currentSize >= config.GetMaxLogSize())
			{
				servicesToMaintain.push_back(service);
			}
		}

		logger.LogInfo(std::format(L"Nalezeno StroJ Nod� k redukci log soubor�: {}", servicesToMaintain.size()));
		// Pro ka�d� StroJ Nod provede redukci log souboru do temp souboru
		for (auto& service : servicesToMaintain)
		{
			// Redukce log souboru
			std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / L"storagenode.log";
			logReducer.ReduceLogToTemp(logPath, config.GetMaxLogSize(), config.GetReduceLogSize());
		}

		// Pro ka�d� StroJ Nod p�ep�e log soubor temp souborem
		for (auto& service : servicesToMaintain)
		{
			// Pause service
			scManager.CustomPauseServiceWithWait(service);
			logger.LogInfo(std::format(L"Pausov�n: {}", service));
			// Redukce log souboru
			std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / L"storagenode.log";
			logReducer.ReplaceLogWithTempFile(logPath);
			// Continue service
			scManager.CustomContinueService(service);
			logger.LogInfo(std::format(L"Spou�t�n: {}", service));
		}
	logger.LogInfo(L"Maintenanc done.");
	return true;
}

bool Tools::CheckStrojNodesStatus()
{
	logger.LogInfo(L"Check Status Done.");
	return true;
}

bool Tools::CheckStrojNodeUpdate()
{
	logger.LogInfo(L"Check Update Done.");
	return true;
}
