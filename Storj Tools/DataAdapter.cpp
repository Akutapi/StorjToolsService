#include "pch.h"
#include "DataAdapter.h"

void DataAdapter::Init()
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.UpdateConfig();
	discordManager.SetDiscordBot(configManager.GetDiscordBotToken(), configManager.GetDiscordUserID());
}

std::wstring DataAdapter::GetTest()
{
	std::lock_guard<std::mutex> lock(mtx);
	return test;
}

void DataAdapter::SetTest(std::wstring text)
{
	std::lock_guard<std::mutex> lock(mtx);
	test = text;
}

DataAdapter::DataAdapter() : logger(L"Debug"), configManager(logger), discordManager(logger), scManager(logger), serviceUpdater(logger)
{
}

DataAdapter::~DataAdapter()
{
}

