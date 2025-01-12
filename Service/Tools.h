/*
 Soubor:  Tools.h
 Popis:  T��da spravuj�c� n�stroje pro slu�bu.
 Autor:  Akutapi
 Datum:  6-12-2024
*/

#pragma once
#include "ConfigFileManager.h"
#include "LogReducer.h"
#include "SCManager.h"
#include "Logger.h"
#include "ServiceUpdater.h"
#include <vector>
#include <functional>

class Tools
{
public:
	struct TimerInfo
    {
		HANDLE hTimer; // handle na timer
		bool isRunning; // zda je timer spu�t�n
		int interval; // v hodin�ch
		std::function<void()> callback; // funkce, kter� se m� zavolat
		std::function<int(const ConfigFileManager&)> getConfigInterval; // funkce pro z�sk�n� intervalu z konfigura�n�ho souboru

		int GetNewInterval(const ConfigFileManager& config) const
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
	SCManager scManager;
	ServiceUpdater serviceUpdater;
	Logger logger;
	LogReducer logReducer;
	ConfigFileManager config;
};


