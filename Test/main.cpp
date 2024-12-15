#include "Tools.h"
#include "Logger.h"
#include <iostream>
#include <windows.h>

int main() {

	//èeština pro výstup do konzole
	std::locale::global(std::locale("cs_CZ.UTF-8"));
	std::wcout << "Test" << std::endl;
	//inicializace loggeru už je otestován
	Logger logger(const_cast<LPWSTR>(L"Test"));
	
	//implementace testù
    Tools tools(logger);
	try
	{
		if (tools.LogMaintenance())
		{
			std::wcout << "Test LogMaintenance OK" << std::endl;
		}
		else
		{
			std::wcout << "Test LogMaintenance Failed" << std::endl;
		}
		
	}
	catch (const std::exception&)
	{
		std::wcout << "Test LogMaintenance Failed" << std::endl;
	}
	std::wcout << "Test Done" << std::endl;
	std::cin.get();
    return 0;
}
