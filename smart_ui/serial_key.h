#ifndef SERIAL_KEY_H
#define SERIAL_KEY_H

#include <QObject>
#include <QTextEdit>
#include <QKeyEvent>
class serial_key : public QTextEdit
{
    Q_OBJECT
public:
    serial_key(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event)
        {
            if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                // Do nothing
            } else {
                QTextEdit::keyPressEvent(event);
            }
        }
private:
    QWidget *main_parent;
};

#endif // SERIAL_KEY_H
