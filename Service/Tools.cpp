#include "Tools.h"

// Konstanty
#define SERVICE_NAME L"storagenode"
#define SERVICE_UPDATE_NAME L"updater"
#define LOG_FILE_NAME L"storagenode.log"

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
	// Na�te konfigura�n� soubor
	config.UpdateConfig();

	// Z�sk� seznam slu�eb bez StorJ Update Nod�
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
	logger.LogInfo(std::format(L"Nalezeno StroJ Nod�: {}", services.size())); // test log

	for (auto& service : services)
	{
		logger.LogInfo(std::format(L"Service: {}", service)); // test log
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
			logger.LogInfo(std::format(L"Log file nen� t�eba redukovat: {}", logPath));	// test log
			continue;			
		}
		logger.LogInfo(std::format(L"Log file mus� b�t redukov�n: {}", logPath));    // test log
		servicesToMaintain.push_back(service);
	}
	// Pokud nejsou ��dn� StroJ Nody k redukci log soubor�, skon��
	if (servicesToMaintain.empty())
	{
		logger.LogInfo(L"No StroJ Nodes for log file reduction.");
		return true;
	}

	logger.LogInfo(std::format(L"Nalezeno StroJ Nod� k redukci log soubor�: {}", servicesToMaintain.size())); // test log
	// Pro ka�d� StroJ Nod provede redukci log souboru do temp souboru
	for (auto& service : servicesToMaintain)
	{
		// Redukce log souboru
		std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / LOG_FILE_NAME;
		if (logReducer.ReduceLogToTemp(logPath, config.GetMaxLogSize(), config.GetReduceLogSize()))
		{
			logger.LogInfo(std::format(L"Log soubor redukov�n: {}", logPath)); // test log
		}
		else
		{
			logger.LogError(std::format(L"Log soubor redukce selhala: {}", logPath)); // test log
			servicesToMaintain.erase(std::remove(servicesToMaintain.begin(), servicesToMaintain.end(), service), servicesToMaintain.end());	
		}
	}

	if (servicesToMaintain.empty())
	{
		logger.LogError(L"Log soubory redukce selhala pro v�echny StroJ Nody.");
		return false;
	}
	logger.LogInfo(L"Vytvo�eny do�asn� soubory s redukovanou velikost�."); // test log


	// Pro ka�d� StroJ Nod p�ep�e log soubor temp souborem
	for (auto& service : servicesToMaintain)
	{
		// Stop service
		logger.LogInfo(std::format(L"Zastavuji servis: {}", service)); // test log
		scManager.CustomStopServiceWithWait(service);
		logger.LogInfo(std::format(L"Servis zastaven: {}", service)); // test log
		// Nahrazen� log souboru temp souborem
		std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / LOG_FILE_NAME;
		if (logReducer.ReplaceLogWithTempFile(logPath))
		{
			logger.LogInfo(std::format(L"Log soubor nahrazen redukovan�m do�asn�m souborem.")); // test log
		}
		else
		{
			logger.LogError(std::format(L"Log soubor nahrazen� selhalo: {}", logPath)); // test log
		}
		// Start service
		logger.LogInfo(std::format(L"Spu�t�n� servisu: {}", service)); // test log
		scManager.CustomStartService(service);
		logger.LogInfo(std::format(L"Servis spu�t�n: {}", service)); // test log
	}
	logger.LogInfo(L"Maintenanc done.");
	return true;
}

bool Tools::CheckStorjNodesStatus()
{
	logger.LogInfo(L"Check Status Done.");
	return true;
}

bool Tools::CheckStorjNodeUpdate()
{
	logger.LogInfo(L"Check Update Done.");
	return true;
}
