#include "pch.h"
#include "DataAdapter.h"
#include <string>

#define SERVICE_TOOL_NAME L"Storj Tools Service"
#define SERVICE_NAME L"storagenode"
#define SERVICE_UPDATE_NAME L"updater"
#define EXE_SERVICE_NAME L"storagenode.exe"

DataAdapter::DataAdapter() : logger(L"Debug"), configManager(logger), discordManager(logger), scManager(logger), serviceUpdater(logger)
{
}

DataAdapter::~DataAdapter()
{
}

wstring DataAdapter::StatusDwordToWstring(DWORD dw)
{
	switch (dw)
	{
	case SERVICE_STOPPED:
		return L"Stopped";
	case SERVICE_START_PENDING:
		return L"Start pending";
	case SERVICE_STOP_PENDING:
		return L"Stop pending";
	case SERVICE_RUNNING:
		return L"Running";
	case SERVICE_CONTINUE_PENDING:
		return L"Continue pending";
	case SERVICE_PAUSE_PENDING:
		return L"Pause pending";
	case SERVICE_PAUSED:
		return L"Paused";
	default:
		return L"Unknown";
	}
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
	return StatusDwordToWstring(scManager.GetServiceStatus(SERVICE_TOOL_NAME));
}

void DataAdapter::RestartService()
{
	std::lock_guard<std::mutex> lock(mtx);
	scManager.CustomStopServiceWithWait(SERVICE_TOOL_NAME);
	scManager.CustomStartServiceWithWait(SERVICE_TOOL_NAME);
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
	std::lock_guard<std::mutex> lock(mtx);
	configManager.SaveConfig();
}

void DataAdapter::LoadSettings()
{
	std::lock_guard<std::mutex> lock(mtx);
	configManager.UpdateConfig();
}

void DataAdapter::SendTestMessageToDiscord()
{
	std::lock_guard<std::mutex> lock(mtx);
	discordManager.SetDiscordBot(configManager.GetDiscordBotToken(), configManager.GetDiscordUserID());
	discordManager.sendDM(L"Test Zpráva z GUI");
}

vector<wstring> DataAdapter::GetNodesNames()
{
	std::vector<std::wstring> services = scManager.GetServicesByName(SERVICE_NAME);
	std::vector<std::wstring> tempServices;
	for (auto& service : services)
	{
		if (service.find(SERVICE_UPDATE_NAME) == std::wstring::npos)
		{
			tempServices.push_back(service);
		}
	}
	services = tempServices;
	tempServices.clear();
	return services;
}

wstring DataAdapter::GetNodePath(wstring name)
{
	return scManager.GetServicePathByName(name);
}

wstring DataAdapter::GetNodeVersion(wstring name)
{
	return serviceUpdater.GetStringFileVersion(scManager.GetServicePathByName(name));
}

DWORD DataAdapter::GetNodeStatus(wstring name)
{
	return scManager.GetServiceStatus(name);
}

void DataAdapter::StartNode(wstring name)
{
	scManager.CustomStartServiceWithWait(name);
}
