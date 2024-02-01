#ifndef WIN_API_H
#define WIN_API_H

#include <QObject>
#include <QtConcurrent>
#include <QFuture>

#include <Windows.h>
#include <wtsapi32.h>
#include <IntSafe.h>
#include <iphlpapi.h>

class worker;

class win_api : public QObject
{
    Q_OBJECT
public:
    explicit win_api(worker *parent = nullptr);
    bool should_run=true;
public slots:
    void trigger_session();
    void Session_Obs();
    DWORD GetActiveSessionId();
    QString GetMacAdd(QString);
    bool PhysToStringAdd(BYTE PhysAdd[], DWORD PhysAddLen, char StringAdd[]);
    void Open_Updater();
    QString getCurrentUserName();
private:
    worker *m_worker;
    DWORD newSessionId, activeSessionId;
    MSG msg;
};

#endif // WIN_API_H

