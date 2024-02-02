#ifndef SAVER_H
#define SAVER_H

#include <QObject>
#include <QSettings>
#include <QStandardPaths>
class saver : public QObject
{
    Q_OBJECT
public:
    explicit saver(QObject *parent = nullptr);
    ~saver();
    void shut();
signals:

};

#endif // SAVER_H
