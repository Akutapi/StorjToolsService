#pragma once
#include <mutex>

#include "Logger.h"
#include "ConfigFileManager.h"
#include "DiscordManager.h"
#include "SCManager.h"
#include "ServiceUpdater.h"

class DataAdapter
{
public:

	// Zakázání kopírování a pøiøazení
	DataAdapter(const DataAdapter&) = delete;
	DataAdapter& operator=(const DataAdapter&) = delete;

	// Získání instance tøídy
	static DataAdapter& Instance(){	static DataAdapter instance;return instance;}
	void Init();

	std::wstring GetTest();
	void SetTest(std::wstring text);

private:
	DataAdapter();
	~DataAdapter();
	std::mutex mtx;
	Logger logger;
	ConfigFileManager configManager;
	DiscordManager discordManager;
	SCManager scManager;
	ServiceUpdater serviceUpdater;	

	std::wstring test;
};

