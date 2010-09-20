//
// managedsearch.cpp
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

#include "managedsearch.h"
#include "network.h"
#include "neighbours.h"
#include "g2node.h"
#include "g2packet.h"
#include "query.h"
#include "hostcache.h"
#include "datagrams.h"
#include "searchmanager.h"
#include "queryhit.h"

#include "quazaasettings.h"

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
	m_nCookie = 0;
	m_nCachedHits = 0;
	m_pCachedHit = 0;
}

CManagedSearch::~CManagedSearch()
{
    if( m_bActive || m_bPaused )
        Stop();

    if( m_pQuery )
        delete m_pQuery;

	if( m_pCachedHit )
	{
		delete m_pCachedHit;
		m_nCachedHits = 0;
		m_pCachedHit = 0;
	}
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

	if( m_nQueryCount > quazaaSettings.Gnutella2.QueryLimit )
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

	QMutexLocker l(&Neighbours.m_pSection);

	for( QList<CG2Node*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode )
    {
		CG2Node* pNode = *itNode;

        if( pNode->m_nState == nsConnected
            && tNow - pNode->m_tConnected > 15
			&& tNow - pNode->m_tLastQuery > quazaaSettings.Gnutella2.QueryHostThrottle
            && !m_lSearchedNodes.contains(pNode->m_oAddress.ip)
            )
        {
            G2Packet* pQuery = m_pQuery->ToG2Packet();
            if( pQuery )
            {
                m_lSearchedNodes[pNode->m_oAddress.ip] = tNow;
				pNode->SendPacket(pQuery, true, true);
            }
        }
    }
}

void CManagedSearch::SearchG2(quint32 tNow, quint32 *pnMaxPackets)
{
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
			QMutexLocker l(&Neighbours.m_pSection);

            bool bFound = false;
			for( QList<CG2Node*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode )
			{
				CG2Node* pNode = *itNode;
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
				if( tNow - m_lSearchedNodes[pHost->m_oAddress.ip] < quazaaSettings.Gnutella2.QueryHostThrottle )
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
		else if( m_bCanRequestKey && tNow - pHost->m_nKeyTime > quazaaSettings.Gnutella2.QueryKeyTime )
        {
            pReceiver = 0;
            CG2Node* pHub = 0;

            if( !Network.IsFirewalled() )
            {
                pReceiver = &Network.m_oAddress;
            }
            else
            {
				QMutexLocker l(&Neighbours.m_pSection);

				bool bCheckLast = Neighbours.m_nHubsConnected > 2;
				for( QList<CG2Node*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode )
                {
					CG2Node* pNode = *itNode;

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

			if( pHub && pHub->m_bCachedKeys )
			{
				G2Packet* pQKR = G2Packet::New("QKR", true);
				pQKR->WritePacket("QNA", 6)->WriteHostAddress(&pHost->m_oAddress);

				qDebug("Requesting query key from %s through %s", pHost->m_oAddress.toString().toAscii().constData(), pHub->m_oAddress.toString().toAscii().constData() );
				pHub->SendPacket(pQKR, true, true);
				*pnMaxPackets -= 1;

				if( pHost->m_tAck == 0 )
					pHost->m_tAck = tNow;
				pHost->m_nKeyTime = tNow;
				pHost->m_nQueryKey = 0;

			}
			else if( pReceiver != 0 )
            {
                if( pReceiver == &Network.m_oAddress )
                {
					G2Packet* pQKR = G2Packet::New("QKR", false);
					Datagrams.SendPacket(pHost->m_oAddress, pQKR, false);
					pQKR->Release();
                    qDebug("Requesting query key from %s", pHost->m_oAddress.toString().toAscii().constData());
                }
                else
                {
					G2Packet* pQKR = G2Packet::New("QKR", true);
					pQKR->WritePacket("RNA", 6)->WriteHostAddress(pReceiver);
					Datagrams.SendPacket(pHost->m_oAddress, pQKR, false);
					pQKR->Release();
                    qDebug("Requesting query key from %s for %s", pHost->m_oAddress.toString().toAscii().constData(), pReceiver->toString().toAscii().constData());
                }

                *pnMaxPackets -= 1;

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

void CManagedSearch::OnQueryHit(CQueryHit* pHits)
{
	CQueryHit* pHit = pHits;
	CQueryHit* pLast = 0;

    while( pHit != 0 )
    {
        m_nHits++;
		m_nCachedHits++;
        pHit = pHit->m_pNext;
		if( pHit )
			pLast = pHit;
    }

	//emit OnHit(pHits);

	if( !m_pCachedHit )
	{
		m_pCachedHit = pHits;
	}
	else if( pLast )
	{
		pLast->m_pNext = pHits;
	}

	if( m_nCachedHits > 100 )
	{
		SendHits();
	}
}
void CManagedSearch::SendHits()
{
	if( !m_pCachedHit )
		return;

	qDebug() << "Sending hits..." << m_nCachedHits;
	QueryHitSharedPtr pSHits(m_pCachedHit);
	emit OnHit(pSHits);
	m_pCachedHit = 0;
	m_nCachedHits = 0;
}
