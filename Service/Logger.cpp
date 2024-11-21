#include "Logger.h"
#include <windows.h>
#include <format>
Logger::Logger(const std::wstring& serviceName) : m_serviceName(serviceName)
{
}

Logger::~Logger()
{
}

void Logger::LogInfo(const std::wstring& logMessage)
{
	WriteToEventLog(logMessage, EVENTLOG_INFORMATION_TYPE);
}

void Logger::LogError(const std::wstring& logMessage)
{
	std::wstring ErrorMessage = std::format(L"{} Failed with: {}", logMessage, GetLastError());
	WriteToEventLog(ErrorMessage, EVENTLOG_ERROR_TYPE);
}

void Logger::LogWarning(const std::wstring& logMessage)
{
	WriteToEventLog(logMessage, EVENTLOG_WARNING_TYPE);
}

void Logger::LogTry(const std::wstring& logMessage, const std::string e)
{
	int wideCharLen = MultiByteToWideChar(CP_UTF8, 0, e.c_str(), -1, nullptr, 0);
	if (wideCharLen == 0)
	{
		LogError(L"MultiByteToWideChar");
		return;
	}
	std::wstring wideCharStr(wideCharLen, 0);
	if (MultiByteToWideChar(CP_UTF8, 0, e.c_str(), -1, wideCharStr.data(), wideCharLen) == 0)
	{
		LogError(L"MultiByteToWideChar");
		return;
	}
	wideCharStr.resize(wideCharLen - 1);
	std::wstring ErrorMessage = std::format(L"{} Failed with: {}", logMessage, wideCharStr);
	WriteToEventLog(ErrorMessage, EVENTLOG_ERROR_TYPE);
}

void Logger::WriteToEventLog(const std::wstring& logMessage, int eventType)
{
	LPCTSTR lpszStrings[2];
	HANDLE hEventSource = RegisterEventSource(NULL, m_serviceName.c_str());
	if (hEventSource)
	{
		lpszStrings[0] = m_serviceName.c_str();
		lpszStrings[1] = logMessage.c_str();
		ReportEvent(hEventSource, eventType, 0, 0, NULL, 2, 0, lpszStrings, NULL);
		DeregisterEventSource(hEventSource);
	}
}