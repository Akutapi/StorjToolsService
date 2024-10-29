#include "LogReducer.h"
#include <fstream>
#include <iostream>
#include <filesystem>

LogReducer::LogReducer(Logger& logger) : m_logger(logger)
{
}

LogReducer::~LogReducer()
{
}

bool LogReducer::ReduceLogToTemp(const std::wstring& logFilePath, const size_t maxFileSize, const size_t reducedSize)
{
	// kontrola spr�vnosti vstup�
	if (maxFileSize <= reducedSize)
	{
		m_logger.LogError(L"Invalid parameters: maxFileSize <= reducedSize");
		return false;
	}

	// Kontrola, zda soubor existuje
	if (!std::filesystem::exists(logFilePath))
	{
		m_logger.LogError(std::format(L"File does not exist: {}", logFilePath));
		return false;
	}

	// kontrola aktu�ln� velikost souboru
	size_t currentSize = std::filesystem::file_size(logFilePath);
	if (currentSize <= maxFileSize)
	{
		// Pokud je velikost souboru men�� nebo rovna maxim�ln� velikosti, neprovede redukci
		m_logger.LogError(std::format(L"File size did not exceed the maximum: {}", logFilePath));
		return true;
	}

	//Otev�en� souboru pro �ten� a z�pis
	std::wstring tempFilePath = std::filesystem::path(logFilePath).parent_path() / L"temp.txt";
	std::ifstream logFile(logFilePath);
	std::ofstream tempLogFile(tempFilePath);
	if (!logFile.is_open() || !tempLogFile.is_open())
	{
		m_logger.LogError(std::format(L"Failed to open file: {}", logFilePath));
		return false;
	}

	//nastav kurzor na spravnou velikost souboru
	logFile.seekg(currentSize - reducedSize, std::ios::beg);

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
	std::wstring tempFilePath = std::filesystem::path(logFilePath).parent_path() / L"temp.txt";
	// Odstra� star� soubor a p�ejmenuj nov�
	std::filesystem::remove(logFilePath);
	std::filesystem::rename(tempFilePath, logFilePath);
	return true;
}
