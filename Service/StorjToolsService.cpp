/*
* 
* Stroj Tools Service je Windows sluûba navrûen· pro ˙drûbovÈ ˙koly na serverech s decentralizovan˝m Stroj ˙loûiötÏm.
* Podporuje vÌce uzl˘ na jednom serveru, coû zajiöùuje efektivnÌ spr·vu a provoz.
* MIT License:
* Tento projekt je licencov·n pod MIT licencÌ
* 
* Stroj Tools Service is a Windows service designed for maintenance tasks on servers with decentralized Stroj storage.
* It supports multiple nodes on a single server, ensuring efficient management and operation.
* MIT License:
* This project is licensed under the MIT License
* 
*/
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <format>
#include "Logger.h"
#include "Tools.h"

// PouûitÈ knihovny
// Used libraries
#pragma comment(lib, "advapi32.lib")

// N·zev sluûby
// Service name
#define SVCNAME const_cast<LPWSTR>(L"StroJToolsService")
Logger logger(SVCNAME);
Tools tools(logger);
std::vector< Tools::TimerInfo> timers;
SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR*);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR*);
VOID StartTimer(Tools::TimerInfo& timer);
VOID CheckAndModifyTimer(Tools::TimerInfo& timer);
VOID InitTimers();
VOID CloseTimers();

// VstupnÌ bod pro sluûbu
// Entry point for the service
int __cdecl _tmain(int argc, TCHAR* argv[])
{
	// ServisnÌ tabulka
	// Service table
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
        { NULL, NULL }
    };

	// SpuötÏnÌ sluûby
	// Start the service
    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        logger.LogError(L"StartServiceCtrlDispatcher");
    }
    return 0;
}

// VstupnÌ bod pro sluûbu
// Entry point for the service
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Zaregistrujte funkci obsluhy pro sluûbu
    // Register the handler function for the service

    gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);

    if (!gSvcStatusHandle)
    {
        logger.LogError(L"RegisterServiceCtrlHandler");
        return;
    }

	// Initialize service status
    // These SERVICE_STATUS members remain as set here
    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    // Reportov·nÌ poË·teËnÌho stavu SCM
    // Report initial status to the SCM
    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // Provede inicializaci a pr·ci specifickou pro sluûbu.
    // Perform service-specific initialization and work.
    SvcInit(dwArgc, lpszArgv);
}

// Inicializace sluûby
// Service initialization
VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Vytvo¯Ì ud·lost. Funkce obsluhy ¯ÌzenÌ, SvcCtrlHandler,
	// Create an event. The control handler function, SvcCtrlHandler,
    ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);   

    if (ghSvcStopEvent == NULL)
    {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // Nahl·sit bÏûÌcÌ stav, kdyû je inicializace dokonËena.
    // Report running status when initialization is complete.
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    try
    {
        // prvnÌ aktivace ËasovaË˘
        // first activation of timers
        InitTimers();
    }
    catch (const std::exception& e)
    {
        logger.LogTry(L"Exception during timer initialization: ", e.what());
    }
    catch (...)
    {
        logger.LogError(L"Unknown exception during timer initialization.");
    }

	
    // Seznam objekt˘ pro `WaitForMultipleObjects`
    // List of objects for `WaitForMultipleObjects`
    std::vector<HANDLE> waitHandles = { ghSvcStopEvent };
    for (const Tools::TimerInfo& timer : timers)
    {
		if (timer.hTimer)
        {
            waitHandles.push_back(timer.hTimer);
        }
    }

    while (true)
    {
        try
        {
            // Zkontroluje, zda byla zasl·na zpr·va 
            // Check if a message was sent
            DWORD dwWait = WaitForMultipleObjects(static_cast<DWORD>(waitHandles.size()), waitHandles.data(), FALSE, INFINITE);
            // Zkontroluje, kter˝ objekt byl signalizov·n
            // Check which object was signaled
            if (dwWait == WAIT_OBJECT_0) // ghSvcStopEvent - poûadavek na zastavenÌ sluûby, request to stop the service
            {
                ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
                return;
            }
            else if (dwWait > WAIT_OBJECT_0 && dwWait < WAIT_OBJECT_0 + waitHandles.size()) // nÏkter˝ z timer˘, one of the timers
            {
                int timerIndex = dwWait - WAIT_OBJECT_0 - 1; // index timeru
                Tools::TimerInfo& curentTimer = timers[timerIndex];
                // kontrola zmÏny intervalu, check interval change
                CheckAndModifyTimer(curentTimer);
              
                if (curentTimer.isRunning)
                {
                    curentTimer.callback(); // vol·nÌ callback funkce pro dan˝ timer, call the callback function for the given timer
                }
            }
            else
            {
                logger.LogError(L"WaitForMultipleObjects");
            }
        }
        catch (const std::exception& e)
        {
            logger.LogTry(L"Exception in Service processing loop: ", e.what());
        }
        catch (...)
        {
            logger.LogError(L"Unknown exception in Service processing loop.");
        }
    }
}
// Report stavu sluûby SCM.
// Report the status of the service to the SCM.
VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // VyplnÌ strukturu SERVICE_STATUS.
    // Fill in the SERVICE_STATUS structure.
    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
    {
        gSvcStatus.dwControlsAccepted = 0;
    }
    else
    {
        gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
    {
        gSvcStatus.dwCheckPoint = 0;
    }
    else
    {
        gSvcStatus.dwCheckPoint = dwCheckPoint++;
    }

    // Nahl·sÌ stav sluûby SCM.
    // Report the status of the service to the SCM.
    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

// Funkce obsluhy ¯ÌzenÌ sluûby
// Service control handler function
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    // Zpracujte poûadovan˝ ¯ÌdicÌ kÛd.
    // Handle the requested control code. 
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        // Signalizujte sluûbÏ, aby se zastavila, Signal the service to stop.
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		CloseTimers();
        SetEvent(ghSvcStopEvent);
        ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
        return;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    default:
        break;
    }

}

VOID StartTimer(Tools::TimerInfo& timer)
{
    LARGE_INTEGER liDueTime;
    timer.hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (timer.hTimer == NULL)
    {
        logger.LogError(L"CreateWaitableTimer failed.");
    }
    liDueTime.QuadPart = -static_cast<LONGLONG>(2) * 10000000; // NastavenÌ na 2 sec , Set to 2 sec
    int interval = 60 * 60 * 1000; //interval v hodin·ch
    //int interval = 1000; // interval v sekund·ch pro testov·nÌ, interval in seconds for testing
    if (timer.interval == 0)
    {
        timer.isRunning = false;
    }
    else
    {
        timer.isRunning = true;
        interval *= timer.interval;
    }

    if (timer.hTimer)
    {
		// NastavenÌ ËasovaËe
		// Set the timer
        if (!SetWaitableTimer(timer.hTimer, &liDueTime,interval, NULL, NULL, FALSE))
        {
            logger.LogError(L"Error setting waitable timer.");
        }
    }

}


VOID CheckAndModifyTimer(Tools::TimerInfo& timer)
{
	// OvÏ¯enÌ, zda je timer platn˝
    // Verify if the timer is valid
    if(!timer.hTimer)
	{
		logger.LogError(L"Modify Timer:Timer handle is NULL.");
		return;
	}

	// OvÏ¯enÌ, zda byl interval zmÏnÏn
    // Verify if the interval has changed
	ConfigFileManager& config = tools.UpdateConfigFromFile();
	int newInterval = timer.GetNewInterval(config);
    if (timer.interval == newInterval)
    {
        return;
    }

	// ZmÏna intervalu
    // Change the interval
	timer.interval = newInterval;

    LARGE_INTEGER liDueTime;
    //liDueTime.QuadPart = -static_cast<LONGLONG>(timer.interval) * 60 * 60 * 1000 * 10000; // p¯evod hodin na 100-nanosekundovÈ intervaly, convert hours to 100-nanosecond intervals
    liDueTime.QuadPart = -static_cast<LONGLONG>(timer.interval) * 10000000; // doËasnÏ v sekund·ch mÌsto hodin, temporarily in seconds instead of hours

    int interval = 60 * 60 * 1000; //interval v hodin·ch
    //int interval = 1000; // interval s sekund·ch pro testov·nÌ , interval in seconds for testing
    if (timer.interval == 0)
	{
		timer.isRunning = false;
	}
    else
    {
		timer.isRunning = true;
		interval *= timer.interval;
    }
        
    // NastavenÌ ËasovaËe
	// Set the timer
    if (!SetWaitableTimer(timer.hTimer, &liDueTime, interval, NULL, NULL, FALSE))
    {
        logger.LogError(L"Error setting waitable timer.");
    }
}

VOID InitTimers()
{   
	// vytvo¯enÌ ËasovaË˘
	// create timers
    timers = tools.GetTimers();
    if (timers.empty())
    {
        return;
    }

	for (Tools::TimerInfo& timer : timers)
	{
		StartTimer(timer);
	}
}

VOID CloseTimers()
{
	// zruöenÌ ËasovaË˘
    // close timers
	if (timers.empty())
	{
		return;
	}
    for (Tools::TimerInfo& timer : timers)
	{
        if (timer.hTimer)
        {
            CancelWaitableTimer(timer.hTimer);
			CloseHandle(timer.hTimer);
			timer.hTimer = NULL;
        }
	}
}
