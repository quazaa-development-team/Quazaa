#ifndef WIDGETCHATCENTER_H
#define WIDGETCHATCENTER_H

#include <QMainWindow>

namespace Ui {
    class WidgetChatCenter;
}

class WidgetChatCenter : public QMainWindow {
    Q_OBJECT
public:
    WidgetChatCenter(QWidget *parent = 0);
    ~WidgetChatCenter();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetChatCenter *ui;
};

#endif // WIDGETCHATCENTER_H
