#include "worker.h"
#include "win_api.h"

worker::worker(QObject *parent)
    : QObject{parent}, win(new win_api(this))
{
    manager=new QNetworkAccessManager(this);
    Q_UNUSED(QtConcurrent::run([this](){Plugged_Status();}));
    Q_UNUSED(QtConcurrent::run([this](){Thresholds_Status();}));
}

worker::~worker()
{
    should_run=false;
}

void worker::Fetch_Settings()
{
    QString path=win->getCurrentUserName();
    QString location="C:/Users/"+path+"/AppData/Roaming/SmartSettings/settings.ini";

    QSettings settings(location,QSettings::IniFormat);
    settings.beginGroup("threshold");
    min_threshold=settings.value("min_threshold").toInt();
    max_threshold=settings.value("max_threshold").toInt();
    settings.endGroup();

    settings.beginGroup("mac");
    oldversion=settings.value("version").toString();
    mac=settings.value("MAC").toString();
    settings.endGroup();

    if(fresh)
    {
        Fresh_Check();
    }

    if(pop==false)
    {
        Check_Update();
    }
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

QString worker::Request(QString index)
{
    QString result;

    QUrl url("http://"+IP+"/"+index);
    QNetworkRequest request(url);
    QNetworkReply *reply=manager->get(request);

    QScopedPointer<QEventLoop> loop(new QEventLoop());
    QObject::connect(reply, &QNetworkReply::finished, this, [&]()
    {
        if(reply->error()==QNetworkReply::NoError)
        {
            QByteArray reply_8bit=reply->readAll();
            result=QString::fromUtf8(reply_8bit);
        }
        else
        {
            result="-1";
        }
        loop->quit();
    });
    loop->exec();
    reply->deleteLater();
    return result;
}

void worker::Fresh_Check()
{

}

void worker::Check_Update()
{
    QUrl version("https://api.github.com/repos/WasifRazaSyed/ProFile/releases/latest");
    QNetworkRequest request(version);
    QNetworkReply* reply = manager->get(request);

    QTimer *singleshot=new QTimer(this);
    singleshot->setSingleShot(true);
    singleshot->setInterval(30000);

    connect(singleshot, &QTimer::timeout, this, [=](){
        if(newversion.isEmpty())
        {
            reply->abort();
            return;
        }
    });

    connect(reply, &QNetworkReply::finished, this, [=](){
        singleshot->stop();
        singleshot->deleteLater();

        if (reply->error() != QNetworkReply::NoError)
        {
            reply->abort();
            return;
        }
        QByteArray data = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data);

        if (document.isNull())
        {
            return;
        }

        QJsonObject rootObject = document.object();

        newversion = rootObject.value("tag_name").toString();
        if(oldversion!=newversion && newversion !="")
        {
            win->Open_Updater();
        }
    });
    reply->deleteLater();
}

void worker::Plugged_Status()
{
    do{
        GetSystemPowerStatus(&status);
        temp=status.ACLineStatus;
        if(temp!=plugged){
            if(temp){
                plugged=temp;
                emit Plugged_();
            }
            else if(!temp){
                plugged=temp;
                emit UnPlugged_();
            }
        }
        Sleep(10);
    }while(should_run==true);
}

void worker::Thresholds_Status()
{
    do{
        Fetch_Settings();
        GetSystemPowerStatus(&status);
        if(status.BatteryLifePercent>=max_threshold)
        {
            if(status.ACLineStatus)
            {

            }
        }
        else if(status.BatteryLifePercent<=min_threshold)
        {
            if(client_connected)
            {

            }
        }
    }while(should_run==true);
}

