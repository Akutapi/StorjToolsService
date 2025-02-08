/*
 Soubor:  Tools.h
 Popis:  Tøída spravující nástroje pro službu.
 Autor:  Akutapi
 Datum:  6-12-2024
*/

#pragma once
#include "ConfigFileManager.h"
#include "LogReducer.h"
#include "SCManager.h"
#include "Logger.h"
#include "ServiceUpdater.h"
#include "DiscordManager.h"
#include <functional>

class Tools
{
public:
	struct TimerInfo
    {
		HANDLE hTimer; // handle na timer
		bool isRunning; // zda je timer spuštìn
		float interval; // v hodinách
		std::function<void()> callback; // funkce, která se má zavolat
		std::function<float(const ConfigFileManager&)> getConfigInterval; // funkce pro získání intervalu z konfiguraèního souboru

		float GetNewInterval(const ConfigFileManager& config) const
		{
			return getConfigInterval(config);
		}
    };

	Tools(Logger& logger);
	~Tools();
	std::vector<TimerInfo> GetTimers();
	ConfigFileManager& UpdateConfigFromFile();
	bool LogMaintenance();
	bool CheckStorjNodesStatus();
	bool CheckStorjNodeUpdate();

private:
	// enum pro množinu služeb
	enum SERVICE_SET { ALL, ALL_TO_MAINTENANCE, ALL_TO_CHECK, ALL_TO_UPDATE, UPDATERS};
	std::vector<std::wstring> GetServices(const SERVICE_SET);

	// struktura služby a jeho stavu updatu
	// enum status pokroku updatu
	enum UPDATE_STATUS { RUNNING_OLD, STOPED_OLD, STOPED_UPDATED, RUNNING_UPDATED };
	struct ServiceStatus
	{
		std::wstring serviceName;
		UPDATE_STATUS status;
	};

	// struktura služby a jeho stavu údržby
	// enum pro stav údržby
	enum MAINTENANCE_STATUS { INIT, TEMPLATED, STOPPED, REPLACED, STARTED };
	struct MaintenanceStatus
	{
		std::wstring serviceName;
		MAINTENANCE_STATUS status;
	};
	bool needsUpdate(const std::vector<ServiceStatus>& services) const;
	bool needsStart(const std::vector<ServiceStatus>& services) const;
	bool needsMaintenance(const std::vector<MaintenanceStatus>& services) const;
	std::vector<ServiceStatus> SetServiceStatusForCheck(const std::vector<std::wstring> services);
	std::vector<ServiceStatus> SetServiceStatusForUpdate(const std::vector<std::wstring> services);
	std::wstring StatusServiceToString(const UPDATE_STATUS status);
	std::wstring StatusServiceToString(const MAINTENANCE_STATUS status);
	std::vector<MaintenanceStatus> SetServiceStatusForMaintenance(const std::vector<std::wstring> services);
	std::wstring GetSorceServicePath(const std::vector<std::wstring>& updaterServices);
	DiscordManager discordManager;
	ConfigFileManager config;
	void SetDiscordManager();
	SCManager scManager;
	ServiceUpdater serviceUpdater;
	Logger logger;
	LogReducer logReducer;
	
};


