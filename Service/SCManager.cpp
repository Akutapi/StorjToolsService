/*
  Soubor: SCManager.cpp
  Popis: Tøída pro správu služeb Windows Service
  Autor: Akutapi
  Datum: 16-12-2024
 */
#include "SCManager.h"
#include <iostream>


SCManager::SCManager(Logger& _logger) : logger(_logger)
{
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		logger.LogError(L"Error opening service manager: ");
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
	// kontrola, zda služba již není spuštìna nebo se nespouští
	if (GetServiceStatus(serviceName) == SERVICE_RUNNING || GetServiceStatus(serviceName) == SERVICE_START_PENDING)
	{
		logger.LogWarning(L"Service is already running or starting " + serviceName);
		return ;
	}

    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_START);
    if (hService)
    {
        if (!::StartService(hService, 0, NULL))
        {
            logger.LogError(L"Error starting service: ");
        }
        CloseServiceHandle(hService);
    }
    else
    {
        logger.LogError(L"Failed to open service: ");
    }
}

bool SCManager::CustomStopService(const std::wstring& serviceName)
{
	// kontrola, zda služba již není zastavena nebo se zastavuje
	if (GetServiceStatus(serviceName) == SERVICE_STOPPED || GetServiceStatus(serviceName) == SERVICE_STOP_PENDING)
	{
		logger.LogWarning(L"Service is already stopped or stopping " + serviceName);
		return true;
	}

    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_STOP);
    if (!hService)
    {
        logger.LogError(L"Failed to open service: ");
        return false;
    }
    SERVICE_STATUS serviceStatus;
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus))
    {
        logger.LogError(L"Error stopping service: ");
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
        logger.LogError(L"Failed to stop service: ");
        return false;
    }

	SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
	if (!hService)
	{
		logger.LogError(L"Failed to open service: ");
		return false;
	}

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    ULONGLONG dwStartTime = GetTickCount64();

    while (true)
    {
        if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
        {
            logger.LogError(L"Error querying service status: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            CloseServiceHandle(hService);
            return true;
        }

        if (GetTickCount64() - dwStartTime > TIMEOUT_SECONDS * 1000)
        {
            logger.LogError(L"Timeout waiting for service to stop: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }
        Sleep(250); // Poèkáme 250 ms pøed dalším dotazem
    }
    
}

bool SCManager::CustomPauseService(const std::wstring& serviceName)
{
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE);
    if (!hService)
    {
        logger.LogError(L"Failed to open service: ");
        return false;
    }
    SERVICE_STATUS serviceStatus;
    if (!ControlService(hService, SERVICE_CONTROL_PAUSE, &serviceStatus))
    {
        logger.LogError(L"Error pausing service: ");
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
        logger.LogError(L"Failed to pause service: ");
        return false;
    }

	SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
	if (!hService)
    {
	   	logger.LogError(L"Failed to open service: ");
		return false; 
	}

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    ULONGLONG dwStartTime = GetTickCount64();

    while (true)
    {
        if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
        {
            logger.LogError(L"Error querying service status: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }

        if (ssp.dwCurrentState == SERVICE_PAUSED)
        {
            CloseServiceHandle(hService);
            return true;
        }

        if (GetTickCount64() - dwStartTime > TIMEOUT_SECONDS * 1000)
        {
            logger.LogError(L"Timeout waiting for service to pause: " + serviceName);
            CloseServiceHandle(hService);
            return false;
        }
        Sleep(250); // Poèkáme 250 ms pøed dalším dotazem
    }
}
 
void SCManager::CustomContinueService(const std::wstring & serviceName)
{
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE);
    if (hService) {
        SERVICE_STATUS serviceStatus;
        if (!ControlService(hService, SERVICE_CONTROL_CONTINUE, &serviceStatus))
        {
            logger.LogError(L"Error continuing service: ");
        }
        CloseServiceHandle(hService);
    }
    else
    {
        logger.LogError(L"Failed to open service: ");
    }
}

DWORD SCManager::GetServiceStatus(const std::wstring& serviceName)
{
    SERVICE_STATUS_PROCESS serviceStatus;
    DWORD bytesNeeded;
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
    if (!hService)
    {
        logger.LogError(L"Failed to open service: ");
        return 0;
    }
    if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&serviceStatus, sizeof(serviceStatus), &bytesNeeded))
    {
        CloseServiceHandle(hService);
        logger.LogError(L"Error getting service status: ");
		return 0;
    }
    CloseServiceHandle(hService);
	return serviceStatus.dwCurrentState;
}

// Funkce na získání seznamu služeb podle názvu
std::vector<std::wstring> SCManager::GetServicesByName(const std::wstring& serviceName)
{
    std::vector<std::wstring> services;
    DWORD bytesNeeded = 0;
    DWORD serviceCount = 0;
    DWORD resumeHandle = 0;
    LPENUM_SERVICE_STATUS lpServices = nullptr;

    // Získání poètu služeb
    if (!EnumServicesStatus(hSCManager, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, nullptr, 0, &bytesNeeded, &serviceCount, &resumeHandle))
    {
        if (GetLastError() != ERROR_MORE_DATA)
        {
            logger.LogError(L"Error enumerating services: ");
            return services;
        }
    }

    // Alokace pamìti pro služby
    lpServices = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, bytesNeeded);
    if (lpServices == nullptr)
    {
        logger.LogError(L"Error allocating memory: ");
        return services;
    }

    // Získání seznamu služeb
    if (!EnumServicesStatus(hSCManager, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, lpServices, bytesNeeded, &bytesNeeded, &serviceCount, &resumeHandle))
    {
        logger.LogError(L"Error enumerating services: ");
        LocalFree(lpServices);
        return services;
    }

    // Kontrola názvù služeb
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

// Funkce na získání cesty k souboru služby z registru
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

	// Odstranìní uvozovek a pøípadných parametrù
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