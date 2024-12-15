#include "LogReducer.h"
#include <fstream>
#include <iostream>
#include <filesystem>

#define TEMP_FILE_NAME L"temp.txt"

LogReducer::LogReducer(Logger& _logger) : logger(_logger)
{
}

LogReducer::~LogReducer()
{
}

bool LogReducer::ReduceLogToTemp(const std::wstring& logFilePath, const std::uintmax_t maxFileSize, const std::uintmax_t reducedSize)
{
	// kontrola spr�vnosti vstup�
	if (maxFileSize <= reducedSize)
	{
		logger.LogError(L"Invalid parameters: maxFileSize <= reducedSize");
		return false;
	}

	// Kontrola, zda soubor existuje
	if (!std::filesystem::exists(logFilePath))
	{
		logger.LogError(std::format(L"File does not exist: {}", logFilePath));
		return false;
	}

	// kontrola aktu�ln� velikost souboru
	std::uintmax_t currentSize = std::filesystem::file_size(logFilePath);
	if (currentSize <= maxFileSize)
	{
		// Pokud je velikost souboru men�� nebo rovna maxim�ln� velikosti, neprovede redukci
		logger.LogError(std::format(L"File size did not exceed the maximum: {}", logFilePath));
		return false;
	}

	//Otev�en� souboru pro �ten� a z�pis
	std::wstring tempFilePath = std::filesystem::path(logFilePath).parent_path() / TEMP_FILE_NAME;
	std::ifstream logFile(logFilePath);
	std::ofstream tempLogFile(tempFilePath);
	if (!logFile.is_open() || !tempLogFile.is_open())
	{
		logger.LogError(std::format(L"Failed to open file: {}", logFilePath));
		return false;
	}

	//nastav kurzor na spravnou velikost souboru
	// P�evod rozd�lu na std::streamoff
	std::uintmax_t offset = currentSize - reducedSize;
	if (offset > static_cast<std::uintmax_t>(std::numeric_limits<std::streamoff>::max()))
	{
		logger.LogError(L"Offset exceeds the maximum representable value of std::streamoff");
		return false;
	}

	logFile.seekg(static_cast<std::streamoff>(offset), std::ios::beg);

	//nastav kurzor na n�sleduj�s� ��dek
	std::string line;
	std::getline(logFile, line);

	//kop�ruj zbytek souboru
	while (std::getline(logFile, line))
	{
		tempLogFile << line << std::endl;
	}

	//Zav�e souboru
	logFile.close();
	tempLogFile.close();
	return true;
}

bool LogReducer::ReplaceLogWithTempFile(const std::wstring& logFilePath)
{
	std::wstring tempFilePath = std::filesystem::path(logFilePath).parent_path() / TEMP_FILE_NAME;

	// Kontrola, zda existuje p�vodn� log soubor
	if (!std::filesystem::exists(logFilePath))
	{
		logger.LogError(std::format(L"Original log file does not exist: {}", logFilePath));
		return false;
	}

	// Kontrola, zda existuje do�asn� soubor
	if (!std::filesystem::exists(tempFilePath))
	{
		logger.LogError(std::format(L"Temporary log file does not exist: {}", tempFilePath));
		return false;
	}
	// Odstra�uje star� soubor a p�ejmenuje nov�
	std::filesystem::remove(logFilePath);
	std::filesystem::rename(tempFilePath, logFilePath);
	return true;
}
