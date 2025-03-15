#pragma once
#include <mutex>

#include "Logger.h"
#include "ConfigFileManager.h"
#include "DiscordManager.h"
#include "SCManager.h"
#include "ServiceUpdater.h"

using namespace std;

class DataAdapter
{
public:

	// Zakázání kopírování a pøiøazení
	DataAdapter(const DataAdapter&) = delete;
	DataAdapter& operator=(const DataAdapter&) = delete;

	// Získání instance tøídy
	static DataAdapter& Instance(){	static DataAdapter instance; return instance;}
	void Init();

	// ServisStatus
	wstring GetServiceStatus();
	void RestartService();

	// Nastavení z konfiguraèního souboru
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
	string GetDiscordUserID();
	void SetDiscordUserID(const string& userID);
	string GetDiscordBotToken();
	void SetDiscordBotToken(const string& botToken);
	void SaveSettings();
	void LoadSettings();

	// Discord
	void SendTestMessageToDiscord();

	// Nodes
	vector<wstring> GetNodesNames();
	wstring GetNodePath(wstring name);
	wstring GetNodeVersion(wstring name);
	DWORD GetNodeStatus(wstring name);
	void StartNode(wstring name);

private:
	DataAdapter();
	~DataAdapter();
	wstring StatusDwordToWstring(DWORD dw);
	mutex mtx;
	Logger logger;
	ConfigFileManager configManager;
	DiscordManager discordManager;
	SCManager scManager;
	ServiceUpdater serviceUpdater;	
};

