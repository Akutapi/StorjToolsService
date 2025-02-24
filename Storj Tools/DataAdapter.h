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
	static DataAdapter& Instance(){	static DataAdapter instance; return instance;}
	void Init();

	// ServisStatus
	std::wstring GetServiceStatus();
	void RestartService();

	// Nastaven� z konfigura�n�ho souboru
	float GetNodesUpdateInterval();
	void SetNodesUpdateInterval(float hours);
	float GetCheckNodesInterval();
	void SetCheckNodesInterval(float hours);
	float GetLogReductionInterval();
	void SetLogReductionInterval(float hours);
	float GetMaxLogSize();
	void SetMaxLogSize(float size);
	float GetLogReducedSize();
	void SetLogReducedSize(float size);
	std::string GetDiscordUserID();
	void SetDiscordUserID(const std::string& userID);
	std::string GetDiscordBotToken();
	void SetDiscordBotToken(const std::string& botToken);
	void SaveSettings();
	void LoadSettings();

	// Discord
	void SendTestMessageToDiscord();


private:
	DataAdapter();
	~DataAdapter();
	std::mutex mtx;
	Logger logger;
	ConfigFileManager configManager;
	DiscordManager discordManager;
	SCManager scManager;
	ServiceUpdater serviceUpdater;	

	std::wstring serviceStatus = L"svefwef";
};

