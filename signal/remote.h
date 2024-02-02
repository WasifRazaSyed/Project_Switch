#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include "rep_remote_replica.h"
#include <QSharedPointer>
class remote : public QObject
{
    Q_OBJECT
public:
    explicit remote(QSharedPointer<reactReplica> ptr);
    ~remote();
signals:
    void exit();
private:
    QSharedPointer<reactReplica> reptr;
    void init();
};

#endif // REMOTE_H


