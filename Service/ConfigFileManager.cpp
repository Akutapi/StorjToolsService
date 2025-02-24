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
#define CONFIG_VERSION 2

ConfigFileManager::ConfigFileManager(Logger& _logger) : logger(_logger), configVersion(CONFIG_VERSION), reduceLogTimeInHours(24), maxLogSize(5), reduceLogSize(0.5), checkStorjNodesTimeInHours(1), discordUserID("YOUR_USER_ID"), discordBotToken("YOUR_BOT_TOKEN"), checkStorjNodeUpdateTimeInHours(12)
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

void ConfigFileManager::SaveConfig()
{
	CreateConfigFile();
	return;
}

float ConfigFileManager::GetReduceLogTimeInHours() const
{
    return reduceLogTimeInHours;
}

void ConfigFileManager::SetReduceLogTimeInHours(float hours)
{
	reduceLogSize = hours;
}

std::uintmax_t ConfigFileManager::GetMaxLogSize() const
{
    return ConvertGBToBytes(maxLogSize);
}

float ConfigFileManager::GetMaxLogSizeAsFLoat() const
{
	return maxLogSize;
}

void ConfigFileManager::SetMaxLogSize(float size)
{
	maxLogSize = size;
}

float ConfigFileManager::GetReduceLogSizeAsFloat() const
{
	return reduceLogSize;
}

std::uintmax_t ConfigFileManager::GetReduceLogSize() const
{
    return ConvertGBToBytes(reduceLogSize);
}

void ConfigFileManager::SetReduceLogSize(float size)
{
	reduceLogSize = size;
}

float ConfigFileManager::GetCheckStorjNodesTimeInHours() const
{
    return checkStorjNodesTimeInHours;
}

void ConfigFileManager::SetCheckStorjNodesTimeInHours(float hours)
{
	checkStorjNodesTimeInHours = hours;
}

std::string ConfigFileManager::GetDiscordUserID() const
{
    return discordUserID;
}

void ConfigFileManager::SetDiscordUserID(const std::string& userID)
{
	discordUserID = userID;
}

std::string ConfigFileManager::GetDiscordBotToken() const
{
    return discordBotToken;
}

void ConfigFileManager::SetDiscordBotToken(const std::string& botToken)
{
	discordBotToken = botToken;
}

float ConfigFileManager::GetCheckStorjNodeUpdateTimeInHours() const
{
    return checkStorjNodeUpdateTimeInHours;
}

void ConfigFileManager::SetCheckStorjNodeUpdateTimeInHours(float hours)
{
	checkStorjNodeUpdateTimeInHours = hours;
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
    if (config["Reduce Log Interval"])
    {
        reduceLogTimeInHours = config["Reduce Log Interval"].as<float>();
    }
    else
    {
        logger.LogWarning(L"Config file: reduceLogTimeInHours not found, using default value.");
    }

    if (config["Max Log File Size"])
    {
        maxLogSize = config["Max Log File Size"].as<float>();
    }
    else
    {
        logger.LogWarning(L"Config file: maxLogSizeInGB not found, using default value.");
    }

    if (config["Reduce Log File to Size"])
    {
        reduceLogSize = config["Reduce Log File to Size"].as<float>();
    }
    else
    {
        logger.LogWarning(L"Config file: reduceLogSizeInGB not found, using default value.");
    }

    if (config["Check StorjNodes Interval"])
    {
        checkStorjNodesTimeInHours = config["Check StorjNodes Interval"].as<float>();
    }
    else
    {
        logger.LogWarning(L"Config file: checkStrojNodesTimeInHours not found, using default value.");
    }

	if (config["Discord User ID"])
	{
		discordUserID = config["Discord User ID"].as<std::string>();
	}
	else
	{
		logger.LogWarning(L"Config file: discordUserID not found");
	}

	if (config["Discord Bot Token"])
	{
		discordBotToken = config["Discord Bot Token"].as<std::string>();
	}
	else
	{
		logger.LogWarning(L"Config file: discordBotToken not found");
	}

    if (config["Update StorjNode Interval"])
    {
        checkStorjNodeUpdateTimeInHours = config["Update StorjNode Interval"].as<float>();
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

    if (fout.is_open())
    {
        YAML::Emitter out;
        out << YAML::Comment("Stroj Tools Service Configuration File");
        out << YAML::Newline << YAML::Comment("You can disable this feature by setting the timer value to 0.");
        out << YAML::Newline << YAML::Comment("Intervals are in hours and log file sizes in gigabytes.");
        out << YAML::Newline << YAML::Newline << YAML::Comment("Configuration Version. Do not modify.");
        out << YAML::BeginMap;
        out << YAML::Key << "Config Version" << YAML::Value << CONFIG_VERSION;
        out << YAML::Newline << YAML::Newline << YAML::Comment("Log Maintenance Configuration") << YAML::Newline;
        out << YAML::Key << "Reduce Log Interval" << YAML::Value << reduceLogTimeInHours;
        out << YAML::Key << "Max Log File Size" << YAML::Value << maxLogSize;
        out << YAML::Key << "Reduce Log File to Size" << YAML::Value << reduceLogSize;
        out << YAML::Newline << YAML::Newline << YAML::Comment("Storj Nodes Health Check Configuration") << YAML::Newline;
        out << YAML::Key << "Check StorjNodes Interval" << YAML::Value << checkStorjNodesTimeInHours;
        out << YAML::Key << "Discord User ID" << YAML::Value << discordUserID;
        out << YAML::Key << "Discord Bot Token" << YAML::Value << discordBotToken;
        out << YAML::Newline << YAML::Newline << YAML::Comment("Storj Nodes Update Check Configuration") << YAML::Newline;
        out << YAML::Key << "Update StorjNode Interval" << YAML::Value << checkStorjNodeUpdateTimeInHours;
        out << YAML::EndMap;
        fout << out.c_str();
        fout.close();
    }
    else
	{
		logger.LogError(L"Error: Unable to write to the config file.");
	}
}