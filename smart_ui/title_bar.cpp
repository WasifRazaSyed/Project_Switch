#include "title_bar.h"
#include <QMouseEvent>

title_bar::title_bar(QWidget *parent)
    : main_parent{parent}
{

}
void title_bar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        {
            cursor = event->globalPos() - main_parent->geometry().topLeft();
            event->accept();
        }
}

void title_bar::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        main_parent->move(event->globalPos()-cursor);
        event->accept();
    }
}
