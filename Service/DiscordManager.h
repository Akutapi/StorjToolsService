/*
Soubor: DiscordManager.h
Popis: Tøída pro obsluhu Discordu.
Autor: Akutapi
Datum: 1-2-20204
*/
#pragma once
#include "Logger.h"
#include <nlohmann/json.hpp>
#include <curl/curl.h>
class DiscordManager
{

public:
	DiscordManager(Logger& logger);
	~DiscordManager();

	bool SetDiscordBot(std::string token, std::string userID);
	bool sendDM(std::string message);
	bool sendDM(std::wstring message);

private:
	bool sendMsg(const std::string& message);
	std::string StringToUTF8(const std::string& s);
	std::wstring StringToWString(const std::string& s);
	std::string WStringToString(const std::wstring& s);
	static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userData);
	Logger& logger;
	CURL* curl;
	struct curl_slist* headers = nullptr;
	std::string dmChannelId;
	std::string token;
	std::string userID;
};

