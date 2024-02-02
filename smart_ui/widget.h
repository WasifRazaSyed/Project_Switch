#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "logo_widget.h"
#include "settings_widget.h"
#include "title_widget.h"
#include "status_bar.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    logo_widget *logo;
    settings_widget *set;
    title_widget *title;
    status_bar *status;
};
#endif // WIDGET_H
