#pragma once
#include <string>
#include "Logger.h"
class LogReducer
{
public:
	LogReducer(Logger& logger);
	~LogReducer();
	bool ReduceLogToTemp(const std::wstring& logsPath, const size_t maxSize, const size_t reducedSize );
	bool ReplaceLogWithTempFile(const std::wstring& logsPath);
private:
	Logger& m_logger;
};

