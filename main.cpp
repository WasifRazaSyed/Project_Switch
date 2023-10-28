#include <QCoreApplication>
#include <iostream>
#include <windows.h>
#include <strsafe.h>

#include "worker.h"

#define SVCNAME TEXT("BattEase Charger Control")
#define SVCDES TEXT("Facilitating user-defined charging thresholds, this service automates battery charger operations with precision through the integration with the Charger Control settings application.")
#define SVC_ERROR 0x00000001

SERVICE_STATUS status;
SERVICE_STATUS_HANDLE status_handle;

VOID WINAPI SVC_MAIN( DWORD, LPWSTR * );
VOID init();
VOID WINAPI Ctrl_HL( DWORD );
BOOL WINAPI Console_HL(DWORD);
VOID Event_Report(LPWSTR);
VOID Set_Actions(SC_HANDLE);
VOID Install();
VOID Delete();

worker *w;

int main(int argc, char *argv[])
{
    //compare the strings
    if(lstrcmpiA(argv[1],"install")==0){
        qDebug()<<"Installing...\r\n";
        Install();
        return 0;
    }
    else if(lstrcmpiA(argv[1], "delete")==0){
        qDebug()<<"Deleting...\r\n";
        Delete();
        return 0;
    }
    SERVICE_TABLE_ENTRYW ServiceTable[] =
        {
            { (LPWSTR)SVCNAME, (LPSERVICE_MAIN_FUNCTIONW)SVC_MAIN },
            { NULL, NULL }
        };

    //does not return if successful until service is in stopped state
    bool result=StartServiceCtrlDispatcherW(ServiceTable);
    if (result!=true)
    {
        HANDLE event_source = RegisterEventSourceW(NULL, LPWSTR(SVCNAME));
        if (event_source != NULL)
        {
            LPCWSTR lpszStrings[2];
            lpszStrings[0] = LPCWSTR(SVCNAME);
            lpszStrings[1] = TEXT("Startup failed. Reinstalling may help eradicating the issue.");

            ReportEventW(event_source, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 2, 0, lpszStrings, NULL);
            DeregisterEventSource(event_source);
        }
        return 1;
    }

    return 0;
}

VOID WINAPI SVC_MAIN( DWORD dw, LPWSTR * lp)
{
    Q_UNUSED(dw);
    Q_UNUSED(lp);

    status_handle=RegisterServiceCtrlHandlerW(SVCNAME, Ctrl_HL);
    if(!status_handle)
    {
        Event_Report(LPWSTR("RegisterServiceCtrlHandler"));
        return;
    }
    SetConsoleCtrlHandler(Console_HL, TRUE);
    status.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwCurrentState=SERVICE_RUNNING;
    status.dwWin32ExitCode=NO_ERROR;
    status.dwServiceSpecificExitCode=0;
    status.dwCheckPoint=0;

    if (SetServiceStatus(status_handle, &status) == FALSE)
    {
        Event_Report(LPWSTR("[ServiceCtrlHandler] SetServiceStatus returned error"));
    }

    init();
}

void init()
{
    int argc=0;
    char *argv={nullptr};
    QCoreApplication a(argc, &argv);
    w=new worker(&a);
    QObject::connect(&a, &QObject::destroyed, w, &QObject::deleteLater);
    a.exec();
}

VOID WINAPI Ctrl_HL(DWORD control)
{
    switch (control)
    {
    // Handle the STOP control code
    case SERVICE_CONTROL_STOP:
    {

        if (status.dwCurrentState != SERVICE_RUNNING)
        {
            break;
        }

        status.dwCurrentState = SERVICE_STOP_PENDING;
        status.dwWin32ExitCode = 0;
        status.dwCheckPoint = 0;
        status.dwWaitHint = 5000;

        if (SetServiceStatus(status_handle, &status) == FALSE)
        {
            Event_Report(LPWSTR("[ServiceCtrlHandler] SetServiceStatus returned error"));
        }

        w->shutdown();

        status.dwCurrentState = SERVICE_STOPPED;
        status.dwWin32ExitCode = 0;
        status.dwCheckPoint = 0;
        status.dwWaitHint = 0;

        if (SetServiceStatus(status_handle, &status) == FALSE)
        {
            Event_Report(LPTSTR("[ServiceCtrlHandler] SetServiceStatus returned error"));
        }
        break;
    }

    case SERVICE_CONTROL_INTERROGATE:
    {
        break;
    }

        // Handle all other control codes
    default:
    {
        break;
    }
    }
}

BOOL WINAPI Console_HL(DWORD signal)
{
    if (signal == CTRL_SHUTDOWN_EVENT || signal == CTRL_LOGOFF_EVENT)
    {
        w->reset();
    }
    return TRUE;
}

VOID Event_Report(LPWSTR lp)
{
    HANDLE event_source;
    LPCWSTR lpszStrings[2];
    wchar_t Buffer[80];

    event_source = RegisterEventSourceW(NULL, SVCNAME);

    if( NULL != event_source )
    {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), lp, GetLastError());

        lpszStrings[0] = LPCWSTR(SVCNAME);
        lpszStrings[1] = Buffer;

        ReportEventW(event_source, EVENTLOG_ERROR_TYPE,0,SVC_ERROR,NULL,2,0,lpszStrings,NULL);
        DeregisterEventSource(event_source);
    }

}

VOID Set_Actions(SC_HANDLE Service_Handle)
{

    SERVICE_FAILURE_ACTIONSW failure_actions;
    SC_ACTION actions[3];

    // Set up the actions for each event
    actions[0].Type = SC_ACTION_RESTART;
    actions[0].Delay = 10000; // 10 seconds

    actions[1].Type = SC_ACTION_RESTART;
    actions[1].Delay = 10000; // 10 seconds

    actions[2].Type = SC_ACTION_RESTART;
    actions[2].Delay = 10000; // 10 seconds

    failure_actions.dwResetPeriod = 3600; // 1 hour
    failure_actions.lpRebootMsg = NULL; // No reboot message
    failure_actions.lpCommand = NULL; // No command to run
    failure_actions.cActions = 3; // Three actions to set up
    failure_actions.lpsaActions = actions;

    // Change the service configuration to set the failure actions
    if (!ChangeServiceConfig2(Service_Handle, SERVICE_CONFIG_FAILURE_ACTIONS, &failure_actions)) {
        // Handle the error case
        Event_Report(LPWSTR("[ChangeServiceConfig2] setting failure actions returned error"));
    }
    return;

}

VOID Install(){
    SC_HANDLE SCM;
    SC_HANDLE Service;
    wchar_t UnquotedPath[MAX_PATH];
    if( !GetModuleFileNameW( NULL, UnquotedPath, MAX_PATH ) )
    {
        std::cout<<"Error: "<< GetLastError();
        return;
    }

    wchar_t Path[MAX_PATH];
    StringCchPrintfW(Path, MAX_PATH, TEXT("\"%s\""), UnquotedPath);

    // Get a handle to the SCM database.
    SCM = OpenSCManager(
        NULL, // local computer
        NULL, // ServicesActive database
        SC_MANAGER_ALL_ACCESS); // full access rights

    if (NULL == SCM)
    {
        std::cout<<"Failed in opening Service Manager, Error: "<< GetLastError();
        return;
    }
    // Create the service
    Service = CreateServiceW(SCM, SVCNAME, SVCNAME, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
                             SERVICE_ERROR_NORMAL, Path, NULL, NULL, NULL, NULL, NULL);

    if (Service == NULL)
    {
        std::cout<<"Failed to create service. Error: "<<GetLastError();
        CloseServiceHandle(SCM);
        return;
    }
    else
    {
        std::cout<<"Service Installed Successfully";
        SERVICE_DESCRIPTIONW service_description = {0};
        service_description.lpDescription = (LPWSTR)SVCDES;

        if (!ChangeServiceConfig2(Service, SERVICE_CONFIG_DESCRIPTION, &service_description)) {
            // Handle the error case
            Event_Report(LPWSTR("[ChangeServiceConfig2] setting failure actions returned error"));
        }
        //set_actions(Service);

    }
    CloseServiceHandle(Service);
    CloseServiceHandle(SCM);
}

VOID Delete(void){
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        NULL, // local computer
        NULL, // ServicesActive database
        SC_MANAGER_ALL_ACCESS); // full access rights

    if (NULL == schSCManager)
    {
        std::cout<<"Failed Opening Service Manager. Error: "<< GetLastError();
        return;
    }

    // Get a handle to the service.
    schService = OpenService(
        schSCManager, // SCM database
        SVCNAME, // name of service
        DELETE); // need delete access

    if (schService == NULL)
    {
        std::cout<<"Failed Opening Service. Error: "<< GetLastError();
        CloseServiceHandle(schSCManager);
        return;
    }
    // Delete the service.
    if (!DeleteService(schService))
    {
        std::cout<<"Failed to delete the service. Error: "<<GetLastError();
        CloseServiceHandle(schSCManager);
        CloseServiceHandle(schService);
    }
    else
    {
        std::cout<<"Service Deleted Auccessfully.";
    }
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}
