#include "pch.h"
#include "DataAdapter.h"
#include <string>

DataAdapter::DataAdapter() : logger(L"Debug"), configManager(logger), discordManager(logger), scManager(logger), serviceUpdater(logger)
{
}

DataAdapter::~DataAdapter()
{
}

void DataAdapter::Init()
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.UpdateConfig();
	discordManager.SetDiscordBot(configManager.GetDiscordBotToken(), configManager.GetDiscordUserID());
}

std::wstring DataAdapter::GetServiceStatus()
{
	std::lock_guard<std::mutex> lock(mtx);
	return serviceStatus;
}

void DataAdapter::RestartService()
{
	std::lock_guard<std::mutex> lock(mtx);
	OutputDebugString(L"RestartService byl vyžádán\n");
}

float DataAdapter::GetNodesUpdateInterval()
{
	std::lock_guard<std::mutex> lock(mtx);
	return configManager.GetCheckStorjNodeUpdateTimeInHours();
}

void DataAdapter::SetNodesUpdateInterval(float hours)
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SetCheckStorjNodeUpdateTimeInHours(hours);
}

float DataAdapter::GetCheckNodesInterval()
{
	std::lock_guard<std::mutex> lock(mtx);
	return configManager.GetCheckStorjNodesTimeInHours();
}

void DataAdapter::SetCheckNodesInterval(float hours)
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SetCheckStorjNodesTimeInHours(hours);
}

float DataAdapter::GetLogReductionInterval()
{
	std::lock_guard<std::mutex> lock(mtx);
	return configManager.GetReduceLogTimeInHours();
}

void DataAdapter::SetLogReductionInterval(float hours)
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SetReduceLogTimeInHours(hours);
}

float DataAdapter::GetMaxLogSize()
{
	std::lock_guard<std::mutex> lock(mtx);
	return configManager.GetMaxLogSizeAsFLoat();
}

void DataAdapter::SetMaxLogSize(float size)
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SetMaxLogSize(size);
}

float DataAdapter::GetLogReducedSize()
{
	std::lock_guard<std::mutex> lock(mtx);
	return configManager.GetReduceLogSizeAsFloat();
}

void DataAdapter::SetLogReducedSize(float size)
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SetReduceLogSize(size);
}

std::string DataAdapter::GetDiscordUserID()
{
	std::lock_guard<std::mutex> lock(mtx);
	return configManager.GetDiscordUserID();
}

void DataAdapter::SetDiscordUserID(const std::string& userID)
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SetDiscordUserID(userID);	
}

std::string DataAdapter::GetDiscordBotToken()
{
	std::lock_guard<std::mutex> lock(mtx);
	return configManager.GetDiscordBotToken();
}

void DataAdapter::SetDiscordBotToken(const std::string& botToken)
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SetDiscordBotToken(botToken);
}

void DataAdapter::SaveSettings()
{
	configManager.SaveConfig();
}

void DataAdapter::LoadSettings()
{
	configManager.UpdateConfig();
}

void DataAdapter::SendTestMessageToDiscord()
{
	std::lock_guard<std::mutex> lock(mtx);
	discordManager.SetDiscordBot(configManager.GetDiscordBotToken(), configManager.GetDiscordUserID());
	discordManager.sendDM(L"Test Zpráva z GUI");
}
