#include "saver.h"
#include <QCoreApplication>
#include <QTimer>
#include <Windows.h>
#include <wtsapi32.h>

QString getCurrentUserName()
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

saver::saver(QObject *parent)
    : QObject{parent}
{
    QSettings private_reg("smart_switch", "smart_settings");
    private_reg.setValue("mac/version", "v1.1");

    QString path=getCurrentUserName();
    QString location="C:/Users/"+path+"/AppData/Roaming/SmartSettings/settings.ini";
    QSettings settings(location,QSettings::IniFormat);
    settings.beginGroup("mac");
    settings.setValue("version", "v1.1");
    settings.endGroup();

    QTimer::singleShot(50, this, [=](){
        shut();
    });
}

saver::~saver()
{

}

void saver::shut()
{
    QCoreApplication::exit();
}
