#ifndef WIDGETCHAT_H
#define WIDGETCHAT_H

#include <QWidget>
#include "widgetchatcenter.h"

namespace Ui {
    class WidgetChat;
}

class WidgetChat : public QWidget {
    Q_OBJECT
public:
    WidgetChat(QWidget *parent = 0);
    ~WidgetChat();
	WidgetChatCenter *panelChatCenter;
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetChat *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETCHAT_H
