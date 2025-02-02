#include "Logger.h"
#include <iostream>
#include <windows.h>
#include "ConfigFileManager.h"
#include "DiscordManager.h"

int main() {

	//�e�tina pro v�stup do konzole
	std::locale::global(std::locale("cs_CZ.UTF-8"));
	std::wcout << "-----------------Test-------------" << std::endl;
	//inicializace loggeru
	Logger logger(const_cast<LPWSTR>(L"Test"));
	ConfigFileManager config(logger);
	DiscordManager discordManager(logger);
	//nastaven� DiscordManager
	config.UpdateConfig();
	std::string token = config.GetDiscordBotToken();
	std::string userID = config.GetDiscordUserID();
	std::cout << "Token: " << token << std::endl;
	std::cout << "UserID: " << userID << std::endl;
	discordManager.SetDiscordBot(config.GetDiscordBotToken(), config.GetDiscordUserID());
	//zpr�va na Discord
	discordManager.sendDM(L"Test message");

	std::wcout << "Tests Done" << std::endl;
	std::cin.get();
    return 0;
}
