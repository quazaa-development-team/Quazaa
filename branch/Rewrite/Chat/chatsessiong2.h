#ifndef CHATSESSIONG2_H
#define CHATSESSIONG2_H

#include "Chat/chatsession.h"

class G2Packet;

class CChatSessionG2 : public CChatSession
{
	Q_OBJECT
protected:
	CEndPoint m_oRemoteHost;
public:
	CChatSessionG2(CEndPoint oRemoteHost, QObject *parent = 0);

	void Connect();

protected:
	void ParseOutgoingHandshake();

	void Send_ChatOK(bool bReply);
	void Send_ChatError(QString sReason);
	void SendStartups();
	void SendPacket(G2Packet* pPacket, bool bRelease = true);
	void OnPacket(G2Packet* pPacket);
	void OnUPROC(G2Packet* pPacket);
	void OnUPROD(G2Packet* pPacket);
	void OnCHATANS(G2Packet* pPacket);
	void OnCMSG(G2Packet* pPacket);

signals:

public slots:
	void OnConnect();
	void OnDisconnect();
	void OnRead();

	void SendMessage(QString sMessage, bool bAction = false);
	void SendMessage(QTextDocument* pMessage, bool bAction = false);

};

#endif // CHATSESSIONG2_H
