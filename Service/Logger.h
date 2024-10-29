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
private:
	std::wstring m_serviceName;
	void WriteToEventLog(const std::wstring& logMessage, int eventType);
};

