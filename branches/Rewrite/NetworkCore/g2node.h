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

#include "neighbour.h"
#include <QElapsedTimer>
#include <QQueue>
#include <QHash>

class G2Packet;
class CQueryHashTable;

class CG2Node : public CNeighbour
{
	Q_OBJECT

public:
	bool            m_bG2Core;
	bool			m_bCachedKeys;
	G2NodeType      m_nType;

	quint16         m_nLeafCount;
	quint16         m_nLeafMax;

	bool            m_bAcceptDeflate;

	quint32         m_tKeyRequest;

	QQueue<G2Packet*>   m_lSendQueue;

	CQueryHashTable* 	m_pRemoteTable;
	CQueryHashTable* 	m_pLocalTable;

	bool			m_bSendQHT;
	quint32			m_tLastQHT;

	QHash<quint32,quint32> m_lRABan; // list of banned return addresses

public:
	CG2Node(QObject* parent = 0);
	~CG2Node();

	void AttachTo(CNetworkConnection* pOther)
	{
		CNeighbour::AttachTo(pOther);
	}

	void SendPacket(G2Packet* pPacket, bool bBuffered = false, bool bRelease = false);
	void FlushSendQueue(bool bFullFlush = false);

protected:

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
