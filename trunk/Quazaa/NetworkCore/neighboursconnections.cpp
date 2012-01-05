/*
** $Id$
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

#include "neighboursconnections.h"
#include "ratecontroller.h"
#include "g2node.h"
#include "hostcache.h"
#include "quazaasettings.h"
#include "network.h"
#include "geoiplist.h"
#include "neighbours.h"
#include "thread.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

CNeighboursConnections::CNeighboursConnections(QObject* parent) :
	CNeighboursRouting(parent),
	m_pController(0),
	m_nHubsConnectedG2(0),
	m_nLeavesConnectedG2(0)
{
}
CNeighboursConnections::~CNeighboursConnections()
{
}

void CNeighboursConnections::Connect()
{
	QMutexLocker l(&m_pSection);

	Q_ASSERT(m_pController == 0);

	m_pController = new CRateController(&m_pSection);
	m_pController->SetDownloadLimit(quazaaSettings.Connection.InSpeed);
	m_pController->SetUploadLimit(quazaaSettings.Connection.OutSpeed);
	m_pController->moveToThread(&NetworkThread);

	m_nHubsConnectedG2 = m_nLeavesConnectedG2 = 0;

	CNeighboursRouting::Connect();
}
void CNeighboursConnections::Disconnect()
{
	QMutexLocker l(&m_pSection);

	while(!m_lNodes.isEmpty())
	{
		CNeighbour* pCurr = m_lNodes.takeFirst();
		delete pCurr;
	}

	delete m_pController;
	m_pController = 0;

	CNeighboursRouting::Disconnect();
}

void CNeighboursConnections::AddNode(CNeighbour* pNode)
{
	ASSUME_LOCK(m_pSection);

	m_pController->AddSocket(pNode);

	CNeighboursRouting::AddNode(pNode);
}

void CNeighboursConnections::RemoveNode(CNeighbour* pNode)
{
	ASSUME_LOCK(m_pSection);

	m_pController->RemoveSocket(pNode);

	CNeighboursRouting::RemoveNode(pNode);
}

CNeighbour* CNeighboursConnections::RandomNode(DiscoveryProtocol nProtocol, int nType, CNeighbour* pNodeExcept)
{
	QList<CNeighbour*> lNodeList;

	for(QList<CNeighbour*>::iterator i = m_lNodes.begin(); i != m_lNodes.end(); i++)
	{
		if((*i)->m_nState == nsConnected && (*i)->m_nProtocol == nProtocol)
		{
			if(( nProtocol == dpGnutella2 ) && ( ((CG2Node*)(*i))->m_nType == nType ) && ( (*i) != pNodeExcept ))
			{
				lNodeList.append((*i));
			}
		}
	}

	if(lNodeList.isEmpty())
	{
		return 0;
	}

	int nIndex = qrand() % lNodeList.count();

	return lNodeList.at(nIndex);
}

void CNeighboursConnections::DisconnectYoungest(DiscoveryProtocol nProtocol, int nType, bool bCore)
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
	}
}

void CNeighboursConnections::Maintain()
{
	ASSUME_LOCK(m_pSection);

	CNeighboursRouting::Maintain();

	quint32 nHubsG2 = 0, nLeavesG2 = 0;
	quint32 nCoreHubsG2 = 0, nCoreLeavesG2 = 0;
	quint32 nUnknown = 0;

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
					nUnknown++;
					break;
			}
		}
		else
		{
			nUnknown++;
		}
	}

	m_nHubsConnectedG2 = nHubsG2;
	m_nLeavesConnectedG2 = nLeavesG2;

	if(!Neighbours.IsG2Hub())
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

			QDateTime tNow = QDateTime::currentDateTimeUtc();
			bool bCountry = true;
			int  nCountry = 0;

			for(; nAttempt > 0; nAttempt--)
			{
				// nowe polaczenie
				CHostCacheHost* pHost = HostCache.GetConnectable(tNow, (bCountry ? (quazaaSettings.Connection.PreferredCountries.size() ? quazaaSettings.Connection.PreferredCountries.at(nCountry) : GeoIP.findCountryCode(Network.m_oAddress)) : "ZZ"));

				if(pHost)
				{
					if( !Neighbours.Find(pHost->m_oAddress) )
					{
						ConnectTo(pHost->m_oAddress, dpGnutella2);
						pHost->m_tLastConnect = tNow;
					}
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

			QDateTime tNow = QDateTime::currentDateTimeUtc();
			qint32 nAttempt = qint32((quazaaSettings.Gnutella2.NumPeers - nHubsG2) * quazaaSettings.Gnutella.ConnectFactor);
			nAttempt = qMin(nAttempt, 8) - nUnknown;

			for(; nAttempt > 0; nAttempt--)
			{
				// nowe polaczenie
				CHostCacheHost* pHost = HostCache.GetConnectable(tNow);

				if(pHost)
				{
					if( !Neighbours.Find(pHost->m_oAddress) )
					{
						ConnectTo(pHost->m_oAddress, dpGnutella2);
						pHost->m_tLastConnect = tNow;
					}
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

quint32 CNeighboursConnections::DownloadSpeed()
{
	return m_pController ? m_pController->DownloadSpeed() : 0;
}

quint32 CNeighboursConnections::UploadSpeed()
{
	return m_pController ? m_pController->UploadSpeed() : 0;
}

CNeighbour* CNeighboursConnections::OnAccept(CNetworkConnection* pConn)
{
	// TODO: Make new CNeighbour deriviate for handshaking with Gnutella clients

	Q_ASSERT(thread() == &NetworkThread);

	systemLog.postLog(LogCategory::Network, LogSeverity::Debug, "CNeighbours::OnAccept");
	//qDebug() << "CNeighbours::OnAccept";

	if(!m_bActive)
	{
		pConn->Close();
		return 0;
	}

	if(!m_pSection.tryLock(50))
	{
		systemLog.postLog(LogCategory::Network, LogSeverity::Debug, "Not accepting incoming connection. Neighbours overloaded");
		pConn->Close();
		return 0;
	}

	CG2Node* pNew = new CG2Node();
	pNew->AttachTo(pConn);
	AddNode(pNew);
	pNew->moveToThread(&NetworkThread);

	m_pSection.unlock();

	return pNew;
}

CNeighbour* CNeighboursConnections::ConnectTo(CEndPoint& oAddress, DiscoveryProtocol nProtocol)
{
	ASSUME_LOCK(m_pSection);

	CNeighbour* pNode = 0;

	switch(nProtocol)
	{
		case dpGnutella2:
			pNode = new CG2Node();
			break;
		default:
			Q_ASSERT_X(0, "CNeighbours::ConnectTo", "Unknown protocol");
	}

	AddNode(pNode);
	pNode->ConnectTo(oAddress);
	pNode->moveToThread(&NetworkThread);
	return pNode;
}

