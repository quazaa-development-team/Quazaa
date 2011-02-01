#ifndef DIALOGPRIVATEMESSAGES_H
#define DIALOGPRIVATEMESSAGES_H

#include <QDialog>
#include "widgetchatinput.h"
#include "Chat/chatsession.h"
#include "widgetprivatemessage.h"

namespace Ui {
    class DialogPrivateMessages;
}

class DialogPrivateMessages : public QDialog {
    Q_OBJECT
public:
    DialogPrivateMessages(QWidget *parent = 0);
	~DialogPrivateMessages();

protected:
    void changeEvent(QEvent *e);
	QList<WidgetPrivateMessage*> m_lChatWindows;

private:
    Ui::DialogPrivateMessages *ui;
	WidgetChatInput *widgetChatInput;
	CChatSession *chatSession;

public slots:
	void addChatSession(CChatSession *newChatSession);
};

#endif // DIALOGPRIVATEMESSAGES_H
