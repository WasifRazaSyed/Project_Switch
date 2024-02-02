#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include <QLabel>
#include <QWidget>
class title_bar : public QLabel
{
    Q_OBJECT
public:
    explicit title_bar(QWidget *parent = nullptr);

private:
    QWidget *main_parent;
    QPoint cursor;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

};

#endif // TITLE_BAR_H
