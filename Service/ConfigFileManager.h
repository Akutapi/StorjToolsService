/*
Soubor: ConfigFileManager.h
Popis: Tøída pro správu konfiguraèního souboru.
Autor: Akutapi
Datum: 15-12-20204
*/
#pragma once
#include <filesystem>
#include "Logger.h"

class ConfigFileManager
{
public:
    ConfigFileManager(Logger& logger);
    ~ConfigFileManager();

    void UpdateConfig();
    float GetReduceLogTimeInHours() const;
    std::uintmax_t GetMaxLogSize() const;
    std::uintmax_t GetReduceLogSize() const;
    float GetCheckStorjNodesTimeInHours() const;
    std::string GetDiscordUserID() const;
    std::string GetDiscordBotToken() const;
    float GetCheckStorjNodeUpdateTimeInHours() const;
    

private:
	void ReadConfigFile();
    std::uintmax_t ConvertGBToBytes(float gb) const;
    void CreateConfigFile();

    Logger& logger;
    std::filesystem::path configFilePath;

    // Konfiguraèní hodnoty
    int configVersion;
    float reduceLogTimeInHours;
    float maxLogSize;
    float reduceLogSize;
    float checkStorjNodesTimeInHours;
    std::string discordUserID;
    std::string discordBotToken;
    float checkStorjNodeUpdateTimeInHours;
};
