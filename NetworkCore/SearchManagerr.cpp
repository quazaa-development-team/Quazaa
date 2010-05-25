#include "SearchManager.h"
#include "ManagedSearch.h"
#include "QueryHit.h"
#include "g2packet.h"
#include <QMutexLocker>
#include "hostcache.h"
#include "network.h"
#include <QMetaType>

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

bool CSearchManager::OnQueryHit(QueryHitSharedPtr pHits)
{
    QMutexLocker l(&m_pSection);

	// TODO: Check validity, security filters
	// now assuming hit is valid...



    if( CManagedSearch* pSearch = Find(pHits->m_pHitInfo->m_oGUID) )
    {
        pSearch->OnQueryHit(pHits);
		emit pSearch->StatsUpdated();
        return false;
    }

	// not our search - tell caller to route it
    return true;
}
