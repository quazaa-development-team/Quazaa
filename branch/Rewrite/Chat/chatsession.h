#ifndef CHATSESSION_H
#define CHATSESSION_H

#include "networkconnection.h"
#include <QUuid>

class WidgetPrivateMessage;
class G2Packet;
class QTextDocument;

enum ChatState
{
	csNull,
	csConnecting,
	csHandshaking,
	csConnected,
	csActive,
	csClosing,
	csClosed
};

class CChatSession : public CNetworkConnection
{
	Q_OBJECT
protected:
	WidgetPrivateMessage* m_pWidget;
	bool				  m_bShareaza;
public:
	QUuid			  m_oGUID;
	DiscoveryProtocol m_nProtocol;
	ChatState		  m_nState;
	QString			  m_sNick;
public:
	CChatSession(QObject *parent = 0);
	virtual ~CChatSession();

	virtual void Connect();

	virtual void OnTimer(quint32 tNow);

	void SetupWidget(WidgetPrivateMessage* pWg);

signals:
	void incomingMessage(QString, bool = false);
	void systemMessage(QString);
	void nickChanged(QString);
	void guidChanged(QUuid);

public slots:
	void OnConnect();
	void OnDisconnect();
	void OnRead();
	void OnError(QAbstractSocket::SocketError e);
	void OnStateChange(QAbstractSocket::SocketState s);

	virtual void SendMessage(QString sMessage, bool bAction = false) = 0;
	virtual void SendMessage(QTextDocument* pMessage, bool bAction = false) = 0;

};

#endif // CHATSESSION_H
