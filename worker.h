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

class win_api;

class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = nullptr);
    ~worker();
    int max_threshold, min_threshold;
    QString newversion, oldversion, mac;
    bool fresh=true, pop=false, temp=false, plugged=false, should_run=true, client_connected=false;
signals:
    void Plugged_();
    void UnPlugged_();

public slots:
    void Fetch_Settings();
    QString GetIp();
    QString Request(QString index);
    void Fresh_Check();
    void Check_Update();
    void Plugged_Status();
    void Thresholds_Status();
private:
    QNetworkAccessManager *manager;
    QString IP;
    QScopedPointer<win_api> win;
    SYSTEM_POWER_STATUS status;
};

#endif // WORKER_H
