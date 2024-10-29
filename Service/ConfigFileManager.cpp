#include "ConfigFileManager.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>

#define GB(x) ((size_t)(x) * 1024 * 1024 * 1024)

ConfigFileManager::ConfigFileManager(Logger& logger, std::filesystem::path path) : m_maxLogSize(5), m_reduceLogSize(1), m_reduceLogTimeInHours(24), m_logger(logger)
{
	m_configFilePath = path.parent_path() / L"config.yaml";
}

ConfigFileManager::~ConfigFileManager()
{
}

bool ConfigFileManager::LoadConfigFile()
{
	ReadConfigFile();
	return true;
}

size_t ConfigFileManager::GetMaxLogSize()
{
	return GB(m_maxLogSize);
}

size_t ConfigFileManager::GetReduceLogSize()
{
	return GB(m_reduceLogSize);
}

int ConfigFileManager::GetReduceLogTimeInHours()
{
	return m_reduceLogTimeInHours;
}

void ConfigFileManager::ReadConfigFile()
{
	// kontrola existence souboru
	if (!std::filesystem::exists(m_configFilePath))
	{
		CreateConfigFile();
		return;
	}
	YAML::Node config = YAML::LoadFile(m_configFilePath.string());
	m_maxLogSize = config["maxLogSizeInGB"].as<int>();
	m_reduceLogSize = config["reduceLogSizeInGB"].as<int>();
	m_reduceLogTimeInHours = config["reduceLogTimeInHours"].as<int>();
}

void ConfigFileManager::CreateConfigFile()
{
	std::ofstream fout(m_configFilePath);
	if (!fout)
	{
		m_logger.LogError(L"Error: Unable to create config file.");
		return;
	}
	YAML::Node config;
	config["maxLogSizeInGB"] = m_maxLogSize;
	config["reduceLogSizeInGB"] = m_reduceLogSize;
	config["reduceLogTimeInHours"] = m_reduceLogTimeInHours;

	fout << config;
	fout.close();
}