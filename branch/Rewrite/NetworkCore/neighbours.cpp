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
#include "neighbour.h"
#include "g2node.h"
#include "hostcache.h"
#include "geoiplist.h"
#include "thread.h"

#include "network.h"

#include <QMutexLocker>

CNeighbours Neighbours;

CNeighbours::CNeighbours(QObject* parent) :
	QObject(parent)
{
	m_pController = 0;
	m_nHubsConnected = m_nLeavesConnected = 0;
	m_bActive = false;
}
CNeighbours::~CNeighbours()
{
	if(m_bActive)
	{
		Disconnect();
	}
}

void CNeighbours::Connect()
{
	QMutexLocker l(&m_pSection);

	Q_ASSERT(m_pController == 0);

	m_pController = new CRateController(&m_pSection);
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

	foreach(CNeighbour * pNode, m_lNodes)
	{
		pNode->Close();
		RemoveNode(pNode);
		l.unlock();
		delete pNode;
		l.relock();
	}
}

CNeighbour* CNeighbours::ConnectTo(CEndPoint& oAddress, DiscoveryProtocol nProtocol)
{
	CNeighbour* pNode = 0;

	switch( nProtocol )
	{
	case dpGnutella2:
		pNode = new CG2Node(this);
		break;
	default:
		Q_ASSERT_X(0, "CNeighbours::ConnectTo", "Unknown protocol");
	}

	AddNode(pNode);
	pNode->ConnectTo(oAddress);
	return pNode;
}

CNeighbour* CNeighbours::OnAccept(CNetworkConnection* pConn)
{
	// TODO: Make new CNeighbour deriviate for handshaking with Gnutella clients

	systemLog.postLog("CNeighbours::OnAccept", LogSeverity::Debug);
	//qDebug() << "CNeighbours::OnAccept";

	if(!m_pSection.tryLock(50))
	{
		systemLog.postLog("Not accepting incoming connection. Neighbours overloaded", LogSeverity::Debug);
		//qDebug() << "Not accepting incoming connection. Neighbours overloaded";
		pConn->Close();
		pConn->deleteLater();
		return 0;
	}

	CG2Node* pNew = new CG2Node();
	pNew->AttachTo(pConn);
	AddNode(pNew);
	pNew->moveToThread(&NetworkThread);

	m_pSection.unlock();

	return pNew;
}

void CNeighbours::DisconnectYoungest(DiscoveryProtocol nProtocol, int nType, bool bCore)
{
	CNeighbour* pNode = 0;

	for(QList<CNeighbour*>::iterator i = m_lNodes.begin(); i != m_lNodes.end(); i++)
	{
		if((*i)->m_nState == nsConnected && (*i)->m_nProtocol == nProtocol)
		{
			if(nProtocol == dpGnutella2 && ((CG2Node*)(*i))->m_nType == nType)
			{
				if(!bCore && ((CG2Node*)(*i))->m_bG2Core)
				{
					continue;
				}

				if(pNode == 0)
				{
					pNode = (*i);
				}
				else
				{
					if((*i)->m_tConnected > pNode->m_tConnected)
					{
						pNode = (*i);
					}
				}
			}
		}
	}

	if(pNode)
	{
		pNode->Close();
	};
}

void CNeighbours::AddNode(CNeighbour* pNode)
{
	m_lNodes.append(pNode);
	m_lNodesByAddr.insert(pNode->m_oAddress, pNode);
	m_lNodesByPtr.insert(pNode);
	m_pController->AddSocket(pNode);
}

void CNeighbours::RemoveNode(CNeighbour* pNode)
{
	m_pController->RemoveSocket(pNode);
	m_lNodes.removeAll(pNode);
	m_lNodesByAddr.remove(pNode->m_oAddress);
	m_lNodesByPtr.remove(pNode);

	if( pNode->m_nProtocol == dpGnutella2 )
		Network.m_oRoutingTable.Remove(static_cast<CG2Node*>(pNode));
}

CNeighbour* CNeighbours::Find(QHostAddress& oAddress, DiscoveryProtocol nProtocol)
{
	if( m_lNodesByAddr.contains(oAddress) )
	{
		CNeighbour* pRet = m_lNodesByAddr.value(oAddress);

		return (pRet->m_nProtocol == nProtocol) ? pRet : 0;
	}
	return 0;
}

bool CNeighbours::NeighbourExists(const CNeighbour* pNode)
{
	return m_lNodesByPtr.contains(const_cast<CNeighbour * const&>(pNode));
}

void CNeighbours::Maintain()
{
	// TODO: Split that

	QMutexLocker l(&m_pSection);

	quint32 tNow = time(0);

	foreach(CNeighbour * pNode, m_lNodes)
	{
		pNode->OnTimer(tNow);
	}

	// G2
	quint32 nHubsG2 = 0, nLeavesG2 = 0, nUnknown = 0;
	quint32 nCoreHubsG2 = 0, nCoreLeavesG2 = 0;

	foreach(CNeighbour * pNode, m_lNodes)
	{
		if(pNode->m_nState == nsConnected)
		{
			switch(pNode->m_nProtocol)
			{
			case dpGnutella2:
				switch(((CG2Node*)pNode)->m_nType)
				{
					case G2_UNKNOWN:
						nUnknown++;
						break;
					case G2_HUB:
						nHubsG2++;
						if(((CG2Node*)pNode)->m_bG2Core)
						{
							nCoreHubsG2++;
						}
						break;
					case G2_LEAF:
						nLeavesG2++;
						if(((CG2Node*)pNode)->m_bG2Core)
						{
							nCoreLeavesG2++;
						}
				}
				break;
			default:
				break;
			}


		}
		else
		{
			nUnknown++;
		}


	}

	if(m_nHubsConnected != nHubsG2 || m_nLeavesConnected != nLeavesG2)
	{
		Network.m_bNeedUpdateLNI = true;
	}

	m_nHubsConnected = nHubsG2;
	m_nLeavesConnected = nLeavesG2;

	if(!Network.isHub())
	{
		if(nHubsG2 > quazaaSettings.Gnutella2.NumHubs)
		{
			int nToDisconnect = nHubsG2 - quazaaSettings.Gnutella2.NumHubs;

			for(; nToDisconnect; nToDisconnect--)
			{
				DisconnectYoungest(dpGnutella2, G2_HUB, (nCoreHubsG2 / nHubsG2) > 0.5);
			}
		}
		else if(nHubsG2 < quazaaSettings.Gnutella2.NumHubs)
		{
			QMutexLocker l(&HostCache.m_pSection);

			qint32 nAttempt = qint32((quazaaSettings.Gnutella2.NumHubs - nHubsG2) * quazaaSettings.Gnutella.ConnectFactor);
			nAttempt = qMin(nAttempt, 8) - nUnknown;

			quint32 tNow = time(0);
			bool bCountry = true;
			int  nCountry = 0;

			for(; nAttempt > 0; nAttempt--)
			{
				// nowe polaczenie
				CHostCacheHost* pHost = HostCache.GetConnectable(tNow, (bCountry ? (quazaaSettings.Connection.PreferredCountries.size() ? quazaaSettings.Connection.PreferredCountries.at(nCountry) : GeoIP.findCountryCode(Network.m_oAddress)) : "ZZ"));

				if(pHost)
				{
					ConnectTo(pHost->m_oAddress, dpGnutella2);
					pHost->m_tLastConnect = tNow;
				}
				else
				{
					if(!bCountry)
					{
						break;
					}
					else
					{
						if(quazaaSettings.Connection.PreferredCountries.size())
						{
							nCountry++;
							if(nCountry >= quazaaSettings.Connection.PreferredCountries.size())
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
		if(nHubsG2 > quazaaSettings.Gnutella2.NumPeers)
		{
			// rozlaczyc hub
			int nToDisconnect = nHubsG2 - quazaaSettings.Gnutella2.NumPeers;

			for(; nToDisconnect; nToDisconnect--)
			{
				DisconnectYoungest(dpGnutella2, G2_HUB, (nCoreHubsG2 / nHubsG2) > 0.5);
			}
		}
		else if(nHubsG2 < quazaaSettings.Gnutella2.NumPeers)
		{
			QMutexLocker l(&HostCache.m_pSection);

			qint32 nAttempt = qint32((quazaaSettings.Gnutella2.NumPeers - nHubsG2) * quazaaSettings.Gnutella.ConnectFactor);
			nAttempt = qMin(nAttempt, 8) - nUnknown;

			for(; nAttempt > 0; nAttempt--)
			{
				// nowe polaczenie
				CHostCacheHost* pHost = HostCache.GetConnectable(tNow);

				if(pHost)
				{
					ConnectTo(pHost->m_oAddress, dpGnutella2);
					pHost->m_tLastConnect = tNow;
				}
				else
				{
					break;
				}
			}
		}

		if(nLeavesG2 > quazaaSettings.Gnutella2.NumLeafs)
		{
			int nToDisconnect = nLeavesG2 - quazaaSettings.Gnutella2.NumLeafs;

			for(; nToDisconnect; nToDisconnect--)
			{
				DisconnectYoungest(dpGnutella2, G2_LEAF, (nCoreLeavesG2 / nLeavesG2) > 0.5);
			}
		}
	}
}

bool CNeighbours::NeedMore(G2NodeType nType)
{
	if(nType == G2_HUB)   // potrzeba hubow?
	{
		if(Network.isHub())   // jesli hub
		{
			return (m_nHubsConnected < quazaaSettings.Gnutella2.NumPeers);
		}
		else    // jesli leaf
		{
			return (m_nLeavesConnected < quazaaSettings.Gnutella2.NumHubs);
		}
	}
	else // potrzeba leaf?
	{
		if(Network.isHub())      // jesli hub
		{
			return (m_nLeavesConnected < quazaaSettings.Gnutella2.NumLeafs);
		}
	}

	return false;
}

qint64 CNeighbours::DownloadSpeed()
{
	//Q_ASSERT(m_pController);

	if(m_pController)
	{
		return m_pController->DownloadSpeed();
	}

	return 0;
}
qint64 CNeighbours::UploadSpeed()
{
	//Q_ASSERT(m_pController);

	if(m_pController)
	{
		return m_pController->UploadSpeed();
	}

	return 0;
}
