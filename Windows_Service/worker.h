#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QEventLoop>
#include <QScopedPointer>
#include <QSettings>
#include <windows.h>
#include <QMutexLocker>
#include <QMutex>

class win_api;

class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = nullptr);
    ~worker();
    int max_threshold, min_threshold;
    QString newversion, oldversion, mac, client_mac;
    bool fresh=true, pop=false, temp=false, plugged=false, should_run=true, client_connected=false, special=false, fresh_API=false;
signals:
    void quit();
public slots:
    void Fetch_Settings();
    void concurrent_starter();
    QString GetIp();
    void SetIp();
    QString Request(QString index);
    void Fresh_Check();
    void Check_Update();
    void Plugged_Status();
    void Thresholds_Status();
    void Plugged_In();
    void Plugged_Out();
    void Log(const QString text);
    int shutdown();
    void reset();
private:
    int destroyed=0;
    QMutex mutex;
    QString IP;
    QScopedPointer<win_api> win;
    SYSTEM_POWER_STATUS status;
};

#endif // WORKER_H
