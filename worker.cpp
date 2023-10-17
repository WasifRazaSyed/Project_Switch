#include "worker.h"

worker::worker(QObject *parent)
    : QObject{parent}
{

}

void worker::setIP()
{

}

void worker::Fresh_Check()
{

}

void worker::Reset()
{

}

void worker::Log(const QString &text)
{

}

void worker::Fetch_Settings()
{

}

void worker::Update_Check()
{

}

void worker::Plugged()
{

}

void worker::UnPlugged()
{

}

QString worker::GetIp()
{
    QList<QNetworkInterface> list=QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface InterFace, list)
    {
        if(InterFace.flags().testFlag(QNetworkInterface::IsUp)
            && InterFace.flags().testFlag(QNetworkInterface::IsRunning)
            && !InterFace.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            QList<QNetworkAddressEntry> IP_entries=InterFace.addressEntries();
            foreach(QNetworkAddressEntry IP, IP_entries)
            {
                if(IP.ip().protocol()==QAbstractSocket::IPv4Protocol)
                {
                    return IP.ip().toString();
                }
            }
        }
    }
    return QString();
}
