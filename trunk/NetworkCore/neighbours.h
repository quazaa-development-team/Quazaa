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

#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H

#include <QObject>
#include <QMutex>
#include <QList>

#include "types.h"

class CG2Node;
class CNetworkConnection;
class CRateController;

class CNeighbours : public QObject
{
    Q_OBJECT

public:
	QMutex			 m_pSection;

protected:
	QList<CG2Node*>	 m_lNodes;
	CRateController* m_pController;

public:
	quint32	m_nHubsConnected;
	quint32 m_nLeavesConnected;
	bool	m_bActive;

public:
	CNeighbours(QObject *parent = 0);

	void Connect();
	void Disconnect();

	void Clear();

	CG2Node* ConnectTo(IPv4_ENDPOINT& oAddress);
	CG2Node* ConnectTo(QHostAddress& oAddress, quint16 nPort);
	CG2Node* OnAccept(CNetworkConnection* pConn);

	void DisconnectYoungest(G2NodeType nType, bool bCore);

	void AddNode(CG2Node* pNode);
	void RemoveNode(CG2Node* pNode);
	CG2Node* Find(IPv4_ENDPOINT& oAddress);
	CG2Node* Find(quint32 nAddress);

	void Maintain();
	bool NeedMore(G2NodeType nType);

	qint64 DownloadSpeed();
	qint64 UploadSpeed();

	inline QList<CG2Node*>::iterator begin()
	{
		return m_lNodes.begin();
	}
	inline QList<CG2Node*>::iterator end()
	{
		return m_lNodes.end();
	}

	inline int GetCount()
	{
		return m_lNodes.size();
	}

signals:

public slots:


	friend class CNetwork;
	friend class CDatagrams;
	friend class CG2Node;
	friend class CManagedSearch;
};

extern CNeighbours Neighbours;
#endif // NEIGHBOURS_H
