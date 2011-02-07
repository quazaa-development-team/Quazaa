#ifndef WIDGETPRIVATEMESSAGE_H
#define WIDGETPRIVATEMESSAGE_H

#include <QWidget>
#include <QUrl>
#include "types.h"
#include "chatsession.h"

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

private slots:
	void on_textEdit_anchorClicked(QUrl link);
};

#endif // WIDGETPRIVATEMESSAGE_H
