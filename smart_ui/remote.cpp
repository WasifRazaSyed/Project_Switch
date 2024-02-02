#include "remote.h"
#include <QApplication>

remote::remote(QObject *parent) :reactSimpleSource(parent)
{

}

remote::~remote()
{

}

void remote::shutdown()
{
    QApplication::exit();
}
