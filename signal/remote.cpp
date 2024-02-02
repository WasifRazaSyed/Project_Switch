#include "remote.h"
#include <QTimer>
#include <QCoreApplication>
remote::remote(QSharedPointer<reactReplica> ptr) :
    QObject(nullptr),reptr(ptr)
{
    connect(this, &remote::exit, reptr.data(), &reactReplica::shutdown);
    QTimer::singleShot(50, this, [=](){
        init();
    });
}

remote::~remote()
{

}

void remote::init()
{
    emit exit();
    QCoreApplication::exit();
}
