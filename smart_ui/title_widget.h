#ifndef TITLE_WIDGET_H
#define TITLE_WIDGET_H

#include <QWidget>
#include <title_bar.h>
class title_widget : public QWidget
{
    Q_OBJECT
public:
    explicit title_widget(QWidget *parent = nullptr);
private:
    title_bar *title;
    QWidget *main_parent;
signals:
    void exit_signal();
};

#endif // TITLE_WIDGET_H
