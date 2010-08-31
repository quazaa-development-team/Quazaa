//
// searchmanager.cpp
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

#include "searchmanager.h"
#include "managedsearch.h"
#include "queryhit.h"
#include "g2packet.h"
#include <QMutexLocker>
#include "hostcache.h"
#include "network.h"
#include <QMetaType>

#include "quazaasettings.h"

CSearchManager SearchManager;

const quint32 PacketsPerSec = 8;

CSearchManager::CSearchManager(QObject *parent) :
    QObject(parent)
{
    m_nPruneCounter = 0;
	m_nCookie = 0;

    qRegisterMetaType<QueryHitSharedPtr>("QueryHitSharedPtr");
}

void CSearchManager::Add(CManagedSearch *pSearch)
{
    QMutexLocker l(&m_pSection);

	Q_ASSERT(!m_lSearches.contains(pSearch->m_oGUID));
	m_lSearches.insert(pSearch->m_oGUID, pSearch);
    if( pSearch->thread() != SearchManager.thread() )
        pSearch->moveToThread(SearchManager.thread());
}

void CSearchManager::Remove(CManagedSearch *pSearch)
{
    QMutexLocker l(&m_pSection);

	Q_ASSERT(m_lSearches.contains(pSearch->m_oGUID));
	m_lSearches.remove(pSearch->m_oGUID);
}

CManagedSearch* CSearchManager::Find(QUuid &oGUID)
{
	return m_lSearches.value(oGUID, 0);
}

void CSearchManager::OnTimer()
{
    QMutexLocker l(&m_pSection);

    quint32 nSearches = m_lSearches.size();
    quint32 nPacketsLeft = PacketsPerSec;

    if( nSearches == 0 )
        return;

    m_nPruneCounter++;
    if( m_nPruneCounter % 30 == 0 )
    {
        HostCache.PruneByQueryAck();
    }

    quint32 nPacketsPerSearch = qMax<quint32>(1, nPacketsLeft / nSearches);
    quint32 tNow = time(0);

	quint32 nPacket = 0;
	bool bUpdate = true;

	for( QHash<QUuid, CManagedSearch*>::iterator itSearch = m_lSearches.begin(); itSearch != m_lSearches.end() && nPacketsLeft > 0; itSearch++ )
	{
		CManagedSearch* pSearch = itSearch.value();
		if( pSearch->m_nCookie != m_nCookie )
		{
			nPacket = nPacketsPerSearch;
			nPacketsLeft -= nPacket;

			pSearch->Execute(tNow, &nPacket);
			pSearch->m_nCookie = m_nCookie;
			bUpdate = false;

			nPacketsLeft += nPacket;
		}
		pSearch->SendHits();
	}

	if( bUpdate || m_lSearches.size() == 1 )
		m_nCookie++;

}

bool CSearchManager::OnQueryAcknowledge(G2Packet *pPacket, IPv4_ENDPOINT &addr, QUuid &oGUID)
{
	if( !pPacket->m_bCompound )
        return false;

	pPacket->SkipCompound();			// skip children to get search GUID
	if( pPacket->GetRemaining() < 16 )	// must be at least 16 bytes for GUID
		return false;

	oGUID = pPacket->ReadGUID();		// Read search GUID

	QMutexLocker l(&m_pSection);

	if( CManagedSearch* pSearch = Find(oGUID) )	// is it our Query Ack?
	{
		// YES, this is ours, let's parse the packet and process it

		quint32 nFromIp = addr.ip;
		QList<quint32>  lDone;
		quint32 nRetryAfter = 0;
		qint64 tAdjust = 0;
		quint32 tNow = time(0);

		quint32 nHubs = 0, nLeaves = 0, nSuggestedHubs = 0;

		pPacket->m_nPosition = 0;	// reset position

		char szType[9];
		quint32 nLength = 0, nNext = 0;

		while( pPacket->ReadPacket(&szType[0], nLength) )
		{
			nNext = pPacket->m_nPosition + nLength;

			if( strcmp("D", szType) == 0 && nLength >= 4 )
			{
				quint32 nIp = pPacket->ReadIntBE<quint32>();
				lDone.append(nIp);

				if( nLength >= 6 )
				{
					quint16 nPort = pPacket->ReadIntLE<quint16>();
					IPv4_ENDPOINT a(nIp, nPort);
					HostCache.Add(a, tNow);
				}

				if( nLength >= 8 )
				{
					nLeaves += pPacket->ReadIntLE<quint16>();
				}
				nHubs++;
			}
			else if( strcmp("S", szType) == 0 && nLength >= 6 )
			{
				IPv4_ENDPOINT a;
				pPacket->ReadHostAddress(&a);
				quint32 tSeen = (nLength >= 10) ? pPacket->ReadIntLE<quint32>() + tAdjust : tNow;

				HostCache.Add(a, tSeen);
				nSuggestedHubs++;
			}
			else if( strcmp("TS", szType) == 0 && nLength >= 4 )
			{
				tAdjust = tNow - pPacket->ReadIntLE<quint32>();
			}
			else if( strcmp("RA", szType) == 0 && nLength >= 2 )
			{
				if( nLength >= 4 )
				{
					nRetryAfter = pPacket->ReadIntLE<quint32>();
				}
				else if( nLength >= 2 )
				{
					nRetryAfter = pPacket->ReadIntLE<quint16>();
				}

				CHostCacheHost* pHost = HostCache.Find(IPv4_ENDPOINT(nFromIp));
				if( pHost )
				{
					pHost->m_tRetryAfter = tNow + nRetryAfter;
				}
			}
			else if( strcmp("FR", szType) == 0 && nLength >= 4 )
			{
				nFromIp = pPacket->ReadIntBE<quint32>();
			}

			pPacket->m_nPosition = nNext;
		}

		// we already know QA GUID

		qDebug("Processing query acknowledge from %s (time adjust %+d seconds): %d hubs, %d leaves, %d suggested hubs, retry after %d seconds.",
			   addr.toString().toAscii().constData(), int(tAdjust), nHubs, nLeaves, nSuggestedHubs, nRetryAfter);

		pSearch->m_nHubs += nHubs;
		pSearch->m_nLeaves += nLeaves;

		pSearch->OnHostAcknowledge(nFromIp, tNow);

		for( int i = 0; i < lDone.size(); i++ )
		{
			pSearch->OnHostAcknowledge(lDone[i], tNow);
		}

		emit pSearch->StatsUpdated();

		return false;
	}

	// not our ack - tell caller to route it
	return true;

}

void CSearchManager::OnQueryHit(G2Packet *pPacket, CG2Node *pNode, IPv4_ENDPOINT *pEndpoint)
{
	qDebug() << "CSearchManager::OnQueryHit " << pPacket << pNode << pEndpoint;
	if( pEndpoint )
		qDebug() << pEndpoint->toString();

	QMutexLocker l(&m_pSection);

	// caller must check if it is compound...

	// do a shallow parsing...

	QSharedPointer<QueryHitInfo> pHitInfo(new QueryHitInfo());

	char szType[9];
	quint32 nLength = 0, nNext = 0;
	bool bCompound = false;

	bool bHaveHits = false;
	bool bHaveNA = false;
	bool bHaveGUID = false;

	if( pEndpoint )
	{
		pHitInfo->m_oNodeAddress = *pEndpoint;
		bHaveNA = true;
	}

	while( pPacket->ReadPacket(&szType[0], nLength, &bCompound) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp("H", szType) == 0 && bCompound )
		{
			bHaveHits = true;
			continue;
		}

		if( bCompound )
			pPacket->SkipCompound();

		if( strcmp("NA", szType) == 0 && nLength >= 6 )
		{
			IPv4_ENDPOINT oNodeAddr;
			pPacket->ReadHostAddress(&oNodeAddr);
			if( oNodeAddr.ip != 0 && oNodeAddr.port != 0 )
			{
				pHitInfo->m_oNodeAddress = oNodeAddr;
				bHaveNA = true;
			}
		}
		else if( strcmp("GU", szType) == 0 && nLength >= 16 )
		{
			QUuid oNodeGUID = pPacket->ReadGUID();
			if( !oNodeGUID.isNull() )
			{
				pHitInfo->m_oNodeGUID = oNodeGUID;
				bHaveGUID = true;
			}
		}
		else if( strcmp("NH", szType) == 0 && nLength >= 6 )
		{
			IPv4_ENDPOINT oNH;
			pPacket->ReadHostAddress(&oNH);
			if( oNH.ip != 0 && oNH.port != 0 )
			{
				pHitInfo->m_lNeighbouringHubs.append(oNH);
			}
		}
		else if( strcmp("V", szType) == 0 && nLength >= 4 )
		{
			pHitInfo->m_sVendor = pPacket->ReadString(4);
		}

		pPacket->m_nPosition = nNext;
	}

	if( pPacket->GetRemaining() < 17 || !bHaveHits || !bHaveNA || !bHaveGUID )
	{
		qDebug() << "Malformatted hit in CSearchManager" << pPacket->GetRemaining() << bHaveHits << bHaveNA << bHaveGUID;
		return;
	}

	pHitInfo->m_nHops = pPacket->ReadByte();
	pHitInfo->m_oGUID = pPacket->ReadGUID();

	if( CManagedSearch* pSearch = Find(pHitInfo->m_oGUID) )
	{
		// our search

		CQueryHit* pHit = CQueryHit::ReadPacket(pPacket, pHitInfo);

		if( pHit )
		{
			pSearch->OnQueryHit(pHit);
		}

		return;
	}

	// not our search - route it

	// increment hop counter, only if ttl not exceed 7, we must be a hub
	if( !Network.isHub() || pHitInfo->m_nHops > 7 )
		return;

	// m_nPosition - 1 = hop count
	pPacket->m_oBuffer[pPacket->m_nPosition - 17] = ++pHitInfo->m_nHops;

	if( pNode || pEndpoint )
	{
		if( pNode ) // if hit is received via tcp
		{
			// Hubs are only supposed to route hits...
			// hits could be returned by local hub cluster, possible need for routing

			// Add node ID to routing table
			Network.m_oRoutingTable.Add(pHitInfo->m_oNodeGUID, pNode, false);
		}
		if( pEndpoint ) // if hit is received via udp
		{
			if( pHitInfo->m_oNodeAddress == *pEndpoint )
			{
				// hits node address matches sender address
				Network.m_oRoutingTable.Add(pHitInfo->m_oNodeGUID, *pEndpoint);
			}
			else if( !pHitInfo->m_lNeighbouringHubs.isEmpty() )
			{
				// hits address does not match sender address (probably forwarded by a hub)
				// and there are neighbouring hubs available, use them instead (sender address can be used instead...)
				Network.m_oRoutingTable.Add(pHitInfo->m_oNodeGUID, pHitInfo->m_lNeighbouringHubs[0], false);
			}
		}

		Network.RoutePacket(pHitInfo->m_oGUID, pPacket);
	}

}
