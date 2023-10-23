#include "worker.h"
#include "win_api.h"

worker::worker(QObject *parent)
    : QObject{parent}, win(new win_api(this))
{
    connect(this, &worker::Plugged_, this, &worker::Plugged_In);
    connect(this, &worker::UnPlugged_, this, &worker::Plugged_Out);
	
    Fetch_Settings();

}

worker::~worker()
{
    should_run=false;
    win->should_run=false;
}

void worker::Fetch_Settings()
{
    QMutexLocker locker(&mutex);
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

void worker::concurrent_starter()
{
    Q_UNUSED(QtConcurrent::run([this](){Plugged_Status();}));
    Q_UNUSED(QtConcurrent::run([this](){Thresholds_Status();}));
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
            client_mac=InterFace.hardwareAddress().toLower().remove(QChar(':'));
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

void worker::SetIp()
{
    QString Ip=GetIp();
    QStringList octet=Ip.split('.');
    if(octet.size()>=3)
    {
        QString common=octet[0]+"."+octet[1]+"."+octet[2];
        QString ip=common+".47";
        this->IP=ip;
    }
}

QString worker::Request(QString index)
{
    QString result;
    QNetworkAccessManager local;
    QUrl url("http://"+IP+"/"+index);
    QNetworkRequest request(url);
    QNetworkReply *reply=local.get(request);

    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(4000);
    connect(&timeout, &QTimer::timeout, this, [&](){
        if(reply->isRunning())
        {
            reply->abort();
        }
    });

    timeout.start();

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
    fresh=false;
    GetSystemPowerStatus(&status);
    if(status.ACLineStatus)
    {
        plugged=true;
        SetIp();
        QString tempMac=win->GetMacAdd(this->IP);
        if(tempMac==mac)
        {
            QString result=Request("status");
            if(result=="0")
            {
                result.clear();
                result=Request("current");
                if(result=="1")
                {
                    result.clear();
                    result=Request("connect");
                    if(result=="connected")
                    {
                        client_connected=true;
                        QNetworkAccessManager very_local_manager;
                        QUrl mac_url(QString("http://"+IP+"/setmac?mac=%1").arg(client_mac));
                        QNetworkRequest set_mac_req(mac_url);
                        very_local_manager.get(set_mac_req);

                        GetSystemPowerStatus(&status);
                        if(status.BatteryLifePercent>=min_threshold)
                        {
                            result=Request("off");
                            if(result=="false")
                            {
                                if(status.ACLineStatus==0)
                                {
                                    QString parser="Turned off at: "+QString::number(status.BatteryLifePercent);
                                    Log(parser);
                                }
                            }
                            if(status.BatteryLifePercent<=max_threshold)
                            {
                                special=true;
                            }
                        }
                    }
                }
            }
        }
    }
    concurrent_starter();
    return;
}

void worker::Check_Update()
{
    QEventLoop loop;
    QNetworkAccessManager local;
    QUrl version("https://api.github.com/repos/WasifRazaSyed/ProFile/releases/latest");
    QNetworkRequest request(version);
    QNetworkReply* reply = local.get(request);

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

    singleshot->start();

    connect(reply, &QNetworkReply::finished, this, [&](){
        if(reply->error()==QNetworkReply::NoError)
        {
            QByteArray jsonData=reply->readAll();
            QJsonDocument jsonDoc=QJsonDocument::fromJson(jsonData);
            if(jsonDoc.isObject())
            {
                QJsonObject jsonObject=jsonDoc.object();
                if(jsonObject.contains("tag_name"))
                {
                    newversion=jsonObject["tag_name"].toString();
                    if(oldversion!=newversion && newversion !="")
                    {
                        win->Open_Updater();
                    }
                }
            }
        }
        loop.quit();
    });
    loop.exec();
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
        QThread::msleep(15);
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
                SetIp();
                QString result;
                result=Request("status");
                if(result=="1")
                {
                    result.clear();
                    result=Request("getmac");
                    if(result==client_mac)
                    {
                        result.clear();
                        result=Request("off");
                        if(result=="false")
                        {
                            GetSystemPowerStatus(&status);
                            if(status.ACLineStatus==0)
                            {
                                client_connected=true;
                                QString parser="Turned off at: "+QString::number(status.BatteryLifePercent);
                                Log(parser);
                            }
                        }
                        else
                        {
                            client_connected=false;
                        }
                    }
                    else
                    {
                        client_connected=false;
                    }
                }
                else if(result=="0")
                {
                    result.clear();
                    result=Request("current");
                    if(result=="1")
                    {
                        result.clear();
                        result=Request("connect");
                        if(result=="connected")
                        {
                            QNetworkAccessManager very_local_manager;
                            QUrl mac_url(QString("http://"+IP+"/setmac?mac=%1").arg(client_mac));
                            QNetworkRequest set_mac_req(mac_url);
                            very_local_manager.get(set_mac_req);

                            client_connected=true;
                            result.clear();
                            result=Request("off");
                            if(result=="false")
                            {
                                GetSystemPowerStatus(&status);
                                if(status.ACLineStatus==0)
                                {
                                    QString parser="Turned off at: "+QString::number(status.BatteryLifePercent);
                                    Log(parser);
                                }
                            }
                            else
                            {
                                client_connected=false;
                            }
                        }
                    }
                    else
                    {
                        client_connected=false;
                    }
                }
                else
                {
                    client_connected=false;
                }

            }
        }
        else if(status.BatteryLifePercent<=min_threshold)
        {
            if(client_connected)
            {
                SetIp();
                QString result;
                result=Request("status");
                if(result=="1")
                {
                    result.clear();
                    result=Request("getmac");
                    if(result==client_mac)
                    {
                        result.clear();
                        result=Request("online");
                        if(result=="0")
                        {
                            result.clear();
                            result=Request("on");
                            if(result=="true")
                            {
                                GetSystemPowerStatus(&status);
                                if(status.ACLineStatus)
                                {
                                    QString parser="Turned on at: "+QString::number(status.BatteryLifePercent);
                                    Log(parser);
                                }
                            }
                            else
                            {
                                client_connected=false;
                            }
                        }
                        else
                        {
                            client_connected=false;
                        }
                    }
                    else
                    {
                        client_connected=false;
                    }
                }
                else if(result=="0")
                {
                    //do nothing
                }
                else
                {
                    client_connected=false;
                }
            }
        }
        QThread::msleep(500);
    }while(should_run==true);
}

void worker::Plugged_In()
{
    QThread::sleep(5);
    GetSystemPowerStatus(&status);
    if(status.ACLineStatus)
    {
        Fetch_Settings();
        SetIp();
        if(client_connected)
        {
            QString result;
            result=Request("status");
            if(result=="1")
            {
                result.clear();
                GetSystemPowerStatus(&status);
                if(status.BatteryLifePercent<=min_threshold)
                {
                    result=Request("getmac");
                    if(result!=client_mac)
                    {
                        client_connected=false;
                    }
                }
                else
                {
                    if(special)
                    {
                        special=false;
                    }
                    else
                    {
                        client_connected=false;
                        result.clear();
                        Request("reset");
                    }
                }
            }
            else if(result=="0")
            {
                result.clear();
                result=Request("current");
                if(result=="0")
                {
                    client_connected=false;
                }
                else if(result=="1")
                {
                    result.clear();
                    result=Request("connect");
                    if(result=="connected")
                    {
                        QNetworkAccessManager very_local_manager;
                        QUrl mac_url(QString("http://"+IP+"/setmac?mac=%1").arg(client_mac));
                        QNetworkRequest set_mac_req(mac_url);
                        very_local_manager.get(set_mac_req);

                        GetSystemPowerStatus(&status);
                        if(status.BatteryLifePercent>=min_threshold)
                        {
                            result.clear();
                            result=Request("off");
                            if(result=="false")
                            {
                                result.clear();
                                if(status.ACLineStatus==0)
                                {
                                    if(status.BatteryLifePercent<=max_threshold)
                                    {
                                        special=true;
                                    }
                                    QString parser="Turned off at: " + QString::number(status.BatteryLifePercent);
                                    Log(parser);
                                }
                            }
                            else
                            {
                                client_connected=false;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            QThread::sleep(15);
            QString result=Request("status");
            if(result=="0")
            {
                QString temp_Mac=win->GetMacAdd(IP);
                if(mac==temp_Mac)
                {
                    result.clear();
                    result=Request("current");
                    if(result=="1")
                    {
                        result.clear();
                        result=Request("connect");
                        if(result=="connected")
                        {
                            QNetworkAccessManager very_local_manager;
                            QUrl mac_url(QString("http://"+IP+"/setmac?mac=%1").arg(client_mac));
                            QNetworkRequest set_mac_req(mac_url);
                            very_local_manager.get(set_mac_req);

                            client_connected=true;

                            GetSystemPowerStatus(&status);
                            if(status.BatteryLifePercent>=min_threshold)
                            {
                                result.clear();
                                result=Request("off");
                                if(result=="false")
                                {
                                    GetSystemPowerStatus(&status);
                                    if(status.ACLineStatus==0)
                                    {
                                        if(status.BatteryLifePercent<=max_threshold)
                                        {
                                            special=true;
                                        }
                                        QString parser="Turned off at: "+QString::number(status.BatteryLifePercent);
                                        Log(parser);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void worker::Plugged_Out()
{
    QThread::sleep(2);
    GetSystemPowerStatus(&status);
    if(status.ACLineStatus==0)
    {
        Fetch_Settings();
        SetIp();
        if(client_connected==true)
        {
            if(!(status.BatteryLifePercent>=max_threshold))
            {
                QString result;
                result=Request("status");
                if(result=="0")
                {
                    client_connected=false;
                }
                else if(result=="1")
                {
                    result.clear();
                    result=Request("getmac");
                    if(result==client_mac)
                    {
                        result.clear();
                        result=Request("online");
                        if(result=="0")
                        {
                            result.clear();
                            result=Request("on");
                            if(result=="true")
                            {
                                GetSystemPowerStatus(&status);
                                if(status.ACLineStatus)
                                {
                                    QString parser="Turned on at: "+QString::number(status.BatteryLifePercent);
                                    Log(parser);
                                }
                                else
                                {
                                    client_connected=false;
                                }
                            }
                            else
                            {
                                client_connected=false;
                            }
                        }
                        else if(result=="1")
                        {
                            client_connected=false;
                            Request("reset");
                        }
                    }
                    else
                    {
                        client_connected=false;
                    }
                }
            }
        }
    }
}

void worker::Log(const QString text)
{
    QString path = win->getCurrentUserName();
    QString filename = "C:/Users/" + path + "/AppData/Roaming/SmartSettings/Activity.txt";
    QFile file(filename);

    // Open the file in append mode
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);

        // Check if the file is open for writing
        if (file.isOpen() && file.isWritable())
        {
            // Write the text and current time to the end of the file
            out << text << "\n" << QDateTime::currentDateTime().toString("h:mm:ss ap dd/MM/yyyy") << "\n\n";
            // Close the file
            file.close();
        }
    }
}

void worker::shutdown()
{
    reset();
    win->should_run=false;
    should_run=false;
}

void worker::reset()
{
    if(client_connected)
    {
        SetIp();
        QString result;
        result=Request("getmac");
        if(result==client_mac)
        {
            result.clear();
            result=Request("reset");
        }
    }
}

