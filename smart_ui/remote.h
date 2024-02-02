#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include "rep_remote_source.h"
#include <QSettings>
#include <QStandardPaths>
class remote : public reactSimpleSource
{
    Q_OBJECT
public:
    remote(QObject *parent = nullptr);
    ~remote();
    virtual void shutdown();
};

#endif // REMOTE_H
