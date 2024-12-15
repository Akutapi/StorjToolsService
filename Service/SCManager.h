#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <filesystem>
#include "Logger.h"
class SCManager
{
public:
	SCManager(Logger& logger);
	~SCManager();
	void CustomStartService(const std::wstring& serviceName);
	bool CustomStopService(const std::wstring& serviceName);
	bool CustomStopServiceWithWait(const std::wstring& serviceName);
	bool CustomPauseService(const std::wstring& serviceName);
	bool CustomPauseServiceWithWait(const std::wstring& serviceName);
	void CustomContinueService(const std::wstring& serviceName);
	DWORD GetServiceStatus(const std::wstring& serviceName);
	std::vector<std::wstring> GetServicesByName(const std::wstring& serviceName);
	std::filesystem::path GetServicePathByName(const std::wstring& serviceName);
private:
	SC_HANDLE hSCManager;
	Logger& logger;
	const int TIMEOUT_SECONDS = 60; // Maximální èas èekání
};

