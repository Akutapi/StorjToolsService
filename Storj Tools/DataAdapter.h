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

	// Zak�z�n� kop�rov�n� a p�i�azen�
	DataAdapter(const DataAdapter&) = delete;
	DataAdapter& operator=(const DataAdapter&) = delete;

	// Z�sk�n� instance t��dy
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

