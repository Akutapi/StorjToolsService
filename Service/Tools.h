#pragma once
#include "ConfigFileManager.h"
#include "LogReducer.h"
#include "SCManager.h"
#include "Logger.h"
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
	bool CheckStrojNodesStatus();
	bool CheckStrojNodeUpdate();
private:
	Logger logger;
	LogReducer logReducer;
	SCManager scManager;
	ConfigFileManager config;
};


