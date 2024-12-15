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

    // Konfiguraèní hodnoty
    int configVersion;
    int reduceLogTimeInHours;
    float maxLogSize;
    float reduceLogSize;
    int checkStorjNodesTimeInHours;
    int checkStorjNodeUpdateTimeInHours;
};
