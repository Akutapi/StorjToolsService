/*
Soubor: ConfigFileManager.cpp
Popis: Tøída pro správu konfiguraèního souboru.
       Tato tøída využívá knihovnu yaml-cpp pro práci s YAML soubory.
Autor: Akutapi
Datum: 15-12-20204
*/
#include "ConfigFileManager.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <Windows.h>

#define CONFIG_FILE_NAME L"config.yaml"
#define CONFIG_VERSION 1

ConfigFileManager::ConfigFileManager(Logger& _logger) : logger(_logger), configVersion(CONFIG_VERSION), reduceLogTimeInHours(24), maxLogSize(5), reduceLogSize(0.5), checkStorjNodesTimeInHours(1), checkStorjNodeUpdateTimeInHours(12)
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::filesystem::path exePath = std::filesystem::path(buffer).parent_path();
    configFilePath = exePath / CONFIG_FILE_NAME;
}

ConfigFileManager::~ConfigFileManager()
{
}

void ConfigFileManager::UpdateConfig()
{
    ReadConfigFile();
    return;
}

int ConfigFileManager::GetReduceLogTimeInHours() const
{
    return reduceLogTimeInHours;
}

std::uintmax_t ConfigFileManager::GetMaxLogSize() const
{
    return ConvertGBToBytes(maxLogSize);
}

std::uintmax_t ConfigFileManager::GetReduceLogSize() const
{
    return ConvertGBToBytes(reduceLogSize);
}

int ConfigFileManager::GetCheckStorjNodesTimeInHours() const
{
    return checkStorjNodesTimeInHours;
}

int ConfigFileManager::GetCheckStorjNodeUpdateTimeInHours() const
{
    return checkStorjNodeUpdateTimeInHours;
}

void ConfigFileManager::ReadConfigFile()
{
    // kontrola existence souboru
    if (!std::filesystem::exists(configFilePath))
    {
		logger.LogWarning(L"Config file does not exist.");
        CreateConfigFile();
		logger.LogWarning(L"Config file created.");
        return;
    }
    // naètení dat z konfiguraèního souboru
    YAML::Node config = YAML::LoadFile(configFilePath.string());
	if (!config)
	{
		logger.LogError(L"Unable to load config file.");
		return;
	}

    configVersion = config["Config Version"].as<int>();

    // kontrola verze konfiguraèního souboru
    if (configVersion != CONFIG_VERSION)
    {
		logger.LogWarning(L"Config file version mismatch.");
        CreateConfigFile();
		logger.LogWarning(L"Config file recreated to the correct version.");
        return;
    }

    // naètení konfiguraèních hodnot s kontrolou existence klíèù
    if (config["Reduce Log Time In Hours"])
    {
        reduceLogTimeInHours = config["Reduce Log Time In Hours"].as<int>();
    }
    else
    {
        logger.LogWarning(L"Config file: reduceLogTimeInHours not found, using default value.");
    }

    if (config["Max Log Size In GB"])
    {
        maxLogSize = config["Max Log Size In GB"].as<float>();
    }
    else
    {
        logger.LogWarning(L"Config file: maxLogSizeInGB not found, using default value.");
    }

    if (config["Reduce Log Size In GB"])
    {
        reduceLogSize = config["Reduce Log Size In GB"].as<float>();
    }
    else
    {
        logger.LogWarning(L"Config file: reduceLogSizeInGB not found, using default value.");
    }

    if (config["Check StrojNodes Time In Hours"])
    {
        checkStorjNodesTimeInHours = config["Check StrojNodes Time In Hours"].as<int>();
    }
    else
    {
        logger.LogWarning(L"Config file: checkStrojNodesTimeInHours not found, using default value.");
    }

    if (config["Check StrojNode Update Time In Hours"])
    {
        checkStorjNodeUpdateTimeInHours = config["Check StrojNode Update Time In Hours"].as<int>();
    }
    else
    {
        logger.LogWarning(L"Config file: checkStrojNodeUpdateTimeInHours not found, using default value.");
    }
}

std::uintmax_t ConfigFileManager::ConvertGBToBytes(float gb) const
{
    // Pøevedení GB na bajty
    return static_cast<std::uintmax_t>(std::abs(gb) * 1024 * 1024 * 1024);
}

void ConfigFileManager::CreateConfigFile()
{
    std::ofstream fout(configFilePath);
    if (!fout)
    {
        logger.LogError(L"Error: Unable to create config file.");
        return;
    }

    YAML::Emitter out;
    out << YAML::Comment("Stroj Tools Service Configuration File");
    out << YAML::Newline << YAML::Comment("You can disable this feature by setting the timer value to 0.");
    out << YAML::Newline << YAML::Newline << YAML::Comment("Configuration Version. Do not modify.");
	out << YAML::BeginMap;
    out << YAML::Key << "Config Version" << YAML::Value << configVersion;
    out << YAML::Newline << YAML::Newline << YAML::Comment("Log Maintenance Configuration") << YAML::Newline;
    out << YAML::Key << "Reduce Log Time In Hours" << YAML::Value << reduceLogTimeInHours;
    out << YAML::Key << "Max Log Size In GB" << YAML::Value << maxLogSize;
    out << YAML::Key << "Reduce Log Size In GB" << YAML::Value << reduceLogSize;
    out << YAML::Newline << YAML::Newline << YAML::Comment("Stroj Nodes Health Check Configuration: Function not supported") << YAML::Newline;
    out << YAML::Key << "Check StrojNodes Time In Hours" << YAML::Value << checkStorjNodesTimeInHours;
    out << YAML::Newline << YAML::Newline << YAML::Comment("Stroj Nodes Update Check Configuration: Function not supported") << YAML::Newline;
    out << YAML::Key << "Check StrojNode Update Time In Hours" << YAML::Value << checkStorjNodeUpdateTimeInHours;
    out << YAML::EndMap;
    fout << out.c_str();
    fout.close();
}