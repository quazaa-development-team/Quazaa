#ifndef CHATSESSION_H
#define CHATSESSION_H

#include "networkconnection.h"
#include <QUuid>

class DialogPrivateMessage;
class G2Packet;

enum ChatState
{
	csNull,
	csConnecting,
	csHandshaking,
	csConnected,
	csClosing,
	csClosed
};

class CChatSession : public CNetworkConnection
{
	Q_OBJECT
protected:
	DialogPrivateMessage* m_pDialog;
	QUuid				  m_oGUID;
	bool				  m_bShareaza;
public:
	DiscoveryProtocol m_nProtocol;
	ChatState		  m_nState;
	QString			  m_sNick;
public:
	CChatSession(DialogPrivateMessage* pDialog = 0, QUuid* oGUID = 0, QObject *parent = 0);
	~CChatSession();
	void ConnectTo(CEndPoint oAddress);

	void OnTimer(quint32 tNow);

protected:
	void SetupDialog();
	void ParseOutgoingHandshakeG2();

	void Send_ChatOKG2(bool bReply);
	void Send_ChatErrorG2(QString sReason);
	void SendStartupsG2();
	void SendPacketG2(G2Packet* pPacket, bool bRelease = true);
	void OnPacket(G2Packet* pPacket);
	void OnUPROC(G2Packet* pPacket);
	void OnUPROD(G2Packet* pPacket);
	void OnCHATANS(G2Packet* pPacket);

signals:

public slots:
	void OnConnect();
	void OnDisconnect();
	void OnRead();
	void OnError(QAbstractSocket::SocketError e);
	void OnStateChange(QAbstractSocket::SocketState s);

};

#endif // CHATSESSION_H
