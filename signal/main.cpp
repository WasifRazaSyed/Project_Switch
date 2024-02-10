#include <QApplication>
#include "remote.h"
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QString>
#include <QUrl>
#include <QByteArray>
#include <QDir>
#include <QStandardPaths>
#include <Windows.h>
#include <wtsapi32.h>

void process();
int download();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon("://logo/icons8-signal-32.ico"));

    if(a.arguments().contains("frominstaller"))
    {
        QSharedPointer<reactReplica> ptr; // shared pointer to hold source replica

        QRemoteObjectNode repNode; // create remote object node
        repNode.connectToNode(QUrl(QStringLiteral("local:switch"))); // connect with remote host node

        ptr.reset(repNode.acquire<reactReplica>()); // acquire replica of source from host node

        remote rswitch(ptr);
        return a.exec();
    }
    else if(a.arguments().contains("fromservice"))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("New version available! Do you want to download and install it?\n\nMake sure to disable your antivirus before clicking Yes. The setup will show up once downloaded.");
        msgBox.setWindowTitle("Smart Switch Update");

        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret=msgBox.exec();

        if(ret==QMessageBox::Yes)
        {

            int ret=download();
            if(ret!=1)
            {
                QMessageBox::critical(nullptr, "Smart Switch", "Something bad happened. Please try again later", QMessageBox::Ok);
                a.exit();
            }
            else{
            return a.exec();
            }
        }
        else{
            a.exit();
        }

    }
    else{
        a.exit();
    }

}
QString getCurrentUserName()
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
int download()
{
    QUrl url("https://github.com/WasifRazaSyed/Project_Switch/blob/main/setup/Setup.exe?raw=true");
    QString user=getCurrentUserName();
    QString tempPath = "C:/Users/"+user+"/AppData/Local/Temp/Setup.exe";
    QNetworkRequest request(url);
    QNetworkReply* reply;
    QNetworkAccessManager *manager=new QNetworkAccessManager(nullptr);

    reply=manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished ,[=](){
        QByteArray data = reply->readAll();
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_6_1); // Set the stream version to Qt 6.1 to match the x64 architecture
            out.writeRawData(data.constData(), data.size()); // Write the binary data to the file
            file.close();
            process();
            return 1;
        }
        else{
            return 0;
        }

    });
    if (reply->error() != QNetworkReply::NoError) {
        return 0;
    }
    else{
        return 1;
    }
    reply->deleteLater();
    manager->deleteLater();
    return 1;
}

void process(){
    QProcess *installerProcess;
    QString user=getCurrentUserName();
    QString tempPath = "C:/Users/"+user+"/AppData/Local/Temp/Setup.exe";
    installerProcess = new QProcess(nullptr);
    installerProcess->startDetached(tempPath);
    installerProcess->deleteLater();
    QApplication::exit();
}
