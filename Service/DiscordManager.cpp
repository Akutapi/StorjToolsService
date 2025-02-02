/*
Soubor: DiscordManager.cpp
Popis: Tøída pro obsluhu Discordu.
Autor: Akutapi
Datum: 1-2-20204
*/
#include "DiscordManager.h"
#include <Windows.h>


DiscordManager::DiscordManager(Logger& _logger) : logger(_logger)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
}

DiscordManager::~DiscordManager()
{
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

bool DiscordManager::SetDiscordBot(std::string _token, std::string _userID)
{
	token = _token;
	userID = _userID;
	//nastavení hlavièky
	curl_slist_free_all(headers);
	headers = nullptr;
	headers = curl_slist_append(headers, ("Authorization: Bot " + token).c_str());
	headers = curl_slist_append(headers, "Content-Type: application/json");

	// Ovìøení existence uživatele
	curl_easy_reset(curl);
	std::string url = "https://discord.com/api/v10/users/" + userID;
	std::string response;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		logger.LogError(L"CURL error: " + StringToWString(curl_easy_strerror(res)));
		return false;
	}

	if (response.empty())
	{
		logger.LogError(L"Prázdná odpovìï ze serveru.");
		return false;
	}

	try
	{
		nlohmann::json j = nlohmann::json::parse(response);
		if (!j.contains("id"))
		{
			logger.LogError(L"Uživatel s ID " + StringToWString(userID) + L" neexistuje.");
			return false;
		}
	}
	catch (const nlohmann::json::parse_error& e)
	{
		logger.LogError(L"Chyba pøi parsování JSON: " + StringToWString(e.what()));
		return false;
	}

	// Vytvoøení DM kanálu
	curl_easy_reset(curl);
	url = "https://discord.com/api/v10/users/@me/channels";
	response.clear();
	nlohmann::json body;
	body["recipient_id"] = userID;
	std::string bodyStr = body.dump();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bodyStr.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	// Odeslání POST požadavku pro vytvoøení DM kanálu
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		logger.LogError(L"CURL error pøi vytváøení DM kanálu: " + StringToWString(curl_easy_strerror(res)));
		return false;
	}


	if (response.empty())
	{
		logger.LogError(L"Prázdná odpovìï ze serveru.");
		return false;
	}

	try
	{
		nlohmann::json dmResponse = nlohmann::json::parse(response);
		if (dmResponse.contains("id"))
		{
			dmChannelId = dmResponse["id"].get<std::string>();
			logger.LogInfo(L"DM kanál s uživatelem " + StringToWString(userID) + L" byl úspìšnì vytvoøen. ID kanálu: " + StringToWString(dmChannelId));
		}
		else
		{
			logger.LogError(L"Nepodaøilo se získat ID DM kanálu.");
			return false;
		}
	}
	catch (const nlohmann::json::parse_error& e)
	{
		logger.LogError(L"Chyba pøi parsování JSON: " + StringToWString(e.what()));
		return false;
	}

	// Uvolnìní hlavièek
	curl_slist_free_all(headers);
	headers = nullptr;
	if (firstSet)
	{
		firstSet = false;
		logger.LogInfo(L"Discord bot byl úspìšnì nastaven.");
		sendDM(L"The Storj service has started.");
	}
	return true;
}

bool DiscordManager::sendDM(std::string message)
{
	return sendMsg(StringToUTF8(message));
}

bool DiscordManager::sendDM(std::wstring message)
{
	return sendMsg(WStringToString(message));
}

bool DiscordManager::sendMsg(const std::string& message)
{
	try
	{
		if (dmChannelId.empty())
		{
			logger.LogError(L"DM kanál není inicializován.");
			return false;
		}
		// Inicializace hlavièek pro každý požadavek
		curl_slist_free_all(headers);
		headers = nullptr;
		headers = curl_slist_append(headers, ("Authorization: Bot " + token).c_str());
		headers = curl_slist_append(headers, "Content-Type: application/json");

		// Nastavení URL pro odeslání zprávy
		curl_easy_reset(curl);
		std::string url = "https://discord.com/api/v10/channels/" + dmChannelId + "/messages";
		std::string response;
		nlohmann::json body;
		body["content"] = message;
		std::string bodyStr = body.dump();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bodyStr.size());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			logger.LogError(L"CURL error pøi odesílání zprávy: " + StringToWString(curl_easy_strerror(res)));
			return false;
		}

		// Ovìøení odpovìdi
		if (response.empty())
		{
			logger.LogError(L"Prázdná odpovìï ze serveru pøi odesílání zprávy.");
			return false;
		}

		logger.LogInfo(L"Zpráva úspìšnì odeslána.");

		// Uvolnìní hlavièek
		curl_slist_free_all(headers);
		headers = nullptr;

		return true;
	}
	catch (const std::exception& e)
	{
		logger.LogError(L"Neoèekávaná výjimka: " + StringToWString(e.what()));
		return false;
	}
}

std::string DiscordManager::StringToUTF8(const std::string& str)
{
	return WStringToString(StringToWString(str));
}

std::wstring DiscordManager::StringToWString(const std::string& s)
{
	int size_needed = MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), &wstr[0], size_needed);
	return wstr;
}

std::string DiscordManager::WStringToString(const std::wstring& ws)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), &str[0], size_needed, nullptr, nullptr);
	return str;
}

size_t DiscordManager::writeCallback(void* contents, size_t size, size_t nmemb, void* userData)
{
	((std::string*)userData)->append((char*)contents, size * nmemb);
	return size * nmemb;
}