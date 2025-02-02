/*
Soubor: DiscordManager.cpp
Popis: T��da pro obsluhu Discordu.
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
	//nastaven� hlavi�ky
	curl_slist_free_all(headers);
	headers = nullptr;
	headers = curl_slist_append(headers, ("Authorization: Bot " + token).c_str());
	headers = curl_slist_append(headers, "Content-Type: application/json");

	// Ov��en� existence u�ivatele
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
		logger.LogError(L"Pr�zdn� odpov�� ze serveru.");
		return false;
	}

	try
	{
		nlohmann::json j = nlohmann::json::parse(response);
		if (!j.contains("id"))
		{
			logger.LogError(L"U�ivatel s ID " + StringToWString(userID) + L" neexistuje.");
			return false;
		}
	}
	catch (const nlohmann::json::parse_error& e)
	{
		logger.LogError(L"Chyba p�i parsov�n� JSON: " + StringToWString(e.what()));
		return false;
	}

	// Vytvo�en� DM kan�lu
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

	// Odesl�n� POST po�adavku pro vytvo�en� DM kan�lu
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		logger.LogError(L"CURL error p�i vytv��en� DM kan�lu: " + StringToWString(curl_easy_strerror(res)));
		return false;
	}


	if (response.empty())
	{
		logger.LogError(L"Pr�zdn� odpov�� ze serveru.");
		return false;
	}

	try
	{
		nlohmann::json dmResponse = nlohmann::json::parse(response);
		if (dmResponse.contains("id"))
		{
			dmChannelId = dmResponse["id"].get<std::string>();
			logger.LogInfo(L"DM kan�l s u�ivatelem " + StringToWString(userID) + L" byl �sp�n� vytvo�en. ID kan�lu: " + StringToWString(dmChannelId));
		}
		else
		{
			logger.LogError(L"Nepoda�ilo se z�skat ID DM kan�lu.");
			return false;
		}
	}
	catch (const nlohmann::json::parse_error& e)
	{
		logger.LogError(L"Chyba p�i parsov�n� JSON: " + StringToWString(e.what()));
		return false;
	}

	// Uvoln�n� hlavi�ek
	curl_slist_free_all(headers);
	headers = nullptr;
	if (firstSet)
	{
		firstSet = false;
		logger.LogInfo(L"Discord bot byl �sp�n� nastaven.");
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
			logger.LogError(L"DM kan�l nen� inicializov�n.");
			return false;
		}
		// Inicializace hlavi�ek pro ka�d� po�adavek
		curl_slist_free_all(headers);
		headers = nullptr;
		headers = curl_slist_append(headers, ("Authorization: Bot " + token).c_str());
		headers = curl_slist_append(headers, "Content-Type: application/json");

		// Nastaven� URL pro odesl�n� zpr�vy
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
			logger.LogError(L"CURL error p�i odes�l�n� zpr�vy: " + StringToWString(curl_easy_strerror(res)));
			return false;
		}

		// Ov��en� odpov�di
		if (response.empty())
		{
			logger.LogError(L"Pr�zdn� odpov�� ze serveru p�i odes�l�n� zpr�vy.");
			return false;
		}

		logger.LogInfo(L"Zpr�va �sp�n� odesl�na.");

		// Uvoln�n� hlavi�ek
		curl_slist_free_all(headers);
		headers = nullptr;

		return true;
	}
	catch (const std::exception& e)
	{
		logger.LogError(L"Neo�ek�van� v�jimka: " + StringToWString(e.what()));
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