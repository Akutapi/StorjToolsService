#include "Logger.h"
#include <iostream>
#include <windows.h>
#include "Tools.h"

int main() {

	//èeština pro výstup do konzole
	std::locale::global(std::locale("cs_CZ.UTF-8"));
	std::wcout << "-----------------Test-------------" << std::endl;
	//inicializace loggeru
	Logger logger(const_cast<LPWSTR>(L"Test"));
	Tools tools(logger);
	tools.CheckStorjNodeUpdate();
	std::wcout << "Tests Done" << std::endl;
	std::cin.get();
    return 0;
}
