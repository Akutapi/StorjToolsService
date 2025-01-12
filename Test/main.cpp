#include "Tools.h"
#include "Logger.h"
#include <iostream>
#include <windows.h>

int main() {

	//èeština pro výstup do konzole
	std::locale::global(std::locale("cs_CZ.UTF-8"));
	std::wcout << "-----------------Test-------------" << std::endl;
	//inicializace loggeru
	Logger logger(const_cast<LPWSTR>(L"Test"));
    Tools tools(logger);
	
	//implementace testù
	try
	{
		tools.CheckStorjNodeUpdate();	
	}
	catch (const std::exception& e) {
		std::wcout << L"Test Failed: " << e.what() << std::endl;
		std::cin.get();
	}
	catch (...) {
		std::wcout << L"An unknown error occurred." << std::endl;
		std::cin.get();
	}


	std::wcout << "Tests Done" << std::endl;
	std::cin.get();
    return 0;
}
