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
#include "DiscordManager.h"
#include <functional>

class Tools
{
public:
	struct TimerInfo
    {
		HANDLE hTimer; // handle na timer
		bool isRunning; // zda je timer spu�t�n
		float interval; // v hodin�ch
		std::function<void()> callback; // funkce, kter� se m� zavolat
		std::function<float(const ConfigFileManager&)> getConfigInterval; // funkce pro z�sk�n� intervalu z konfigura�n�ho souboru

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
	DiscordManager discordManager;
	ConfigFileManager config;
	void SetDiscordManager();
	SCManager scManager;
	ServiceUpdater serviceUpdater;
	Logger logger;
	LogReducer logReducer;
	
};


