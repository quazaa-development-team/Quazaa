#ifndef WIDGETPRIVATEMESSAGE_H
#define WIDGETPRIVATEMESSAGE_H

#include <QWidget>
#include "types.h"
#include "Chat/chatsession.h"

class QTextDocument;

namespace Ui {
	class WidgetPrivateMessage;
}

class WidgetPrivateMessage : public QWidget {
	Q_OBJECT
public:
	QUuid	m_oGUID;
	QString m_sNick;
	CChatSession* m_pSession;
public:
	WidgetPrivateMessage(QWidget *parent = 0);
	~WidgetPrivateMessage();
	QString inputMessage;

	void SendMessage(QTextDocument* pMessage, bool bAction = false);
	void SendMessage(QString sMessage, bool bAction = false);

protected:
	void changeEvent(QEvent *e);

private:
	Ui::WidgetPrivateMessage *ui;

signals:
	void SendMessageS(QTextDocument*, bool);
	void SendMessageS(QString, bool);

public slots:
	void OnIncomingMessage(QString sMessage, bool bAction = false);
	void OnSystemMessage(QString sMessage);
	void OnGUIDChanged(QUuid oGUID);
	void OnNickChanged(QString sNick);
};

#endif // WIDGETPRIVATEMESSAGE_H
