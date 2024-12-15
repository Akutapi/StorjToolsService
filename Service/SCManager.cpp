#include "SCManager.h"
#include <iostream>


SCManager::SCManager(Logger& _logger) : logger(_logger)
{
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		logger.LogError(L"Chyba p�i otev�r�n� spr�vce slu�eb: ");
	}
}

SCManager::~SCManager()
{
    if (hSCManager)
    {
        CloseServiceHandle(hSCManager);
    }
}

void SCManager::CustomStartService(const std::wstring& serviceName)
{
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_START);
    if (hService)
    {
        if (!::StartService(hService, 0, NULL))
        {
            logger.LogError(L"Chyba p�i spu�t�n� slu�by: ");
        }
        CloseServiceHandle(hService);
    }
    else
    {
        logger.LogError(L"Slu�bu se nepoda�ilo otev��t: ");
    }
}

bool SCManager::CustomStopService(const std::wstring& serviceName)
{
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_STOP);
    if (!hService)
    {
        logger.LogError(L"Slu�bu se nepoda�ilo otev��t: ");
        return false;
    }
    SERVICE_STATUS serviceStatus;
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus))
    {
        logger.LogError(L"Chyba p�i zastavov�n� slu�by: ");
        CloseServiceHandle(hService);
		return false;
    }
	CloseServiceHandle(hService);
	return true;
}

bool SCManager::CustomStopServiceWithWait(const std::wstring& serviceName)
{
	if(!CustomStopService(serviceName))
    {
        logger.LogError(L"Slu�bu se nepoda�ilo zastavit: ");
        return false;
    }

	SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
	if (!hService)
	{
		logger.LogError(L"Slu�bu se nepoda�ilo otev��t: ");
		return false;
	}

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    ULONGLONG dwStartTime = GetTickCount64();
	int pocetpokusu = 0;

    while (true)
    {
        if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
        {
            logger.LogError(L"Chyba p�i dotazov�n� na stav slu�by: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            logger.LogInfo(L"Slu�ba byla �sp�n� zastavena: " + serviceName);
            CloseServiceHandle(hService);
            return true;
        }

        if (GetTickCount64() - dwStartTime > TIMEOUT_SECONDS * 1000)
        {
            logger.LogError(L"Timeout p�i �ek�n� na zastaven� slu�by: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }

		++pocetpokusu;
		logger.LogInfo(L"�ek�n� na zastaven� slu�by: Tick" + pocetpokusu);
        Sleep(250); // Po�k�me 250 ms p�ed dal��m dotazem
    }
    
}

bool SCManager::CustomPauseService(const std::wstring& serviceName)
{
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE);
    if (!hService)
    {
        logger.LogError(L"Slu�bu se nepoda�ilo otev��t: ");
        return false;
    }
    SERVICE_STATUS serviceStatus;
    if (!ControlService(hService, SERVICE_CONTROL_PAUSE, &serviceStatus))
    {
        logger.LogError(L"Chyba p�i pozastavov�n� slu�by: ");
		CloseServiceHandle(hService);
		return false;
    }

    CloseServiceHandle(hService);
    return true;
}

bool SCManager::CustomPauseServiceWithWait(const std::wstring& serviceName)
{
	if (!CustomPauseService(serviceName))
    {
        logger.LogError(L"Slu�bu se nepoda�ilo pozastavit: ");
        return false;
    }

	SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
	if (!hService)
    {
	   	logger.LogError(L"Slu�bu se nepoda�ilo otev��t: ");
		return false; 
	}

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    ULONGLONG dwStartTime = GetTickCount64();
    int pocetpokusu = 0;

    while (true)
    {
        if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
        {
            logger.LogError(L"Chyba p�i dotazov�n� na stav slu�by: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }

        if (ssp.dwCurrentState == SERVICE_PAUSED)
        {
            logger.LogInfo(L"Slu�ba byla �sp�n� zastavena: " + serviceName);
            CloseServiceHandle(hService);
            return true;
        }

        if (GetTickCount64() - dwStartTime > TIMEOUT_SECONDS * 1000)
        {
            logger.LogError(L"Timeout p�i �ek�n� na zastaven� slu�by: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }

        ++pocetpokusu;
        logger.LogInfo(L"�ek�n� na zastaven� slu�by: Tick" + pocetpokusu);
        Sleep(250); // Po�k�me 250 ms p�ed dal��m dotazem
    }
}
 
void SCManager::CustomContinueService(const std::wstring & serviceName)
{
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE);
    if (hService) {
        SERVICE_STATUS serviceStatus;
        if (!ControlService(hService, SERVICE_CONTROL_CONTINUE, &serviceStatus))
        {
            logger.LogError(L"Chyba p�i pokra�ov�n� slu�by: ");
        }
        CloseServiceHandle(hService);
    }
    else
    {
        logger.LogError(L"Slu�bu se nepoda�ilo otev��t: ");
    }
}

DWORD SCManager::GetServiceStatus(const std::wstring& serviceName)
{
    SERVICE_STATUS_PROCESS serviceStatus;
    DWORD bytesNeeded;
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
    if (!hService)
    {
        logger.LogError(L"Slu�bu se nepoda�ilo otev��t: ");
        return 0;
    }
    if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&serviceStatus, sizeof(serviceStatus), &bytesNeeded))
    {
        CloseServiceHandle(hService);
        logger.LogError(L"Chyba p�i z�sk�v�n� stavu slu�by: ");
		return 0;
    }
    CloseServiceHandle(hService);
	return serviceStatus.dwCurrentState;
}

// Funkce na z�sk�n� seznamu slu�eb podle n�zvu
std::vector<std::wstring> SCManager::GetServicesByName(const std::wstring& serviceName)
{
    std::vector<std::wstring> services;
    DWORD bytesNeeded = 0;
    DWORD serviceCount = 0;
    DWORD resumeHandle = 0;
    LPENUM_SERVICE_STATUS lpServices = nullptr;

    // Z�sk�n� po�tu slu�eb
    if (!EnumServicesStatus(hSCManager, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, nullptr, 0, &bytesNeeded, &serviceCount, &resumeHandle))
    {
        if (GetLastError() != ERROR_MORE_DATA)
        {
            logger.LogError(L"Chyba p�i enumeraci slu�eb: ");
            return services;
        }
    }

    // Alokace pam�ti pro slu�by
    lpServices = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, bytesNeeded);
    if (lpServices == nullptr)
    {
        logger.LogError(L"Chyba p�i alokaci pam�ti: ");
        return services;
    }

    // Z�sk�n� seznamu slu�eb
    if (!EnumServicesStatus(hSCManager, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, lpServices, bytesNeeded, &bytesNeeded, &serviceCount, &resumeHandle))
    {
        logger.LogError(L"Chyba p�i enumeraci slu�eb: ");
        LocalFree(lpServices);
        return services;
    }

    // Kontrola n�zv� slu�eb
    for (DWORD i = 0; i < serviceCount; ++i)
    {
        if (std::wstring(lpServices[i].lpServiceName).find(serviceName) != std::wstring::npos)
        {
            services.push_back(lpServices[i].lpServiceName);
        }
    }

    LocalFree(lpServices);
    return services;
}

// Funkce na z�sk�n� cesty k souboru slu�by z registru
std::filesystem::path SCManager::GetServicePathByName(const std::wstring& serviceName)
{
    HKEY hKey;
    std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        logger.LogError(L"Failed to open registry key for service: ");
		return L"";
    }
    wchar_t imagePath[512];
    DWORD bufSize = sizeof(imagePath);
    if (RegQueryValueExW(hKey, L"ImagePath", nullptr, nullptr, (LPBYTE)imagePath, &bufSize) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        logger.LogError(L"Failed to query ImagePath for service: ");
		return L"";
	}
    RegCloseKey(hKey);
    std::wstring pathStr(imagePath);
    if (pathStr.empty())
    {
        logger.LogError(L"Failed to get ImagePath for service: ");
        return L"";
    }

	// Odstran�n� uvozovek a p��padn�ch parametr�
    if (pathStr.front() == L'\"')
    { 
        pathStr = pathStr.substr(1);
        size_t pos = pathStr.find(L'\"');
        if (pos != std::wstring::npos)
        {
            pathStr = pathStr.substr(0, pos);
        }
    }

    return std::filesystem::path(pathStr);
}