#pragma once
#include <winrt/base.h>
#include "Logger.h"
#include "DiscordManager.h"
#include "SCManager.h"
#include "ConfigFileManager.h"

class DataAdapter
{
public:
	DataAdapter();
	~DataAdapter();

	winrt::hstring GetStorjToolsServiceStatus();
	void RestartStorjToolsService();

	void SetLogReductionInterval(float interval);
	float GetLogReductionInterval();
	void SetMaxLogSize(float size);
	float GetMaxLogSize();
	void SetLogReductionSize(float size);
	float GetLogReductionSize();

	void SetCheckStorjNodesInterval(float interval);
	float GetCheckStorjNodesInterval();

	void SetDiscordUserID(winrt::hstring userID);
	winrt::hstring GetDiscordUserID();
	void SetDiscordBotToken(winrt::hstring botToken);
	winrt::hstring GetDiscordBotToken();
	void SendDiscordMessage(winrt::hstring message);


	void SetCheckStorjNodeUpdateInterval(float interval);
	float GetCheckStorjNodeUpdateInterval();

	void SaveConfig();


	struct Node	
	{
		winrt::hstring name;
		winrt::hstring path;
		winrt::hstring status;
		winrt::hstring version;
	};

	void StartNode(winrt::hstring serviceName);
	void updateNodesStatus();
	std::vector<Node> GetNodes();

private:

	float logReductionInterval;
	float maxLogSize;
	float logReductionSize;

	float checkStorjNodesInterval;

	winrt::hstring discordUserID;
	winrt::hstring discordBotToken;

	float checkStorjNodeUpdateInterval;

	std::vector<Node> nodes;

	void LoadConfig();

	DiscordManager discordManager;
	SCManager scManager;
	ConfigFileManager configManager;

	winrt::hstring ConvertToHString(const std::wstring& str) {return winrt::hstring(str);}
	std::wstring ConvertFromHString(const winrt::hstring& hstr) {return std::wstring(hstr.c_str());}
};

