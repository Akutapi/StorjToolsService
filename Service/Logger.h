#pragma once
#include <string>
class Logger
{
public:
	Logger(const std::wstring& serviceName);
	~Logger();
	void LogInfo(const std::wstring& logMessage);
	void LogError(const std::wstring& logMessage);
	void LogWarning(const std::wstring& logMessage);
    void LogTry(const std::wstring& logMessage, const std::string e);
private:
	std::wstring serviceName;
	void WriteToEventLog(const std::wstring& logMessage, int eventType);
	void WriteLogToConsole(const std::wstring& logMessage, int eventType);
	void WriteLog(const std::wstring& logMessage, int eventType);
};
