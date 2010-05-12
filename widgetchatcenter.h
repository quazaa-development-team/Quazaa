#ifndef WIDGETCHATCENTER_H
#define WIDGETCHATCENTER_H

#include <QMainWindow>
#include <QLineEdit>
#include <QToolButton>

#include "quazaairc.h"

namespace Ui {
    class WidgetChatCenter;
}

class WidgetChatCenter : public QMainWindow {
    Q_OBJECT
public:
    WidgetChatCenter(QWidget *parent = 0);
    ~WidgetChatCenter();
	QLineEdit *lineEditTextInput;
	QToolButton *toolButtonSmilies;
	QToolButton *toolButtonOp;
	QuazaaIRC *m_oQuazaaIrc;
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetChatCenter *ui;

private slots:
        void on_actionConnect_triggered();
	void on_actionChatSettings_triggered();
	void skinChangeEvent();
};

#endif // WIDGETCHATCENTER_H
