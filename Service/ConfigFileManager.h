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
    size_t GetMaxLogSize() const;
    size_t GetReduceLogSize() const;
    int GetCheckStrojNodesTimeInHours() const;
    int GetCheckStrojNodeUpdateTimeInHours() const;
    

private:
    void ReadConfigFile();
    void CreateConfigFile();

    Logger& logger;
    std::filesystem::path configFilePath;

    // Konfiguraèní hodnoty
    int configVersion;
    int reduceLogTimeInHours;
    double maxLogSize;
    double reduceLogSize;
    int checkStrojNodesTimeInHours;
    int checkStrojNodeUpdateTimeInHours;
};
