/*
Soubor:  Logger.cpp
Popis:   Tøída pro logování do konzole a do event logu
Autor:   Akutapi
Datum:   30-11-2024
*/
#include "Logger.h"
#include <windows.h>
#include <format>
#include <iostream>

Logger::Logger(const std::wstring& _serviceName) : serviceName(_serviceName)
{
}

Logger::~Logger()
{
}

void Logger::LogInfo(const std::wstring& logMessage)
{
	WriteLog(logMessage, EVENTLOG_INFORMATION_TYPE);
}

void Logger::LogError(const std::wstring& logMessage)
{
	std::wstring ErrorMessage = std::format(L"{} Failed with: {}", logMessage, GetLastError());
	WriteLog(ErrorMessage, EVENTLOG_ERROR_TYPE);
}

void Logger::LogWarning(const std::wstring& logMessage)
{
	WriteLog(logMessage, EVENTLOG_WARNING_TYPE);
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
	WriteLog(ErrorMessage, EVENTLOG_ERROR_TYPE);
}

void Logger::WriteToEventLog(const std::wstring& logMessage, int eventType)
{
	LPCTSTR lpszStrings[2];
	HANDLE hEventSource = RegisterEventSource(NULL, serviceName.c_str());
	if (hEventSource)
	{
		lpszStrings[0] = serviceName.c_str();
		lpszStrings[1] = logMessage.c_str();
		ReportEvent(hEventSource, static_cast<WORD>(eventType), 0, 0, NULL, 2, 0, lpszStrings, NULL);
		DeregisterEventSource(hEventSource);
	}
}

void Logger::WriteLogToConsole(const std::wstring& logMessage, int eventType)
{
	// Výpis do konzole pro testování
	std::wstring event;
	switch (eventType)
	{
	case EVENTLOG_INFORMATION_TYPE:
		event = L"INFO: ";
		break;
	case EVENTLOG_ERROR_TYPE:
		event = L"ERROR: ";
		break;
	case EVENTLOG_WARNING_TYPE:
		event = L"WARNING: ";
		break;
	default:
		event = L"UNKNOWN: ";
		break;
	}
	std::wcout << event << logMessage << std::endl;
}

void Logger::WriteLog(const std::wstring& logMessage, int eventType)
{
	if (serviceName != L"Test")
	{
		WriteToEventLog(logMessage, eventType);
		return;
	}
	WriteLogToConsole(logMessage, eventType);
}

