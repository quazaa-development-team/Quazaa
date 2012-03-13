/*
** $Id: managedsearch.cpp 938 2012-02-23 05:08:24Z smokexyz $
**
** Copyright © Quazaa Development Team, 2009-2012.
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

#include "managedsearch.h"
#include "network.h"
#include "neighbours.h"
#include "g2node.h"
#include "g2packet.h"
#include "query.h"
#include "hostcache.h"
#include "datagrams.h"
#include "searchmanager.h"
#include "systemlog.h"
#include "Hashes/hash.h"
#include "queryhit.h"

#include <QMutexLocker>

#include "quazaasettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

CManagedSearch::CManagedSearch(CQuery* pQuery, QObject* parent) :
	QObject(parent)
{
	m_bActive = false;
	m_bPaused = false;
	m_pQuery = pQuery;
	m_tStarted = QDateTime::currentDateTimeUtc();
	m_tCleanHostsNext = QDateTime::currentDateTimeUtc().addSecs(quazaaSettings.Gnutella2.QueryHostThrottle);

	m_oGUID = QUuid::createUuid();
	pQuery->SetGUID(m_oGUID);

	m_nHubs = m_nLeaves = m_nHits = 0;

	m_bCanRequestKey = true;
	m_nQueryCount = 0;
	m_nCookie = 0;
	m_nCachedHits = 0;
	m_pCachedHit = 0;
	m_nQueryHitLimit = quazaaSettings.Gnutella.MaxResults;
}

CManagedSearch::~CManagedSearch()
{
	if(m_bActive || m_bPaused)
	{
		Stop();
	}

	if(m_pQuery)
	{
		delete m_pQuery;
	}

	if(m_pCachedHit)
	{
		delete m_pCachedHit;
		m_nCachedHits = 0;
		m_pCachedHit = 0;
	}
}

void CManagedSearch::Start()
{
	if(!m_bPaused)
	{
		SearchManager.Add(this);
	}

	m_bActive = true;
	m_bPaused = false;

	m_nQueryCount = 0;
	m_nQueryHitLimit = m_nHits + quazaaSettings.Gnutella.MaxResults;

	emit StateChanged();
}
void CManagedSearch::Pause()
{
	m_bPaused = true;
	m_bActive = false;

	emit StateChanged();
}
void CManagedSearch::Stop()
{
	m_bActive = false;
	m_bPaused = false;

	SearchManager.Remove(this);

	emit StateChanged();
}

void CManagedSearch::Execute(QDateTime& tNow, quint32* pnMaxPackets)
{
	if(!m_bActive)
	{
		return;
	}

	if(m_nQueryCount > quazaaSettings.Gnutella2.QueryLimit)
	{
		systemLog.postLog(LogSeverity::Debug, "Pausing search: query limit reached");
		Pause();
		return;
	}

	quint32 nMaxPackets = *pnMaxPackets;

	if(m_tStarted.secsTo(tNow) < 30)
	{
		nMaxPackets = qMin(quint32(2), nMaxPackets);
	}

	*pnMaxPackets -= nMaxPackets;

	SearchNeighbours(tNow);
	SearchG2(tNow, &nMaxPackets);

	*pnMaxPackets += nMaxPackets;

	m_bCanRequestKey = !m_bCanRequestKey;

	if( tNow > m_tCleanHostsNext )
	{
#ifdef _DEBUG
		quint32 nRemoved = 0;
		quint32 nOldSize = m_lSearchedNodes.size();
#endif

		for(QHash<QHostAddress, QDateTime>::iterator itHost = m_lSearchedNodes.begin(); itHost != m_lSearchedNodes.end(); )
		{
			if( (*itHost).secsTo(tNow) > quazaaSettings.Gnutella2.RequeryDelay )
			{
#ifdef _DEBUG
				++nRemoved;
#endif
				itHost = m_lSearchedNodes.erase(itHost);
			}
			else
			{
				++itHost;
			}
		}

		m_tCleanHostsNext = tNow.addSecs(quazaaSettings.Gnutella2.QueryHostThrottle);

#ifdef _DEBUG
		systemLog.postLog(LogSeverity::Debug, QString("Clearing don't-try list for search %1, old size: %2, new size: %3, items removed: %4").arg(m_oGUID.toString()).arg(nOldSize).arg(m_lSearchedNodes.size()).arg(nRemoved));
#endif
	}
}

void CManagedSearch::SearchNeighbours(QDateTime& tNow)
{
	QMutexLocker l(&Neighbours.m_pSection);

	quint32 tNowT = tNow.toTime_t();

	for(QList<CNeighbour*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode)
	{
		if((*itNode)->m_nProtocol != dpGnutella2)
		{
			continue;
		}

		CG2Node* pNode = (CG2Node*)(*itNode);

		if(pNode->m_nState == nsConnected
				&& tNowT - pNode->m_tConnected > 15
				&& (tNowT - pNode->m_tLastQuery > quazaaSettings.Gnutella2.QueryHostThrottle
					&& !m_lSearchedNodes.contains(pNode->m_oAddress))
		  )
		{
			G2Packet* pQuery = m_pQuery->ToG2Packet(Network.IsFirewalled() ? 0 : &Network.m_oAddress);
			if(pQuery)
			{
				m_lSearchedNodes[pNode->m_oAddress] = tNow;
				pNode->SendPacket(pQuery, true, true);
				pNode->m_tLastQuery = tNowT;
			}
		}
	}
}

void CManagedSearch::SearchG2(QDateTime& tNow, quint32* pnMaxPackets)
{
	QMutexLocker oHostCacheLock(&HostCache.m_pSection);

	CG2Node* pLastNeighbour = 0;

	for(CHostCacheIterator itHost = HostCache.m_lHosts.begin(); itHost != HostCache.m_lHosts.end(); ++itHost)
	{
		CHostCacheHost* pHost = *itHost;

		if(pHost->m_tTimestamp.secsTo(tNow) > quazaaSettings.Gnutella2.HostCurrent)
			break; // timestamp sorted cache

		if(!pHost->CanQuery(tNow))
			continue;

		// don't query already queried hosts
		// this applies to query key requests as well,
		// so we don't waste our resources to request a key that will be useless in this search anyway
		if(m_lSearchedNodes.contains(pHost->m_oAddress))
		{
			if(m_lSearchedNodes[pHost->m_oAddress].secsTo(tNow) < int(quazaaSettings.Gnutella2.RequeryDelay))
			{
				continue;
			}
		}

		Neighbours.m_pSection.lock();
		if(Neighbours.Find(pHost->m_oAddress))
		{
			// don't udp to neighbours
			Neighbours.m_pSection.unlock();
			continue;
		}
		Neighbours.m_pSection.unlock();

		CEndPoint pReceiver;

		bool bRefreshKey = false;

		if(pHost->m_nQueryKey == 0)
		{
			// we don't have a key
		}
		else
		{
			if(pHost->m_nKeyTime.secsTo(tNow) > quazaaSettings.Gnutella2.QueryKeyTime)
			{
				// query key expired
				pHost->m_nQueryKey = 0;
				bRefreshKey = true;
			}
			else if(!Network.IsFirewalled())
			{
				if(pHost->m_nKeyHost == Network.m_oAddress)
				{
					pReceiver = Network.m_oAddress;
				}
				else
				{
					pHost->m_nQueryKey = 0;
				}
			}
			else
			{
				// we are firewalled, so key must be for one of our connected neighbours
				Neighbours.m_pSection.lock();

				CNeighbour* pNode = Neighbours.Find(pHost->m_nKeyHost, dpGnutella2);

				if( pNode && static_cast<CG2Node*>(pNode)->m_nState == nsConnected )
				{
					pReceiver = pNode->m_oAddress;
				}
				else
				{
					pHost->m_nQueryKey = 0;
				}

				Neighbours.m_pSection.unlock();
			}
		}

		// if we still have a key, send the query
		if(pHost->m_nQueryKey != 0)
		{
			Q_ASSERT(!pReceiver.isNull());

			m_lSearchedNodes[pHost->m_oAddress] = tNow;

			pHost->m_tLastQuery = tNow;
			if(pHost->m_tAck.isNull())
			{
				pHost->m_tAck = tNow;
			}

			G2Packet* pQuery = m_pQuery->ToG2Packet(&pReceiver, pHost->m_nQueryKey);

			if(pQuery)
			{
				systemLog.postLog(LogSeverity::Debug, QString("Querying %1").arg(pHost->m_oAddress.toString()));
				*pnMaxPackets -= 1;
				Datagrams.SendPacket(pHost->m_oAddress, pQuery, true);
				pQuery->Release();
				m_nQueryCount++;
			}
		}
		else if(m_bCanRequestKey
				&& pHost->m_nKeyTime.secsTo(tNow) > quazaaSettings.Gnutella2.QueryHostThrottle)
		{
			// we can request a query key now
			// UDP QKR is sent without ACK request, so this may be a retry

			bool bKeyRequested = false;

			if(!Network.IsFirewalled())
			{
				// request a key for our address
				G2Packet* pQKR = G2Packet::New("QKR", false);
				pQKR->WritePacket("RNA", (Network.m_oAddress.protocol() == 0 ? 6 : 18))->WriteHostAddress(&Network.m_oAddress);
				Datagrams.SendPacket(pHost->m_oAddress, pQKR, false);
				pQKR->Release();
				systemLog.postLog(LogSeverity::Debug, QString("Requesting query key from %1").arg(pHost->m_oAddress.toString()));

				bKeyRequested = true;
			}
			else
			{
				Neighbours.m_pSection.lock();

				CG2Node* pHub = 0;

				// Find best hub for routing
				bool bCheckLast = Neighbours.m_nHubsConnectedG2 > 2;
				for(QList<CNeighbour*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode)
				{
					if((*itNode)->m_nProtocol != dpGnutella2 || (*itNode)->m_nState != nsConnected)
					{
						continue;
					}

					CG2Node* pNode = (CG2Node*)(*itNode);

					// Must be a hub that already acked our query
					if(pNode->m_nType == G2_HUB &&
							m_lSearchedNodes.contains(pNode->m_oAddress))
					{
						if((bCheckLast && pNode == pLastNeighbour))
						{
							continue;
						}

						if(pHub)
						{
							if(pNode->m_nPingsWaiting == 0
									&& pNode->m_tRTT < pHub->m_tRTT
									&& pNode->m_tRTT < 10000)
							{
								pHub = pNode;
							}
						}
						else if(pNode->m_nPingsWaiting == 0)
						{
							pHub = pNode;
						}
					}
				}

				if(pHub)
				{
					pLastNeighbour = pHub;
					if(pHub->m_tKeyRequest == 0)
					{
						pHub->m_tKeyRequest = tNow.toTime_t();
					}

					if(pHub->m_bCachedKeys)
					{
						G2Packet* pQKR = G2Packet::New("QKR", true);
						pQKR->WritePacket("QNA", (pHost->m_oAddress.protocol() == 0 ? 6 : 18))->WriteHostAddress(&pHost->m_oAddress);
						if( bRefreshKey )
							pQKR->WritePacket("REF", 0);
						systemLog.postLog(LogSeverity::Debug, QString("Requesting query key from %1 through %2").arg(pHost->m_oAddress.toString()).arg(pHub->m_oAddress.toString()));
						pHub->SendPacket(pQKR, true, true);
					}
					else
					{
						G2Packet* pQKR = G2Packet::New("QKR", true);
						pQKR->WritePacket("RNA", (pHub->m_oAddress.protocol() == 0 ? 6 : 18))->WriteHostAddress(&pHub->m_oAddress);
						Datagrams.SendPacket(pHost->m_oAddress, pQKR, false);
						pQKR->Release();
						systemLog.postLog(LogSeverity::Debug, QString("Requesting query key from %1 for %2").arg(pHost->m_oAddress.toString()).arg(pHub->m_oAddress.toString()));
					}

					bKeyRequested = true;
				}

				Neighbours.m_pSection.unlock();
			}

			if( bKeyRequested )
			{
				*pnMaxPackets -= 1;

				if(pHost->m_tAck.isNull())
				{
					pHost->m_tAck = tNow;
				}
				pHost->m_nKeyTime = tNow;
				pHost->m_nQueryKey = 0;
			}
		}

		if(*pnMaxPackets == 0)
		{
			break;
		}
	}
}

void CManagedSearch::OnHostAcknowledge(QHostAddress nHost, QDateTime& tNow)
{
	m_lSearchedNodes[nHost] = tNow;
}

void CManagedSearch::OnQueryHit(CQueryHit* pHits)
{
	CQueryHit* pHit = pHits;
	CQueryHit* pLast = 0;

	while(pHit != 0)
	{
		m_nHits++;
		m_nCachedHits++;
		pLast = pHit;
		pHit = pHit->m_pNext;
	}

	//emit OnHit(pHits);

	if(m_pCachedHit)
	{
		pLast->m_pNext = m_pCachedHit;
	}

	m_pCachedHit = pHits;

	if(m_nCachedHits > 100)
	{
		SendHits();
	}

	if(m_nHits > m_nQueryHitLimit && !m_bPaused)
	{
		systemLog.postLog(LogSeverity::Debug, tr("Pausing search: query hit limit reached"));
		Pause();
	}
}
void CManagedSearch::SendHits()
{
	if(!m_pCachedHit)
	{
		return;
	}

	systemLog.postLog(LogSeverity::Debug, QString("Sending hits... %1").arg(m_nCachedHits));
	//qDebug() << "Sending hits..." << m_nCachedHits;
	QueryHitSharedPtr pSHits(m_pCachedHit);
	emit OnHit(pSHits);
	m_pCachedHit = 0;
	m_nCachedHits = 0;
}

