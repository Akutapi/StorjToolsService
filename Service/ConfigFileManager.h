#pragma once
#include "Logger.h"
#include <filesystem>
class ConfigFileManager
{
public:
	ConfigFileManager(Logger& logger,std::filesystem::path path);
	~ConfigFileManager();
	bool LoadConfigFile();
	size_t GetMaxLogSize();
	size_t GetReduceLogSize();
	int GetReduceLogTimeInHours();
private:
	void ReadConfigFile();
	void CreateConfigFile();
	int m_maxLogSize;
	int m_reduceLogSize;
	int m_reduceLogTimeInHours;
	Logger& m_logger;
	std::filesystem::path m_configFilePath;
};

