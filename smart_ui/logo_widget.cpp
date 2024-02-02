#include "logo_widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QFontDatabase>
logo_widget::logo_widget(QWidget *parent)
    : QWidget{parent}
{
    QLabel *logo_label=new QLabel(this);
    QLabel *logo_text= new QLabel(this);
    logo_text->setObjectName("logo_text");
    logo_label->setObjectName("logo_label");
    QVBoxLayout *logo_widget_layout=new QVBoxLayout(this);
    logo_widget_layout->addWidget(logo_label,3);
    logo_widget_layout->addWidget(logo_text, 1);
    logo_widget_layout->setContentsMargins(0,0,0,0);
    logo_widget_layout->setSpacing(0);
    QPixmap logo_pic;
    logo_pic.load("://logo/icons8-light-switch-80 (1).png");
    logo_label->setPixmap(logo_pic.scaled(80,80));
    logo_label->setAlignment(Qt::AlignCenter);
    logo_text->setText("S M A R T  S W I T C H");
    logo_text->setAlignment(Qt::AlignCenter);

    int id = QFontDatabase::addApplicationFont("://font/SteelrDemo.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont steel(family);
    steel.setPointSize(10);
    logo_text->setFont(steel);
}
