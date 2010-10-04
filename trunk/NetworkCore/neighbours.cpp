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

#include "neighbours.h"

#include "quazaasettings.h"

#include "ratecontroller.h"
#include "g2node.h"
#include "hostcache.h"
#include "geoiplist.h"
#include "thread.h"

#include "network.h"

#include <QMutexLocker>

CNeighbours Neighbours;

CNeighbours::CNeighbours(QObject *parent) :
    QObject(parent)
{
	m_pController = 0;
	m_nHubsConnected = m_nLeavesConnected = 0;
	m_bActive = false;
}
CNeighbours::~CNeighbours()
{
	if( m_bActive )
		Disconnect();
}

void CNeighbours::Connect()
{
	QMutexLocker l(&m_pSection);

	Q_ASSERT(m_pController == 0);

	m_pController = new CRateController();
	m_pController->SetDownloadLimit(quazaaSettings.Connection.InSpeed);
	m_pController->SetUploadLimit(quazaaSettings.Connection.OutSpeed);

	m_nHubsConnected = m_nLeavesConnected = 0;

	moveToThread(&NetworkThread);
	m_pController->moveToThread(&NetworkThread);

	m_bActive = true;
}
void CNeighbours::Disconnect()
{
	Q_ASSERT(m_pController);

	Clear();

	QMutexLocker l(&m_pSection);

	delete m_pController;
	m_pController = 0;

	m_bActive = false;

}

void CNeighbours::Clear()
{
	QMutexLocker l(&m_pSection);

	foreach( CG2Node* pNode, m_lNodes )
	{
		pNode->Close();
		RemoveNode(pNode);
		l.unlock();
		delete pNode;
		l.relock();
	}
}

CG2Node* CNeighbours::ConnectTo(IPv4_ENDPOINT& oAddress)
{
	CG2Node* pNode = new CG2Node(this);
	AddNode(pNode);
	pNode->ConnectTo(oAddress);
	return pNode;
}

CG2Node* CNeighbours::ConnectTo(QHostAddress& oAddress, quint16 nPort)
{
	CG2Node* pNode = new CG2Node(this);
	AddNode(pNode);
	pNode->ConnectTo(oAddress, nPort);
	return pNode;
}

CG2Node* CNeighbours::OnAccept(CNetworkConnection* pConn)
{
	if( !m_pSection.tryLock(50) )
	{
		qDebug() << "Not accepting incoming connection. Neighbours overloaded";
		pConn->Close();
		pConn->deleteLater();
		return 0;
	}

	CG2Node* pNew = new CG2Node();
	pNew->AttachTo(pConn);
	pNew->moveToThread(&NetworkThread);
	AddNode(pNew);

	m_pSection.unlock();

	return pNew;
}

void CNeighbours::DisconnectYoungest(G2NodeType nType, bool bCore)
{
	CG2Node* pNode = 0;

	for( QList<CG2Node*>::iterator i = m_lNodes.begin(); i != m_lNodes.end(); i++ )
	{
		if( (*i)->m_nState == nsConnected )
		{
			if( (*i)->m_nType == nType )
			{
				if( !bCore && (*i)->m_bG2Core )
					continue;

				if( pNode == 0 )
				{
					pNode = (*i);
				}
				else
				{
					if( (*i)->m_tConnected > pNode->m_tConnected )
						pNode = (*i);
				}
			}
		}
	}

	if( pNode )
		pNode->Close();;
}

void CNeighbours::AddNode(CG2Node* pNode)
{
	m_lNodes.append(pNode);
	m_lNodesByAddr.insert(pNode->m_oAddress.ip, pNode);
	m_lNodesByPtr.insert(pNode);
	m_pController->AddSocket(pNode);
}

void CNeighbours::RemoveNode(CG2Node* pNode)
{
	m_pController->RemoveSocket(pNode);
	m_lNodes.removeAll(pNode);
	m_lNodesByAddr.remove(pNode->m_oAddress.ip);
	m_lNodesByPtr.remove(pNode);

	Network.m_oRoutingTable.Remove(pNode);
}

CG2Node* CNeighbours::Find(IPv4_ENDPOINT& oAddress)
{
	return Find(oAddress.ip);
}

CG2Node* CNeighbours::Find(quint32 nAddress)
{
	if( !m_lNodesByAddr.contains(nAddress) )
		return 0;
	else
		return m_lNodesByAddr.value(nAddress);
}

bool CNeighbours::NeighbourExists(const CG2Node *pNode)
{
	return m_lNodesByPtr.contains(const_cast<CG2Node* const &>(pNode));
}

void CNeighbours::Maintain()
{
	QMutexLocker l(&m_pSection);

	quint32 tNow = time(0);

	foreach( CG2Node* pNode, m_lNodes )
	{
		pNode->OnTimer(tNow);
	}

	quint32 nHubs = 0, nLeaves = 0, nUnknown = 0;
	quint32 nCoreHubs = 0, nCoreLeaves = 0;

	foreach( CG2Node* pNode, m_lNodes )
	{
		if( pNode->m_nState == nsConnected )
		{
			switch( pNode->m_nType )
			{
			case G2_UNKNOWN:
				nUnknown++;
				break;
			case G2_HUB:
				nHubs++;
				if( pNode->m_bG2Core )
					nCoreHubs++;
				break;
			case G2_LEAF:
				nLeaves++;
				if( pNode->m_bG2Core )
					nCoreLeaves++;
			}
		}
		else
		{
			nUnknown++;
		}


	}

	if( m_nHubsConnected != nHubs || m_nLeavesConnected != nLeaves )
		Network.m_bNeedUpdateLNI = true;

	m_nHubsConnected = nHubs;
	m_nLeavesConnected = nLeaves;

	if( !Network.isHub() )
	{
		if( nHubs > quazaaSettings.Gnutella2.NumHubs )
		{
			int nToDisconnect = nHubs - quazaaSettings.Gnutella2.NumHubs;

			for( ; nToDisconnect; nToDisconnect-- )
			{
				DisconnectYoungest(G2_HUB, (nCoreHubs / nHubs) > 0.5);
			}
		}
		else if( nHubs < quazaaSettings.Gnutella2.NumHubs )
		{
			QMutexLocker l(&HostCache.m_pSection);

			qint32 nAttempt = qint32((quazaaSettings.Gnutella2.NumHubs - nHubs) * quazaaSettings.Gnutella.ConnectFactor );
			nAttempt = qMin(nAttempt, 8) - nUnknown;

			quint32 tNow = time(0);
			bool bCountry = true;
			int  nCountry = 0;

			for( ; nAttempt > 0; nAttempt-- )
			{
				// nowe polaczenie
				CHostCacheHost* pHost = HostCache.GetConnectable(tNow, (bCountry ? (quazaaSettings.Connection.PreferredCountries.size() ? quazaaSettings.Connection.PreferredCountries.at(nCountry) : GeoIP.findCountryCode(Network.m_oAddress)) : "ZZ"));

				if( pHost )
				{
					ConnectTo(pHost->m_oAddress);
					pHost->m_tLastConnect = tNow;
				}
				else
				{
					if( !bCountry )
					{
						break;
					}
					else
					{
						if( quazaaSettings.Connection.PreferredCountries.size() )
						{
							nCountry++;
							if( nCountry >= quazaaSettings.Connection.PreferredCountries.size() )
							{
								bCountry = false;
							}
							nAttempt++;
							continue;
						}
						bCountry = false;
						nAttempt++;
					}
				}
			}

		}
	}
	else
	{
		if( nHubs > quazaaSettings.Gnutella2.NumPeers )
		{
			// rozlaczyc hub
			int nToDisconnect = nHubs - quazaaSettings.Gnutella2.NumPeers;

			for( ; nToDisconnect; nToDisconnect-- )
			{
				DisconnectYoungest(G2_HUB, (nCoreHubs / nHubs) > 0.5);
			}
		}
		else if( nHubs < quazaaSettings.Gnutella2.NumPeers )
		{
			QMutexLocker l(&HostCache.m_pSection);

			qint32 nAttempt = qint32((quazaaSettings.Gnutella2.NumPeers - nHubs) * quazaaSettings.Gnutella.ConnectFactor );
			nAttempt = qMin(nAttempt, 8) - nUnknown;

			for( ; nAttempt > 0; nAttempt-- )
			{
				// nowe polaczenie
				CHostCacheHost* pHost = HostCache.GetConnectable(tNow);

				if( pHost )
				{
					ConnectTo(pHost->m_oAddress);
					pHost->m_tLastConnect = tNow;
				}
				else
					break;
			}
		}

		if( nLeaves > quazaaSettings.Gnutella2.NumLeafs )
		{
			int nToDisconnect = nLeaves - quazaaSettings.Gnutella2.NumLeafs;

			for( ; nToDisconnect; nToDisconnect-- )
			{
				DisconnectYoungest(G2_LEAF, (nCoreLeaves / nLeaves) > 0.5);
			}
		}
	}
}

bool CNeighbours::NeedMore(G2NodeType nType)
{
	if( nType == G2_HUB ) // potrzeba hubow?
	{
		if( Network.isHub() ) // jesli hub
			return ( m_nHubsConnected < quazaaSettings.Gnutella2.NumPeers );
		else    // jesli leaf
			return ( m_nLeavesConnected < quazaaSettings.Gnutella2.NumHubs );
	}
	else // potrzeba leaf?
	{
		if( Network.isHub() )    // jesli hub
			return ( m_nLeavesConnected < quazaaSettings.Gnutella2.NumLeafs );
	}

	return false;
}

qint64 CNeighbours::DownloadSpeed()
{
	//Q_ASSERT(m_pController);

	if( m_pController )
		return m_pController->DownloadSpeed();

	return 0;
}
qint64 CNeighbours::UploadSpeed()
{
	//Q_ASSERT(m_pController);

	if( m_pController )
		return m_pController->UploadSpeed();

	return 0;
}
