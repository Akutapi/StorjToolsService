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
	void SaveConfig();
    float GetReduceLogTimeInHours() const;
	void SetReduceLogTimeInHours(float hours);
    std::uintmax_t GetMaxLogSize() const;
	void SetMaxLogSize(float size);
    std::uintmax_t GetReduceLogSize() const;
	void SetReduceLogSize(float size);
    float GetCheckStorjNodesTimeInHours() const;
	void SetCheckStorjNodesTimeInHours(float hours);
    std::string GetDiscordUserID() const;
	void SetDiscordUserID(const std::string& userID);
    std::string GetDiscordBotToken() const;
	void SetDiscordBotToken(const std::string& botToken);
    float GetCheckStorjNodeUpdateTimeInHours() const;
	void SetCheckStorjNodeUpdateTimeInHours(float hours);
    

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
