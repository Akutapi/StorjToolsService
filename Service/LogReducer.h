/*
Soubor: LogReducer.h
Popis: T��da pro redukci logovac�ho souboru
Autor: Akutapi
Datum: 15-12-2024
*/
#pragma once
#include "Logger.h"
class LogReducer
{
public:
	LogReducer(Logger& logger);
	~LogReducer();
	bool ReduceLogToTemp(const std::wstring& logsPath, const std::uintmax_t maxSize, const std::uintmax_t reducedSize );
	bool ReplaceLogWithTempFile(const std::wstring& logsPath);
private:
	Logger& logger;
};

