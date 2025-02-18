#include "Logger.h"
#include <iostream>
#include <windows.h>
#include "Tools.h"

int main() {

	//�e�tina pro v�stup do konzole
	std::locale::global(std::locale("cs_CZ.UTF-8"));
	std::wcout << "-----------------Test-------------" << std::endl;
	//inicializace loggeru
	Logger logger(L"Debug");
	logger.LogInfo(L"Testovac� zpr�va");
	std::wcout << "Tests Done" << std::endl;
	std::cin.get();
    return 0;
}
