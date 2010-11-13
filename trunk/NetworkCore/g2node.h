//
// g2node.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef G2NODE_H
#define G2NODE_H

#include "compressedconnection.h"
#include <QElapsedTimer>
#include <QQueue>

class G2Packet;
class CQueryHashTable;

enum G2NodeState { nsClosed, nsConnecting, nsHandshaking, nsConnected, nsClosing, nsError };

class CG2Node : public CCompressedConnection
{
	Q_OBJECT

public:
	quint32         m_tLastPacketIn;
	quint32         m_tLastPacketOut;
	quint32         m_nPacketsIn;
	quint32         m_nPacketsOut;
	QString         m_sUserAgent;
	bool            m_bG2Core;
	bool			m_bCachedKeys;
	G2NodeType      m_nType;
	G2NodeState     m_nState;
	quint32         m_tLastPingOut;
	quint32         m_nPingsWaiting;
	QElapsedTimer   m_tRTTTimer;
	qint64          m_tRTT;
	quint32         m_tLastQuery;

	quint16         m_nLeafCount;
	quint16         m_nLeafMax;

	bool            m_bAcceptDeflate;

	quint32         m_tKeyRequest;

	QQueue<G2Packet*>   m_lSendQueue;

	CQueryHashTable* 	m_pRemoteTable;
	CQueryHashTable* 	m_pLocalTable;

	bool			m_bSendQHT;
	quint32			m_tLastQHT;

	quint32			m_nCookie;

public:
	CG2Node(QObject* parent = 0);
	~CG2Node();

	virtual void ConnectTo(QHostAddress oAddress, quint16 nPort)
	{
		systemLog.postLog(LogSeverity::Information, "Initiating neighbour connection to %s...", qPrintable(oAddress.toString()));
		m_nState = nsConnecting;
		CNetworkConnection::ConnectTo(oAddress, nPort);
		SetupSlots();
	}
	virtual void ConnectTo(IPv4_ENDPOINT oAddress)
	{
		CNetworkConnection::ConnectTo(oAddress);
	}

	void AttachTo(CNetworkConnection* pOther)
	{
		m_nState = nsHandshaking;
		SetupSlots();
		CCompressedConnection::AttachTo(pOther);
	}

	void SendPacket(G2Packet* pPacket, bool bBuffered = false, bool bRelease = false);
	void FlushSendQueue(bool bFullFlush = false);

protected:
	void SetupSlots();

	void ParseOutgoingHandshake();
	void ParseIncomingHandshake();

	void Send_ConnectError(QString sReason);
	void Send_ConnectOK(bool bReply, bool bDeflated = false);
	void SendStartups();

public:
	void OnTimer(quint32 tNow);
signals:
	void NodeStateChanged();
public slots:
	void OnConnect();
	void OnDisconnect();
	void OnRead();
	void OnError(QAbstractSocket::SocketError e);
	void OnStateChange(QAbstractSocket::SocketState s);

public:
	void SendLNI();
protected:
	void OnPacket(G2Packet* pPacket);
	void OnPing(G2Packet* pPacket);
	void OnPong(G2Packet* pPacket);
	void OnLNI(G2Packet* pPacket);
	void OnKHL(G2Packet* pPacket);
	void OnQHT(G2Packet* pPacket);
	void OnQKR(G2Packet* pPacket);
	void OnQKA(G2Packet* pPacket);
	void OnQA(G2Packet* pPacket);
	void OnQH2(G2Packet* pPacket);
	void OnQuery(G2Packet* pPacket);


	friend class CNetwork;
};

#endif // G2NODE_H
