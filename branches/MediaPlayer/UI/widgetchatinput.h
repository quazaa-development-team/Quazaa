#ifndef WIDGETCHATINPUT_H
#define WIDGETCHATINPUT_H

#include <QtGui>
#include "widgetreturnemittextedit.h"
#include "widgetsmileylist.h"

#include "network.h"

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
	void closing();

protected:
	void changeEvent(QEvent *e);
	WidgetReturnEmitTextEdit *textEditInput;

private:
	Ui::WidgetChatInput *ui;
	QCheckBox *checkBoxSendOnEnter;
	QToolButton *toolButtonSmilies;
	WidgetSmileyList *widgetSmileyList;
	QToolButton *toolButtonPickColor;
	QToolButton *toolButtonPrivateMessage;

public slots:
	void setText(QString text);

private slots:
	void on_actionUnderline_toggled(bool checked);
	void on_actionItalic_toggled(bool checked);
	void on_actionBold_toggled(bool checked);
	void on_toolButtonSend_clicked();
	void onTextFormatChange(QTextCharFormat newFormat);
	void pickColor();
	void addPrivateMessage();
};

#endif // WIDGETCHATINPUT_H
