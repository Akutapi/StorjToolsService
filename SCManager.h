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
	void CustomStopService(const std::wstring& serviceName);
	bool CustomPauseService(const std::wstring& serviceName);
	void CustomPauseServiceWithWait(const std::wstring& serviceName);
	void CustomContinueService(const std::wstring& serviceName);
	std::vector<std::wstring> GetServicesByName(const std::wstring& serviceName);
	std::filesystem::path GetServicePathByName(const std::wstring& serviceName);
private:
	SC_HANDLE m_hSCManager;
	Logger& m_logger;
};

