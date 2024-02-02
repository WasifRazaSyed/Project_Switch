#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include <QWidget>
#include <QSlider>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QFontDatabase>
#include <QVariant>
#include <QTimer>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include "serial_key.h"
#include <QStandardPaths>

class settings_widget : public QWidget
{
    Q_OBJECT
public:
    explicit settings_widget(QWidget *parent = nullptr);
private:
    QSlider *max_slider;
    QSlider *min_slider;
    QVariant max_value;
    QVariant min_value;
    serial_key *key;
    bool check;
    QString serial_key_;

    void savesettings();
    void loadsettings();

public:
    void exit_check();

};

#endif // SETTINGS_WIDGET_H
