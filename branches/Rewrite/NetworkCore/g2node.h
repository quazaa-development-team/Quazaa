/*
** g2node.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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

protected:
	qint64 writeToNetwork(qint64 nBytes);
	bool HasData()
	{
		if( !m_lSendQueue.isEmpty() )
			return true;

		return CNeighbour::HasData();
	}

	friend class CNetwork;
};

#endif // G2NODE_H
