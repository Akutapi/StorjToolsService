#include "SCManager.h"
#include "SCManager.h"
#include <iostream>


SCManager::SCManager(Logger& logger) : m_logger(logger)
{
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!m_hSCManager)
	{
		m_logger.LogError(L"Chyba pøi otevírání správce služeb: ");
	}
}

SCManager::~SCManager()
{
    if (m_hSCManager)
    {
        CloseServiceHandle(m_hSCManager);
    }
}

void SCManager::CustomStartService(const std::wstring& serviceName)
{
    SC_HANDLE hService = OpenService(m_hSCManager, serviceName.c_str(), SERVICE_START);
    if (hService)
    {
        if (!::StartService(hService, 0, NULL))
        {
            m_logger.LogError(L"Chyba pøi spuštìní služby: ");
        }
        CloseServiceHandle(hService);
    }
    else
    {
        m_logger.LogError(L"Službu se nepodaøilo otevøít: ");
    }
}

void SCManager::CustomStopService(const std::wstring& serviceName)
{
    SC_HANDLE hService = OpenService(m_hSCManager, serviceName.c_str(), SERVICE_STOP);
    if (hService)
    {
        SERVICE_STATUS serviceStatus;
        if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus))
        {
            m_logger.LogError(L"Chyba pøi zastavování služby: ");
        }
        CloseServiceHandle(hService);
    }
    else
    {
        m_logger.LogError(L"Službu se nepodaøilo otevøít: ");
    }
}

bool SCManager::CustomPauseService(const std::wstring& serviceName)
{
    SC_HANDLE hService = OpenService(m_hSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE);
    if (!hService)
    {
        m_logger.LogError(L"Službu se nepodaøilo otevøít: ");
        return false;
    }
    SERVICE_STATUS serviceStatus;
    if (!ControlService(hService, SERVICE_CONTROL_PAUSE, &serviceStatus))
    {
        m_logger.LogError(L"Chyba pøi pozastavování služby: ");
		CloseServiceHandle(hService);
		return false;
    }

    CloseServiceHandle(hService);
    return true;
}

void SCManager::CustomPauseServiceWithWait(const std::wstring& serviceName)
{
	if (!CustomPauseService(serviceName))
    {
        m_logger.LogError(L"Službu se nepodaøilo pozastavit: ");
        return;
    }
	SERVICE_STATUS_PROCESS serviceStatus;
    DWORD bytesNeeded;
    DWORD statusSize;
	SC_HANDLE hService = OpenService(m_hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
	if (!hService)
    {
	   	m_logger.LogError(L"Službu se nepodaøilo otevøít: ");
		return; 
	}

    statusSize = sizeof(serviceStatus);

	while (true)
	{
		if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&serviceStatus, statusSize, &bytesNeeded))
		{
			m_logger.LogError(L"Chyba pøi získávání stavu služby: ");
			break;
		}
		if (serviceStatus.dwCurrentState == SERVICE_PAUSED)
		{
			break;
		}
		Sleep(1000);
	}
    CloseServiceHandle(hService);
}
 
void SCManager::CustomContinueService(const std::wstring & serviceName)
{
    SC_HANDLE hService = OpenService(m_hSCManager, serviceName.c_str(), SERVICE_PAUSE_CONTINUE);
    if (hService) {
        SERVICE_STATUS serviceStatus;
        if (!ControlService(hService, SERVICE_CONTROL_CONTINUE, &serviceStatus))
        {
            m_logger.LogError(L"Chyba pøi pokraèování služby: ");
        }
        CloseServiceHandle(hService);
    }
    else
    {
        m_logger.LogError(L"Službu se nepodaøilo otevøít: ");
    }
}

// Funkce na získání seznamu služeb podle názvu
std::vector<std::wstring> SCManager::GetServicesByName(const std::wstring& serviceName)
{
    std::vector<std::wstring> services;
    DWORD bytesNeeded;
    DWORD serviceCount;
    DWORD resumeHandle = 0;
    LPENUM_SERVICE_STATUS lpServices = nullptr;

    // Získání poètu služeb
    if (!EnumServicesStatus(m_hSCManager, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, nullptr, 0, &bytesNeeded, &serviceCount, &resumeHandle))
    {
        if (GetLastError() != ERROR_MORE_DATA)
        {
            m_logger.LogError(L"Chyba pøi enumeraci služeb: ");
            return services;
        }
    }

    // Alokace pamìti pro služby
    lpServices = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, bytesNeeded);
    if (lpServices == nullptr)
    {
        m_logger.LogError(L"Chyba pøi alokaci pamìti: ");
        return services;
    }

    // Získání seznamu služeb
    if (!EnumServicesStatus(m_hSCManager, SERVICE_TYPE_ALL, SERVICE_STATE_ALL, lpServices, bytesNeeded, &bytesNeeded, &serviceCount, &resumeHandle))
    {
        m_logger.LogError(L"Chyba pøi enumeraci služeb: ");
        LocalFree(lpServices);
        return services;
    }

    // Kontrola názvù služeb
    for (DWORD i = 0; i < serviceCount; ++i)
    {
		std::wstring curentserviceName = lpServices[i].lpServiceName;
        if (curentserviceName.find(serviceName) != std::wstring::npos)
        {
            services.push_back(lpServices[i].lpServiceName);
        }
    }

    return services;
}

// Funkce na získání cesty k souboru služby z registru
std::filesystem::path SCManager::GetServicePathByName(const std::wstring& serviceName)
{
    HKEY hKey;
    std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        wchar_t imagePath[512];
        DWORD bufSize = sizeof(imagePath);
        if (RegQueryValueExW(hKey, L"ImagePath", nullptr, nullptr, (LPBYTE)imagePath, &bufSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            std::wstring pathStr(imagePath);
            // Odstranìní uvozovek, pokud existují
            if (!pathStr.empty() && pathStr.front() == L'\"' && pathStr.back() == L'\"')
            {
                pathStr = pathStr.substr(1, pathStr.size() - 2);
            }
            return std::filesystem::path(pathStr);
        }
        else
        {
            RegCloseKey(hKey);
            m_logger.LogError(L"Failed to query ImagePath for service: ");
        }
    }
    else
    {
        m_logger.LogError(L"Failed to open registry key for service: ");
    }
    return L"";
}