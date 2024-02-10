#include "status_bar.h"

status_bar::status_bar(QWidget *parent)
    : QWidget(parent)
{
    QString user=getCurrentUserName();
    QString tempPath = "C:/Users/"+user+"/AppData/Local/Temp/Setup.exe";
    QFile file(tempPath);
    if (file.exists()) {
        if (!file.remove()) {
        }
    }


    QUrl url_v("https://api.github.com/repos/WasifRazaSyed/Project_Switch/releases/latest");
    QSettings private_reg("smart_switch", "smart_settings");
    QString location=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/settings.ini";
    QSettings private_ini(location,QSettings::IniFormat);

    private_reg.beginGroup("mac");
    oldversion=private_reg.value("version").toString();
    private_reg.endGroup();

    if(oldversion.isEmpty()){
        private_ini.beginGroup("mac");
        oldversion=private_ini.value("version").toString();
        private_ini.endGroup();

        if(oldversion.isEmpty()){
            oldversion="v1.0";
            private_ini.setValue("mac/version", "v1.0");
            private_reg.setValue("mac/version", "v1.0");
        }
    }
    version=new QLabel(this);
    version->setText(oldversion);
    version->setObjectName("version");
    version->setFixedHeight(size().height());


    QHBoxLayout *status_layout=new QHBoxLayout(this);
    status_layout->addWidget(version);

    status_layout->setSpacing(0);
    status_layout->setContentsMargins(0,0,0,0);

    manager=new QNetworkAccessManager(this);
    version->setText("Checking for updates . . . . . ");
    checkupdate(manager, url_v, status_layout);

}

status_bar::~status_bar()
{

}

QString status_bar::getCurrentUserName()
{
    DWORD sessionId = WTSGetActiveConsoleSessionId();
        if (sessionId == 0xFFFFFFFF) {
            // handle the error
            return QString();
        }

        LPWSTR username = nullptr;
        DWORD size = 0;
        if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessionId, WTSUserName, &username, &size)) {
            WCHAR* wUsername = new WCHAR[size / sizeof(WCHAR)];
            wcscpy_s(wUsername, size / sizeof(WCHAR), username);
            QString result = QString::fromWCharArray(wUsername);
            WTSFreeMemory(username);
            delete[] wUsername;
            return result;
        } else {
            // handle the error
            return QString();
        }
}

void status_bar::checkupdate(QNetworkAccessManager *manager, QUrl url_v, QHBoxLayout *status_layout)
{
    QNetworkRequest request(url_v);
    QNetworkReply* reply = manager->get(request);
    QTimer::singleShot(30000, this, [=](){
        if(newversion.isEmpty()){
            reply->abort();
            version->setText("Network Error.");
            QTimer::singleShot(5000, this, [=](){
                version->setText(oldversion);
            });
            return;
        }
    });
    connect(qApp, &QApplication::aboutToQuit, reply, &QNetworkReply::abort);
    connect(reply, &QIODevice::readyRead, this, [=](){
        if (reply->error() != QNetworkReply::NoError) {
            reply->abort();
            version->setText("An error occurred, Please try again later");
            QTimer::singleShot(3000, this, [=](){
                version->setText(oldversion);
            });
            return;
        }
        QByteArray data = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data);

        if (document.isNull()) {
            return;
        }

        QJsonObject rootObject = document.object();
        newversion = rootObject.value("tag_name").toString();
        if(oldversion!=newversion && newversion !=""){
            version->setText("New version is on the desk.");
            int ret=QMessageBox::information(this, "Update Available","New version available! Do you want to download and install it?\n\nMake sure to disable you antivirus software to begin the installation.", QMessageBox::Yes, QMessageBox::No);
            if(ret==QMessageBox::Yes){

                delete version;
                progress=new QProgressBar(this);
                progress->setFixedHeight(size().height());
                progress->setTextVisible(true);
                status_layout->addWidget(progress);

                int ret=downloadupdate(manager, status_layout);
                if(ret!=1){
                    QMessageBox::critical(this, "Error", "Something bad happened. Please try again later", QMessageBox::Ok);
                }
            }
            else{
                version->setText("New version is on the desk, restart the application and download it when you are ready.");
                QTimer::singleShot(5000,this,  [=](){
                    version->setText(oldversion);
                });
            }
            reply->deleteLater();
        }
        else{
            version->setText("Current version is up-to-date");
            QTimer::singleShot(3000,this, [=](){
                version->setText(oldversion);
            });
            reply->deleteLater();
        }
    });
}

int status_bar::downloadupdate(QNetworkAccessManager *manager, QHBoxLayout *status_layout)
{
    QUrl url("https://github.com/WasifRazaSyed/Project_Switch/blob/main/setup/Setup.exe?raw=true");
    QString user=getCurrentUserName();
    QString tempPath = "C:/Users/"+user+"/AppData/Local/Temp/Setup.exe";
    QNetworkRequest request(url);
    QNetworkReply* reply;

    reply=manager->get(request);
    connect(reply, &QNetworkReply::downloadProgress,  this, [=](qint64 bytesReceived, qint64 bytesTotal) {
        int progress_value = static_cast<int>((bytesReceived * 100) / bytesTotal);
        progress->setValue(progress_value);
    });
    connect(reply, &QNetworkReply::finished, this,[=](){
        QByteArray data = reply->readAll();
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_6_1); // Set the stream version to Qt 6.1 to match the x64 architecture
            out.writeRawData(data.constData(), data.size()); // Write the binary data to the file
            file.close();
            startprocess();
            return 1;
        }
        else{
            return 0;
        }

    });
    if (reply->error() != QNetworkReply::NoError) {
        delete progress;
        version=new QLabel(this);
        status_layout->addWidget(version, 12);
        version->setText("Network Error");
        QTimer::singleShot(3000, this,[=](){
            version->setText(oldversion);
        });
        return 0;
    }
    else{
        return 1;
    }
    reply->deleteLater();
    return 1;
}

void status_bar::startprocess()
{   
    QString user=getCurrentUserName();
    QString tempPath = "C:/Users/"+user+"/AppData/Local/Temp/Setup.exe";
    installerProcess = new QProcess(this);
    installerProcess->startDetached(tempPath);
    QApplication::exit();
}










