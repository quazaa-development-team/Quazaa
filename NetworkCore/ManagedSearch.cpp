#include "ManagedSearch.h"
#include "network.h"
#include "g2node.h"
#include "g2packet.h"
#include "Query.h"
#include "hostcache.h"
#include "datagrams.h"
#include "SearchManager.h"
#include "QueryHit.h"

CManagedSearch::CManagedSearch(CQuery* pQuery, QObject *parent) :
    QObject(parent)
{
    m_bActive = false;
    m_bPaused = false;
    m_pQuery = pQuery;
    m_tStarted = time(0);

    m_oGUID = QUuid::createUuid();
    pQuery->SetGUID(m_oGUID);

    m_nHubs = m_nLeaves = m_nHits = 0;

    m_bCanRequestKey = true;
    m_nQueryCount = 0;
}

CManagedSearch::~CManagedSearch()
{
    if( m_bActive || m_bPaused )
        Stop();

    if( m_pQuery )
        delete m_pQuery;
}

void CManagedSearch::Start()
{
    if( !m_bPaused )
        SearchManager.Add(this);

    m_bActive = true;
    m_bPaused = false;

    m_nQueryCount = 0;

}
void CManagedSearch::Pause()
{
    m_bPaused = true;
    m_bActive = false;
}
void CManagedSearch::Stop()
{
    m_bActive = false;
    m_bPaused = false;

    SearchManager.Remove(this);
}

void CManagedSearch::Execute(quint32 tNow, quint32 *pnMaxPackets)
{
    if( !m_bActive )
        return;

    if( m_nQueryCount > 1000 )
    {
        qDebug() << "==========> Pausing search: query limit reached";
        Pause();
        return;
    }

    quint32 nMaxPackets = *pnMaxPackets;

    if( tNow - m_tStarted < 30 )
        nMaxPackets = qMin(quint32(2), nMaxPackets);

    *pnMaxPackets -= nMaxPackets;

    SearchNeighbours(tNow);
    SearchG2(tNow, &nMaxPackets);

    *pnMaxPackets += nMaxPackets;

    m_bCanRequestKey = !m_bCanRequestKey;
}

void CManagedSearch::SearchNeighbours(quint32 tNow)
{
    qDebug() << "Searching neighbours";

    foreach(CG2Node* pNode, *Network.List())
    {
        if( pNode->m_nState == nsConnected
            && tNow - pNode->m_tConnected > 15
            && tNow - pNode->m_tLastQuery > 120
            && !m_lSearchedNodes.contains(pNode->m_oAddress.ip)
            )
        {
            G2Packet* pQuery = m_pQuery->ToG2Packet();
            if( pQuery )
            {
                m_lSearchedNodes[pNode->m_oAddress.ip] = tNow;
                pNode->SendPacket(pQuery);
            }
        }
    }
}

void CManagedSearch::SearchG2(quint32 tNow, quint32 *pnMaxPackets)
{
    qDebug() << "Searching on G2 " << *pnMaxPackets;

    CG2Node* pLastNeighbour = 0;

    for( quint32 i = 0; i < HostCache.size(); i++ )
    {
        CHostCacheHost* pHost = HostCache.m_lHosts[i];

        if( Network.IsConnectedTo(pHost->m_oAddress) )
            continue;

        if( !pHost->CanQuery(tNow) )
            continue;

        IPv4_ENDPOINT* pReceiver = 0;

        if( pHost->m_nQueryKey == 0 )
        {
            // nie ma keya
        }
        else if( !Network.IsFirewalled() )
        {
            if( pHost->m_nKeyHost == Network.m_oAddress.ip )
                pReceiver = &Network.m_oAddress;
            else
                pHost->m_nQueryKey = 0;
        }
        else
        {
            bool bFound = false;
            foreach( CG2Node* pNode, Network.m_lNodes )
            {
                if( pHost->m_nKeyHost == pNode->m_oAddress.ip )
                {
                    pReceiver = &pNode->m_oAddress;
                    bFound = true;
                }
            }
            if( !bFound )
                pHost->m_nQueryKey = 0;
        }

        if( pHost->m_nQueryKey != 0 )
        {
                // send query

            if( m_lSearchedNodes.contains(pHost->m_oAddress.ip) )
            {
                if( tNow - m_lSearchedNodes[pHost->m_oAddress.ip] < 300 )
                    continue;
            }

            m_lSearchedNodes[pHost->m_oAddress.ip] = tNow;

            pHost->m_tLastQuery = tNow;
            if( pHost->m_tAck == 0 )
                pHost->m_tAck = tNow;

            G2Packet* pQuery = m_pQuery->ToG2Packet(pReceiver, pHost->m_nQueryKey);

            if( pQuery )
            {
                qDebug("Querying %s", pHost->m_oAddress.toString().toAscii().constData());
                *pnMaxPackets -= 1;
                Datagrams.SendPacket(pHost->m_oAddress, pQuery, true);
                pQuery->Release();
                m_nQueryCount++;
            }
        }
        else if( m_bCanRequestKey && tNow - pHost->m_nKeyTime > 300 )
        {
            pReceiver = 0;
            CG2Node* pHub = 0;

            if( !Network.IsFirewalled() )
            {
                pReceiver = &Network.m_oAddress;
            }
            else
            {
                bool bCheckLast = Network.m_nHubsConnected > 2;
                foreach(CG2Node* pNode, Network.m_lNodes)
                {
                    if( m_lSearchedNodes.contains(pNode->m_oAddress.ip)
                        && pNode->m_nType == G2_HUB )
                    {
                        if( (bCheckLast && pNode == pLastNeighbour) )
                            continue;

                        if( pHub )
                        {
                            if( pNode->m_nPingsWaiting == 0 && pNode->m_tRTT < pHub->m_tRTT && pNode->m_tRTT < 10000 )
                            {
                                pHub = pNode;
                                pReceiver = &pNode->m_oAddress;
                            }
                        }
                        else if( pNode->m_nPingsWaiting == 0 )
                        {
                            pHub = pNode;
                            pReceiver = &pNode->m_oAddress;
                        }
                    }
                }

                if( pHub )
                {
                    pLastNeighbour = pHub;
                    if( pHub->m_tKeyRequest == 0 )
                        pHub->m_tKeyRequest = tNow;
                }
            }

            if( pReceiver != 0 )
            {
                G2Packet* pQKR = G2Packet::New("QKR");

                if( pReceiver == &Network.m_oAddress )
                {
                    qDebug("Requesting query key from %s", pHost->m_oAddress.toString().toAscii().constData());
                }
                else
                {
                    pQKR->WriteChild("RNA")->WriteHostAddress(pReceiver);
                    qDebug("Requesting query key from %s for %s", pHost->m_oAddress.toString().toAscii().constData(), pReceiver->toString().toAscii().constData());
                }

                Datagrams.SendPacket(pHost->m_oAddress, pQKR, false);
                *pnMaxPackets -= 1;
                pQKR->Release();

                if( pHost->m_tAck == 0 )
                    pHost->m_tAck = tNow;
                pHost->m_nKeyTime = tNow;
                pHost->m_nQueryKey = 0;
            }
        }

        if( *pnMaxPackets == 0 )
            break;
    }
}

void CManagedSearch::OnHostAcknowledge(quint32 nHost, quint32 tNow)
{
    m_lSearchedNodes[nHost] = tNow;
}

void CManagedSearch::OnQueryHit(QueryHitSharedPtr pHits)
{
    CQueryHit* pHit = pHits.data();

    while( pHit != 0 )
    {
        m_nHits++;
        pHit = pHit->m_pNext;
    }

    emit OnHit(pHits);
}
