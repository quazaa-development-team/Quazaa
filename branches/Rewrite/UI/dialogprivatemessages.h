#ifndef DIALOGPRIVATEMESSAGES_H
#define DIALOGPRIVATEMESSAGES_H

#include <QDialog>
#include "widgetchatinput.h"
#include "Chat/chatsession.h"

class WidgetPrivateMessage;
class QTextDocument;

namespace Ui {
	class DialogPrivateMessages;
}

class DialogPrivateMessages : public QDialog {
	Q_OBJECT
public:
	DialogPrivateMessages(QWidget *parent = 0);
	~DialogPrivateMessages();

	WidgetPrivateMessage* FindByGUID(QUuid oGUID);

	void OpenChat(CChatSession* pSess);

protected:
	void changeEvent(QEvent *e);
	QList<WidgetPrivateMessage*> m_lChatWindows;

private:
	Ui::DialogPrivateMessages *ui;
	WidgetChatInput *widgetChatInput;
	WidgetPrivateMessage* m_pCurrentWidget;

public slots:
	void onMessageSent(QTextDocument* pDoc);
private slots:
	void on_tabWidgetPrivateMessages_tabCloseRequested(int index);
 void on_tabWidgetPrivateMessages_currentChanged(int index);
};

#endif // DIALOGPRIVATEMESSAGES_H
