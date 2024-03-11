#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <widget.h>
#include <windows.h>
#include "remote.h"

int main( int argc, char* argv[] )
{
    
    QApplication app( argc, argv );
    remote rObj;
    QRemoteObjectHost srcNode(QUrl(QStringLiteral("local:switch"))); // create host node without Registry
    srcNode.enableRemoting(&rObj);
    Widget w;
    QString title="Smart Settings";
    QSharedMemory sharedMem("unique_key..");
    if(!sharedMem.create(1)&&sharedMem.error()==QSharedMemory::AlreadyExists){
        HWND hwnd=FindWindow(NULL,title.toStdWString().c_str());
        if(hwnd){
            ShowWindow(hwnd, SW_RESTORE);
            SetForegroundWindow(hwnd);
            return 0;
        }
    }

    w.setWindowTitle(title);
    w.show();
    return app.exec();
    

}
