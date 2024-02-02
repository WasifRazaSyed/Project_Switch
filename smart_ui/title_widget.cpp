#include "title_widget.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFontDatabase>

title_widget::title_widget(QWidget *parent)
    : main_parent{parent}
{
    int id = QFontDatabase::addApplicationFont("://font/Nunito-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont regular(family);
    regular.setPointSize(11);

    QLabel *logo=new QLabel(this);
    title=new title_bar(main_parent);
    QPushButton *min=new QPushButton(this);
    QPushButton *exit=new QPushButton(this);

    exit->setObjectName("exit");
    min->setObjectName("min");
    title->setObjectName("title");
    logo->setObjectName("logo");

    QHBoxLayout *title_widget_layout=new QHBoxLayout(this);
    title_widget_layout->addWidget(logo, 1);
    title_widget_layout->addWidget(title, 12);
    title_widget_layout->addWidget(min, 1);
    title_widget_layout->addWidget(exit, 1);
    title_widget_layout->setContentsMargins(0,0,0,0);
    title_widget_layout->setSpacing(0);
    logo->setFixedWidth(30);
    min->setFixedHeight(main_parent->size().height()/10);
    exit->setFixedHeight(main_parent->size().height()/10);
    QPixmap logo_pic;
    logo_pic.load("://logo/icons8-gear-30.png");
    logo->setPixmap(logo_pic.scaled(30,30));
    exit->setIcon(QIcon("://logo/icons8-close (1).svg"));
    min->setIcon(QIcon("://logo/icons8-subtract-32 (3).png"));
    title->setText("Choose smart settings");
    title->setFont(regular);

    connect(min, &QPushButton::clicked, [=](){
        main_parent->showMinimized();
    });
    connect(exit, &QPushButton::clicked, [=](){
        emit exit_signal();
    });

}
