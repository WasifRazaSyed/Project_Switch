#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QNetworkInterface>

class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = nullptr);

signals:
    void Plugged_();
    void UnPlugged_();

public slots:
    void setIP();
    void Fresh_Check();
    void Reset();
    void Log(const QString& text);
    void Fetch_Settings();
    void Update_Check();
    void Plugged();
    void UnPlugged();
    QString GetIp();
private:

};

#endif // WORKER_H
