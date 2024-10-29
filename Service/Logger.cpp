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