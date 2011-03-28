/*
** neighbours.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
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

#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QHash>
#include <QSet>

#include "types.h"

class CNeighbour;
class CNetworkConnection;
class CRateController;

class CNeighbours : public QObject
{
	Q_OBJECT

public:
	QMutex			 m_pSection;

protected:
	QList<CNeighbour*>		 m_lNodes;
	CRateController* 	m_pController;

	QHash<QHostAddress, CNeighbour*> m_lNodesByAddr;  // lookups by ip address
	QSet<CNeighbour*>			 m_lNodesByPtr;	// lookups by pointer

public:
	quint32	m_nHubsConnected;
	quint32 m_nLeavesConnected;
	bool	m_bActive;

public:
	CNeighbours(QObject* parent = 0);
	~CNeighbours();

	void Connect();
	void Disconnect();

	void Clear();

	void DisconnectYoungest(DiscoveryProtocol nProtocol, int nType = 0, bool bCore = false);

	void AddNode(CNeighbour* pNode);
	void RemoveNode(CNeighbour* pNode);
	CNeighbour* Find(QHostAddress& oAddress, DiscoveryProtocol nProtocol);

	bool NeighbourExists(const CNeighbour* pNode);

	void Maintain();
	bool NeedMore(G2NodeType nType);

	qint64 DownloadSpeed();
	qint64 UploadSpeed();

	inline QList<CNeighbour*>::iterator begin()
	{
		return m_lNodes.begin();
	}
	inline QList<CNeighbour*>::iterator end()
	{
		return m_lNodes.end();
	}

	inline int GetCount()
	{
		return m_lNodes.size();
	}
	inline CNeighbour* GetAt(int nIndex)
	{
		Q_ASSERT(nIndex >= 0 && nIndex < m_lNodes.size());

		return m_lNodes.at(nIndex);
	}

signals:
	void NeighbourAdded(CNeighbour*);
	void NeighbourRemoved(CNeighbour*);

public slots:
	CNeighbour* ConnectTo(CEndPoint& oAddress, DiscoveryProtocol nProtocol);
	CNeighbour* OnAccept(CNetworkConnection* pConn);


	friend class CNetwork;
	friend class CDatagrams;
	friend class CG2Node;
	friend class CManagedSearch;
};

extern CNeighbours Neighbours;
#endif // NEIGHBOURS_H
