#include "win_api.h"
#include "worker.h"

win_api::win_api(worker *parent): m_worker(parent)
{
    HWND hwnd = CreateWindowExW(
        0,
        LPCWSTR("STATIC"),
        LPCWSTR(""),
        WS_POPUP,
        0,
        0,
        0,
        0,
        HWND_MESSAGE,
        NULL,
        NULL,
        NULL);

    WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_ALL_SESSIONS);

    QTimer::singleShot(20000, this, [=](){
        Q_UNUSED(QtConcurrent::run([this](){Session_Obs();}));
    });

}

void win_api::Session_Obs()
{
    do
    {
        GetMessage(&msg, NULL, 0, 0);
        if (msg.message == WM_WTSSESSION_CHANGE)
        {
            DWORD eventType = (DWORD)msg.wParam;
            if (eventType == WTS_SESSION_LOGON)
            {
                QFuture<void> settings=QtConcurrent::run([this](){m_worker->Fetch_Settings();});
                settings.waitForFinished();
            }
            else if (eventType == WTS_SESSION_LOGOFF)
            {
            }
        }
        newSessionId= GetActiveSessionId();
        if(newSessionId != activeSessionId)
        {
            QFuture<void> settings=QtConcurrent::run([this](){m_worker->Fetch_Settings();});
            settings.waitForFinished();
            activeSessionId = newSessionId;
            return;
        }
        QThread::msleep(100);
    }while(should_run);
}

DWORD win_api::GetActiveSessionId()
{
    DWORD SessionId = WTSGetActiveConsoleSessionId();
    if (SessionId == 0xFFFFFFFF) {
        return 0;
    }

    HANDLE ServerHandle = WTS_CURRENT_SERVER_HANDLE;
    LPWSTR SessionName = NULL;
    DWORD SessionNameLength = 0;

    if (WTSQuerySessionInformation(ServerHandle, SessionId, WTSClientName, &SessionName, &SessionNameLength))
    {
        if (wcsstr(SessionName, L"services") == NULL)
        {
            return SessionId;
        }
    }

    return 0;
}

QString win_api::GetMacAdd(QString IP)
{
    char mac_str[256];
    QString mac;
    IPAddr ipAddr;
    unsigned char macAddress[6];

    QProcess *ping=new QProcess(this);
    QStringList arg;
    arg<<IP;
    ping->startDetached("ping -c 1 -w 2",arg);
    ping->deleteLater();

    std::string IPAdd=IP.toStdString();
    char IP_C[256];
    DWORD Ret;
    ULONG MacAddLen = 6;
    strncpy(IP_C, IPAdd.c_str(), sizeof(IP_C)-1);
    IP_C[sizeof(IP_C)-1]='\0';

    ipAddr = inet_addr (IP_C);
    Ret=SendARP(ipAddr, (unsigned long)NULL,(PULONG)&macAddress, &MacAddLen);
    if(Ret==NO_ERROR){
        PhysToStringAdd(macAddress, (DWORD)MacAddLen, mac_str);
        mac =QString::fromLocal8Bit(mac_str);
        return mac;
    }
    else if(Ret==ERROR_GEN_FAILURE)
    {
        return "1";
    }

    WSACleanup();
    return "1";
}

bool win_api::PhysToStringAdd(BYTE PhysAdd[], DWORD PhysAddLen, char StringAdd[])
{
    DWORD index;
    if (PhysAdd == NULL || PhysAddLen == 0 || StringAdd == NULL)
        return FALSE;
    StringAdd[0] = '\0';
    for (index = 0; index < PhysAddLen; index++)
    {
        sprintf_s(StringAdd+(index*2), sizeof(StringAdd+(index*2)), "%02X", ((int)PhysAdd[index])&0xff);
    }
    for (index = 0; index < (PhysAddLen*2); index++)
    {
        StringAdd[index] = tolower(StringAdd[index]); // convert to lowercase
    }
    return TRUE;
}

void win_api::Open_Updater()
{
    //this function opens the update dialog box using the signal.exe application.

    /*1. Granting enough and required privilege to the process.
      2. Getting the token of current logged-on user.
      3. Using the token, impersonating the user.
      4. Preparing the arguments and process(to be started) location
      5. Creating the process.
      6. Terminating the impersonation.
    */

    HANDLE Handler;
    BOOL Result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Handler);
    if (!Result) {}

    // Enable the SE_INCREASE_QUOTA_NAME privilege required by CreateProcessAsUser function

    TOKEN_PRIVILEGES TP;
    ZeroMemory(&TP, sizeof(TP));
    TP.PrivilegeCount = 1;
    TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //gets the LUID of specified privilege
    Result = LookupPrivilegeValue(NULL, SE_INCREASE_QUOTA_NAME, &TP.Privileges[0].Luid);
    if (!Result) {}

    //enables or disables privilege on the specified token
    Result = AdjustTokenPrivileges(Handler, FALSE, &TP, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    if (!Result) {}

    // Close the token handle
    CloseHandle(Handler);

    QString exe=QCoreApplication::applicationDirPath()+"/bin/signal.exe";
    LPCWSTR path_to_exe = reinterpret_cast<LPCWSTR>(exe.utf16());

    QString dir=QCoreApplication::applicationDirPath()+"/bin";
    LPCWSTR path_to_dir = reinterpret_cast<LPCWSTR>(dir.utf16());

    //gets the primary access token of logged-on user specified by the session ID
    HANDLE Token;
    WTSQueryUserToken(WTSGetActiveConsoleSessionId(), &Token);

    //lets the calling thread impersonate the security context of logged-on user
    BOOL result = ImpersonateLoggedOnUser(Token);
    if (!result) {}

    //process started by logged-on user are associated with WinSta0\\Default
    QString arg="WinSta0\\Default";
    std::wstring wstr = arg.toStdWString();

    //set the details of a process at creation time
    STARTUPINFO info;
    ZeroMemory(&info, sizeof(info));
    info.cb = sizeof(info);
    info.lpDesktop = const_cast<LPWSTR>(wstr.c_str());

    //this structure will be filled by the CreateProcessAsUser() function
    PROCESS_INFORMATION P_info;

    //preparing arguments to be passed to process being started
    QStringList arg0;
    arg0<<"fromservice";
    QString list=arg0.join("");
    std::wstring wide=list.toStdWString();

    //creating new process and its primary thread
    result = CreateProcessAsUserW(Token, path_to_exe, &wide[0], NULL, NULL, FALSE, 0, NULL, path_to_dir, &info, &P_info);
    if (!result) {}
    CloseHandle(Token);

    //terminating the impersonation
    result = RevertToSelf();
    if (!result) {}

    m_worker->pop=true;
}

QString win_api::getCurrentUserName()
{
    DWORD sessionId = WTSGetActiveConsoleSessionId();
    if (sessionId == 0xFFFFFFFF) {
        // handle the error
        return QString();
    }

    LPWSTR username = nullptr;
    DWORD size = 0;
    if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessionId, WTSUserName, &username, &size))
    {
        WCHAR* wUsername = new WCHAR[size / sizeof(WCHAR)];
        wcscpy_s(wUsername, size / sizeof(WCHAR), username);
        QString result = QString::fromWCharArray(wUsername);
        WTSFreeMemory(username);
        delete[] wUsername;
        return result;
    }
    else
    {
        // handle the error
        return QString();
    }
}
