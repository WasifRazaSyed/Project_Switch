#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QFrame>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <synchapi.h>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <wtsapi32.h>
#include <windows.h>
class status_bar : public QWidget
{
    Q_OBJECT
public:
    explicit status_bar(QWidget *parent = nullptr);
    ~status_bar();
    QString getCurrentUserName();
private:
    QLabel *version;
    QProgressBar *progress;
    QString oldversion;    
    QString newversion;
    QProcess *installerProcess;
    QNetworkAccessManager *manager;

    void checkupdate(QNetworkAccessManager *manager, QUrl url_v, QHBoxLayout*);
    int downloadupdate(QNetworkAccessManager *manager,QHBoxLayout*);
    void startprocess();
};

#endif // STATUS_BAR_H
