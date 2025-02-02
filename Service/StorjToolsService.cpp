/*
* 
* Stroj Tools Service je Windows slu�ba navr�en� pro �dr�bov� �koly na serverech s decentralizovan�m Stroj �lo�i�t�m.
* Podporuje v�ce uzl� na jednom serveru, co� zaji��uje efektivn� spr�vu a provoz.
* MIT License:
* Tento projekt je licencov�n pod MIT licenc�
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

// Pou�it� knihovny
// Used libraries
#pragma comment(lib, "advapi32.lib")

// N�zev slu�by
// Service name
#define SVCNAME const_cast<LPWSTR>(L"StroJToolsService")
// �asov� hodnoty
// Time values
#define SECOND_IN_100NS 10000000LL  // 1 sekunda v 100 ns, 1 second in 100 ns
#define OFFSET_MULTIPLIER 5         // Offset v minut�ch, Offset in minutes
#define MINUTE_IN_100NS (60 * SECOND_IN_100NS)  // 1 minuta v 100 ns, 1 minute in 100 ns
#define HOUR_IN_MILLISECONDS (60 * 60 * 1000) // 1 hodina v milisekund�ch, 1 hour in milliseconds
#define HOUR_IN_100NS (60 * 60 * SECOND_IN_100NS) // 1 hodina v 100 ns, 1 hour in 100 ns

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
VOID StartTimer(Tools::TimerInfo& timer, int timeOffset);
VOID CheckAndModifyTimer(Tools::TimerInfo& timer);
VOID InitTimers();
VOID CloseTimers();

// Vstupn� bod pro slu�bu
// Entry point for the service
int __cdecl _tmain(int argc, TCHAR* argv[])
{
	// Servisn� tabulka
	// Service table
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
        { NULL, NULL }
    };

	// Spu�t�n� slu�by
	// Start the service
    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        logger.LogError(L"StartServiceCtrlDispatcher");
    }
    return 0;
}

// Vstupn� bod pro slu�bu
// Entry point for the service
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Zaregistrujte funkci obsluhy pro slu�bu
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

    // Reportov�n� po��te�n�ho stavu SCM
    // Report initial status to the SCM
    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // Provede inicializaci a pr�ci specifickou pro slu�bu.
    // Perform service-specific initialization and work.
    SvcInit(dwArgc, lpszArgv);
}

// Inicializace slu�by
// Service initialization
VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Vytvo�� ud�lost. Funkce obsluhy ��zen�, SvcCtrlHandler,
	// Create an event. The control handler function, SvcCtrlHandler,
    ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);   

    if (ghSvcStopEvent == NULL)
    {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // Nahl�sit b��c� stav, kdy� je inicializace dokon�ena.
    // Report running status when initialization is complete.
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    try
    {
        // prvn� aktivace �asova��
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

	
    // Seznam objekt� pro `WaitForMultipleObjects`
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
            // Zkontroluje, zda byla zasl�na zpr�va 
            // Check if a message was sent
            DWORD dwWait = WaitForMultipleObjects(static_cast<DWORD>(waitHandles.size()), waitHandles.data(), FALSE, INFINITE);
            // Zkontroluje, kter� objekt byl signalizov�n
            // Check which object was signaled
            if (dwWait == WAIT_OBJECT_0) // ghSvcStopEvent - po�adavek na zastaven� slu�by, request to stop the service
            {
                ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
                return;
            }
            else if (dwWait > WAIT_OBJECT_0 && dwWait < WAIT_OBJECT_0 + waitHandles.size()) // n�kter� z timer�, one of the timers
            {
                int timerIndex = dwWait - WAIT_OBJECT_0 - 1; // index timeru
                Tools::TimerInfo& curentTimer = timers[timerIndex];
                // kontrola zm�ny intervalu, check interval change
                CheckAndModifyTimer(curentTimer);
              
                if (curentTimer.isRunning)
                {
                    curentTimer.callback(); // vol�n� callback funkce pro dan� timer, call the callback function for the given timer
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
// Report stavu slu�by SCM.
// Report the status of the service to the SCM.
VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Vypln� strukturu SERVICE_STATUS.
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

    // Nahl�s� stav slu�by SCM.
    // Report the status of the service to the SCM.
    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

// Funkce obsluhy ��zen� slu�by
// Service control handler function
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    // Zpracujte po�adovan� ��dic� k�d.
    // Handle the requested control code. 
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        // Signalizujte slu�b�, aby se zastavila, Signal the service to stop.
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

VOID StartTimer(Tools::TimerInfo& timer, int timeOffset)
{
    LARGE_INTEGER liDueTime;
    timer.hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (timer.hTimer == NULL)
    {
        logger.LogError(L"CreateWaitableTimer failed.");
		return;
    }

	// Posunut� �asu pro jednotliv� �asova�e, aby se sn�ila re�ije p�i spou�t�n� slu�by
	// Offset time for individual timers to reduce overhead when starting the service
	// Prvn� spu�t�n� �asova�e 2 sec + offset * 5min
   	// First timer run 2 sec + offset * 5min
	liDueTime.QuadPart = -(2 * SECOND_IN_100NS + static_cast<LONGLONG>(timeOffset) * OFFSET_MULTIPLIER * MINUTE_IN_100NS);
    LONG interval = 0;
    // Nastaven� �asova�e a b�hu slu�by
    // Set the timer and run the service
    if (timer.interval == 0)
    {
        timer.isRunning = false;
    }
    else
    {
        timer.isRunning = true;
        interval = static_cast<LONG>(std::lround(timer.interval * HOUR_IN_MILLISECONDS));
    }

	// Nastaven� �asova�e
	// Set the timer
    if (!SetWaitableTimer(timer.hTimer, &liDueTime,interval, NULL, NULL, FALSE))
    {
        logger.LogError(L"Error setting waitable timer.");
    }
}


VOID CheckAndModifyTimer(Tools::TimerInfo& timer)
{
	// Ov��en�, zda je timer platn�
    // Verify if the timer is valid
    if(!timer.hTimer)
	{
		logger.LogError(L"Modify Timer:Timer handle is NULL.");
		return;
	}

	// Ov��en�, zda byl interval zm�n�n
    // Verify if the interval has changed
	ConfigFileManager& config = tools.UpdateConfigFromFile();
	float newInterval = timer.GetNewInterval(config);
    if (timer.interval == newInterval)
    {
        return;
    }

	// Zm�na intervalu
    // Change the interval
	timer.interval = newInterval;

    LARGE_INTEGER liDueTime;
	// Prvn� spu�t�n� po dob� intervalu
	// First run after the interval time
    liDueTime.QuadPart = -static_cast<LONGLONG>(std::lround(timer.interval * HOUR_IN_100NS)); // p�evod hodin na 100-nanosekundov� intervaly, convert hours to 100-nanosecond intervals
    LONG interval = 0; //interval v hodin�ch, interval in hours

	// Nastaven� �asova�e a b�hu slu�by
	// Set the timer and run the service
    if (timer.interval == 0)
	{
		timer.isRunning = false;
	}
    else
    {
		timer.isRunning = true;
        interval = static_cast<LONG>(std::lround(timer.interval * HOUR_IN_MILLISECONDS));
    }
        
    // Nastaven� �asova�e
	// Set the timer
    if (!SetWaitableTimer(timer.hTimer, &liDueTime, interval, NULL, NULL, FALSE))
    {
        logger.LogError(L"Error setting waitable timer.");
    }
}

VOID InitTimers()
{   
	// vytvo�en� �asova��
	// create timers
    timers = tools.GetTimers();
    if (timers.empty())
    {
        return;
    }

    for (size_t i = 0; i < timers.size(); ++i)
    {
        int index = static_cast<int>(i);
        StartTimer(timers[i], index);
    }
}

VOID CloseTimers()
{
	// zru�en� �asova��
    // close timers
	if (timers.empty())
	{
		return;
	}
    for (Tools::TimerInfo& timer : timers)
	{
        try
        {
            if (timer.hTimer)
            {
                if (!CancelWaitableTimer(timer.hTimer))
                {
                    logger.LogError(L"Error canceling waitable timer.");
                }

                if (!CloseHandle(timer.hTimer))
                {
                    logger.LogError(L"Error closing waitable timer.");
                }
                timer.hTimer = NULL;
            }
        }
		catch (const std::exception& e)
		{
			logger.LogTry(L"Exception during timer closing: ", e.what());
		}
		catch (...)
		{
			logger.LogError(L"Unknown exception during timer closing.");
		}
	}
}
