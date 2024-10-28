#include "ConfigFileManager.h"
#include "LogReducer.h"
#include "SCManager.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <filesystem>
#include <strsafe.h>
#include <format>
#include "Logger.h"


#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "advapi32.lib")

#define SVCNAME const_cast<LPWSTR>(L"StroJLogService")
Logger logger(SVCNAME);

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;
HANDLE                  hTimer = NULL;

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR*);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR*);

VOID StartTimer(HANDLE& hTimer, int hours);
VOID Maintenance();

// Vstupní bod pro službu
VOID __cdecl _tmain(int argc, TCHAR* argv[])
{

	// If the command-line parameter is "install", install the service.
    if (lstrcmpi(argv[1], TEXT("install")) == 0)
    {
        SvcInstall();
		return;
    }

    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
        { NULL, NULL }
    };

    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.

    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        logger.LogError(L"StartServiceCtrlDispatcher");
    }
}

// Instalace služby
VOID SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szUnquotedPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szUnquotedPath, MAX_PATH))
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    TCHAR szPath[MAX_PATH];
    StringCbPrintf(szPath, MAX_PATH, TEXT("\"%s\""), szUnquotedPath);

    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Create the service

    schService = CreateService(
        schSCManager,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (schService == NULL)
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }
    else printf("Service installed successfully\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

// Vstupní bod pro službu
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Register the handler function for the service

    gSvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        SvcCtrlHandler);

    if (!gSvcStatusHandle)
    {
        logger.LogError(L"RegisterServiceCtrlHandler");
        return;
    }

    // These SERVICE_STATUS members remain as set here

    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    // Report initial status to the SCM

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // Perform service-specific initialization and work.

    SvcInit(dwArgc, lpszArgv);
}

// Inicializace služby
VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
	// Create an event. The control handler function, SvcCtrlHandler,
    ghSvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    if (ghSvcStopEvent == NULL)
    {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // Report running status when initialization is complete.
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // Start Maintenance
    StartTimer(hTimer, 0);

    // Seznam objektù pro `WaitForMultipleObjects`
    HANDLE waitHandles[2] = { ghSvcStopEvent, hTimer };

    while (1)
    {
		// Zkontroluje, zda byla zaslána zpráva 
        DWORD dwWait = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
        // Zkontroluje, který objekt byl signalizován
        if (dwWait == WAIT_OBJECT_0) // ghSvcStopEvent - požadavek na zastavení služby
        {
            ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
            return;
        }
        else if (dwWait == WAIT_OBJECT_0 + 1) // hTimer - vyvolání funkce èasovaèe
        {
            Maintenance();
        }
        else
        {
            logger.LogError(L"WaitForMultipleObjects");
        }
    }
}

// Report the status of the service to the SCM.
VOID ReportSvcStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED))
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

// Service control handler function
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    // Handle the requested control code. 

    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

        if (hTimer != NULL) {
            CancelWaitableTimer(hTimer);
            CloseHandle(hTimer);
        }
        // Signal the service to stop.
        SetEvent(ghSvcStopEvent);
        ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

        return;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    default:
        break;
    }

}

// Nastavení èasovaèe
VOID StartTimer(HANDLE& hTimer, int hours)
{
    LARGE_INTEGER liDueTime;
    if (hours == 0)  // Pokud je hodin 0, nastaví se interval na 2 sec
    {
        liDueTime.QuadPart = -static_cast<LONGLONG>(2) * 10000000; // Nastavení na 2 sec
    }
    else
    {
        liDueTime.QuadPart = -static_cast<LONGLONG>(hours) * 60 * 60 * 10000000; // Pøevod hodin na 100-nanosekundové intervaly
    }


    // Vytvoøení èasovaèe
    hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (hTimer == NULL) {
        logger.LogError(L"Error creating waitable timer.");
        return;
    }

    // Nastavení èasovaèe
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, FALSE)) {
        logger.LogError(L"Error setting waitable timer.");
        CloseHandle(hTimer);
        return;
    }
}

// Údržba
VOID Maintenance()
{
	// Inicializace objektù
    SCManager scManager(logger);
 	ConfigFileManager config(logger, scManager.GetServicePathByName(L"StroJLogService"));
	LogReducer logReducer(logger);

	// Naète konfiguraèní soubor
	config.LoadConfigFile();
	// Nastaví další èas údržby
    StartTimer(hTimer, config.GetReduceLogTimeInHours());

	// Získá seznam služeb
	std::vector<std::wstring> servicesAll = scManager.GetServicesByName(L"storagenode"); 
	if (servicesAll.empty())
	{
		logger.LogError(L"StroJ Nody nenalezeny.");
		return;
	}
	// Získá seznam služeb bez StorJ Update Nodù
	std::vector<std::wstring> services;
    for (auto& service : servicesAll)
	{
		if (service.find(L"updater") == std::wstring::npos)
		{
			services.push_back(service);
		}
	}
    logger.LogInfo(std::format(L"Nalezeno StroJ Nodù: {}", services.size()));

	// kontrola existence souboru a jeho velikosti
    std::vector<std::wstring> servicesToMaintain;
    for (auto& service : services)
    {
        std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / L"storagenode.log";
        size_t currentSize = std::filesystem::file_size(logPath);
		if (std::filesystem::exists(logPath) && currentSize >= config.GetMaxLogSize())
		{
			servicesToMaintain.push_back(service);
    		continue;
		}
    }

    logger.LogInfo(std::format(L"Nalezeno StroJ Nodù k redukci log souborù: {}", servicesToMaintain.size()));
	// Pro každý StroJ Nod provede redukci log souboru do temp souboru
    for (auto& service : servicesToMaintain)
    {
        // Redukce log souboru
        std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / L"storagenode.log";
        logReducer.ReduceLogToTemp(logPath, config.GetMaxLogSize(), config.GetReduceLogSize());
    }

	// Pro každý StroJ Nod pøepíše log soubor temp souborem
	for (auto& service : servicesToMaintain)
	{
		// Pause service
		scManager.CustomPauseServiceWithWait(service);
        logger.LogInfo(std::format(L"Pausován: {}", service));
        // Redukce log souboru
		std::wstring logPath = std::filesystem::path(scManager.GetServicePathByName(service)).parent_path() / L"storagenode.log";
        logReducer.ReplaceLogWithTempFile(logPath);
		// Continue service
		scManager.CustomContinueService(service);
        logger.LogInfo(std::format(L"Spouštìn: {}", service));
	}
    logger.LogInfo(L"Maintenanc done.");
}