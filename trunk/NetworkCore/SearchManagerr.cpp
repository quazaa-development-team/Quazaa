#include "SearchManager.h"
#include "ManagedSearch.h"
#include "QueryHit.h"
#include "g2packet.h"
#include <QMutexLocker>
#include "hostcache.h"
#include <QMetaType>

CSearchManager SearchManager;

const quint32 PacketsPerSec = 8;

CSearchManager::CSearchManager(QObject *parent) :
    QObject(parent)
{
    m_nPruneCounter = 0;

    qRegisterMetaType<QueryHitSharedPtr>("QueryHitSharedPtr");
}

void CSearchManager::Add(CManagedSearch *pSearch)
{
    QMutexLocker l(&m_pSection);

    Q_ASSERT(!m_lSearches.contains(pSearch));
    m_lSearches.append(pSearch);
    if( pSearch->thread() != SearchManager.thread() )
        pSearch->moveToThread(SearchManager.thread());
}
void CSearchManager::Remove(CManagedSearch *pSearch)
{
    QMutexLocker l(&m_pSection);

    Q_ASSERT(m_lSearches.contains(pSearch));
    m_lSearches.removeAll(pSearch);
}

CManagedSearch* CSearchManager::Find(QUuid &oGUID)
{
    for( int i = 0; i < m_lSearches.size(); i++ )
    {
        if( m_lSearches[i]->m_oGUID == oGUID )
            return m_lSearches[i];
    }
    return 0;
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
        qDebug() << "HostCache Prune by ack";
        HostCache.PruneByQueryAck();
    }

    quint32 nPacketsPerSearch = qMax<quint32>(1, nPacketsLeft / nSearches);
    quint32 tNow = time(0);

    for( quint32 i = 0; i < nSearches && nPacketsLeft > 0; i++ )
    {
        quint32 nPacket = nPacketsPerSearch;
        nPacketsLeft -= nPacket;
        m_lSearches[i]->Execute(tNow, &nPacket);
        nPacketsLeft += nPacket;
    }

    if( nSearches > 1 )
    {
        CManagedSearch* pTemp = m_lSearches.takeFirst();
        m_lSearches.append(pTemp);
    }

}

bool CSearchManager::OnQueryAcknowledge(G2Packet *pPacket, IPv4_ENDPOINT &addr, QUuid &oGUID)
{
    if( !pPacket->HasChildren() )
        return false;

    quint32 nFromIp = addr.ip;
    QList<quint32>  lDone;
    quint32 nRetryAfter = 0;
    qint64 tAdjust = 0;
    quint32 tNow = time(0);

    quint32 nHubs = 0, nLeaves = 0, nSuggestedHubs = 0;

    while( G2Packet* pChild = pPacket->ReadNextChild() )
    {
        if( pChild->IsType("D") && pChild->PayloadLength() >= 4 )
        {
            quint32 nIp = pChild->ReadIntBE<quint32>();
            lDone.append(nIp);

            if( pChild->PayloadLength() >= 6 )
            {
                quint16 nPort = pChild->ReadIntLE<quint16>();
                IPv4_ENDPOINT a(nIp, nPort);
                HostCache.Add(a, tNow);
            }

            if( pChild->PayloadLength() >= 8 )
            {
                nLeaves += pChild->ReadIntLE<quint16>();
            }
            nHubs++;
        }
        else if( pChild->IsType("S") && pChild->PayloadLength() >= 6 )
        {
            IPv4_ENDPOINT a;
            pChild->ReadHostAddress(&a);
            quint32 tSeen = (pChild->PayloadLength() >= 10) ? pChild->ReadIntLE<quint32>() + tAdjust : tNow;

            HostCache.Add(a, tSeen);
            nSuggestedHubs++;
        }
        else if( pChild->IsType("TS") && pChild->PayloadLength() >= 4 )
        {
            tAdjust = tNow - pChild->ReadIntLE<quint32>();
        }
        else if( pChild->IsType("RA") && pChild->PayloadLength() >= 2 )
        {
            if( pChild->PayloadLength() >= 4 )
            {
                nRetryAfter = pChild->ReadIntLE<quint32>();
            }
            else if( pChild->PayloadLength() >= 2 )
            {
                nRetryAfter = pChild->ReadIntLE<quint16>();
            }

            CHostCacheHost* pHost = HostCache.Find(IPv4_ENDPOINT(nFromIp));
            if( pHost )
            {
                pHost->m_tRetryAfter = tNow + nRetryAfter;
            }
        }
        else if( pChild->IsType("FR") && pChild->PayloadLength() >= 4 )
        {
            nFromIp = pChild->ReadIntBE<quint32>();
        }
    }

    if( pPacket->PayloadLength() < 16 )
        return false;

    oGUID = pPacket->ReadGUID();

    qDebug("Processing query acknowledge from %s (time adjust %+d seconds): %d hubs, %d leaves, %d suggested hubs, retry after %d seconds.",
           addr.toString().toAscii().constData(), int(tAdjust), nHubs, nLeaves, nSuggestedHubs, nRetryAfter);

    QMutexLocker l(&m_pSection);

    if( CManagedSearch* pSearch = Find(oGUID) )
    {
        pSearch->m_nHubs += nHubs;
        pSearch->m_nLeaves += nLeaves;

        pSearch->OnHostAcknowledge(nFromIp, tNow);

        for( int i = 0; i < lDone.size(); i++ )
        {
            pSearch->OnHostAcknowledge(lDone[i], tNow);
        }

        emit StatsUpdated(pSearch);

        return false;
    }

    return true;

}

bool CSearchManager::OnQueryHit(QueryHitSharedPtr pHits)
{
    QMutexLocker l(&m_pSection);

    if( CManagedSearch* pSearch = Find(pHits->m_pHitInfo->m_oGUID) )
    {
        pSearch->OnQueryHit(pHits);
        emit StatsUpdated(pSearch);
        return false;
    }

    // routing
    return true;
}
