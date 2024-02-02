#include <QCoreApplication>
#include "saver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    bool start=a.arguments().contains("frominstaller");
    if(start)
    {
        saver save;
    return a.exec();
    }
    else{
        a.exit();
    }
}
