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
	DiscordManager discordManager;
	ConfigFileManager config;
	void SetDiscordManager();
	SCManager scManager;
	ServiceUpdater serviceUpdater;
	Logger logger;
	LogReducer logReducer;
	
};


