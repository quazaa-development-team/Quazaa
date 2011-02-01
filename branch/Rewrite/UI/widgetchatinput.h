#ifndef WIDGETCHATINPUT_H
#define WIDGETCHATINPUT_H

#include <QtGui>
#include "widgetreturnemittextedit.h"

namespace Ui {
    class WidgetChatInput;
}

class WidgetChatInput : public QMainWindow {
    Q_OBJECT
public:
    WidgetChatInput(QWidget *parent = 0);
    ~WidgetChatInput();

signals:
	void messageSent(QTextDocument *text);
	void messageSent(QString text);

protected:
    void changeEvent(QEvent *e);
	WidgetReturnEmitTextEdit *textEditInput;

private:
    Ui::WidgetChatInput *ui;
	QCheckBox *checkBoxSendOnEnter;
	QToolButton *toolButtonSmilies;

public slots:
	void setText(QString text);

private slots:
	void on_toolButtonSend_clicked();
};

#endif // WIDGETCHATINPUT_H
