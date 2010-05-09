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
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetChatCenter *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETCHATCENTER_H
