/*
Soubor: ConfigFileManager.h
Popis: T��da pro spr�vu konfigura�n�ho souboru.
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
    int GetReduceLogTimeInHours() const;
    std::uintmax_t GetMaxLogSize() const;
    std::uintmax_t GetReduceLogSize() const;
    int GetCheckStorjNodesTimeInHours() const;
    int GetCheckStorjNodeUpdateTimeInHours() const;
    

private:
	void ReadConfigFile();
    std::uintmax_t ConvertGBToBytes(float gb) const;
    void CreateConfigFile();

    Logger& logger;
    std::filesystem::path configFilePath;

    // Konfigura�n� hodnoty
    int configVersion;
    int reduceLogTimeInHours;
    float maxLogSize;
    float reduceLogSize;
    int checkStorjNodesTimeInHours;
    int checkStorjNodeUpdateTimeInHours;
};
