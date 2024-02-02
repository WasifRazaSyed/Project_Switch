#include "widget.h"
#include <QApplication>
#include <QScreen>
#include <QGridLayout>
#include <QFile>
#include <QFontDatabase>
#include <QPalette>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Smart_Switch");
    QFile file("://style/stylesheet.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    setStyleSheet(styleSheet);
    setMaximumSize(550, 320);
    setMinimumSize(550,320);
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowIcon(QIcon("://logo/win_icon.ico"));
    logo=new logo_widget(this);
    title=new title_widget(this);
    set=new settings_widget(this);
    status=new status_bar(this);

    QGridLayout *widget_main_layout=new QGridLayout(this);
    widget_main_layout->addWidget(title, 0, 0, 1, 3);
    widget_main_layout->addWidget(logo, 1, 0, 4, 1);
    widget_main_layout->addWidget(set, 1, 1, 4, 2);
    widget_main_layout->addWidget(status, 5, 0, 1, 3);


    widget_main_layout->setContentsMargins(0,0,0,0);
    widget_main_layout->setSpacing(0);
    title->setFixedHeight(this->size().height()/10);
    title->setObjectName("title_widget");

    QPalette pal=title->palette();
    pal.setColor(QPalette::Window, Qt::white);
    title->setAutoFillBackground(true);
    title->setPalette(pal);
    connect(title, &title_widget::exit_signal, set, &settings_widget::exit_check);

}

Widget::~Widget()
{

}



