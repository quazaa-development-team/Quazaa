//
// datagrams.cpp
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

#include "datagrams.h"
#include "network.h"
#include "datagramfrags.h"
#include "g2node.h"
#include "g2packet.h"
#include <QTimer>
#include "hostcache.h"
#include "searchmanager.h"
#include "queryhit.h"

#include "quazaaglobals.h"
#include "quazaasettings.h"

#include "thread.h"


CDatagrams Datagrams;
CThread DatagramsThread;

CDatagrams::CDatagrams()
{
	m_nUploadLimit = 8192; // TODO it

    m_pRecvBuffer = new QByteArray();
    m_pHostAddress = new QHostAddress();
    m_nSequence = 0;

    m_bActive = false;

    m_pSocket = 0;
    m_tSender = 0;
    m_bFirewalled = true;

}
CDatagrams::~CDatagrams()
{
    if( m_bActive )
        Disconnect();

    if( m_pSocket )
        delete m_pSocket;

    if( m_tSender )
        delete m_tSender;

    if( m_pRecvBuffer )
        delete m_pRecvBuffer;
    if( m_pHostAddress )
        delete m_pHostAddress;
}

void CDatagrams::SetupThread()
{
    qDebug() << "SetupThread";
    qDebug() << "Thread id: " << QThread::currentThreadId();

    Q_ASSERT(m_pSocket == 0);
    Q_ASSERT(m_tSender == 0);

    m_pSocket = new QUdpSocket(this);
    m_tSender = new QTimer(this);

    IPv4_ENDPOINT addr = Network.GetLocalAddress();
    if( m_pSocket->bind(addr.port) )
    {
		qDebug() << "Datagrams listening on " << m_pSocket->localPort() << addr.port;
        m_nDiscarded = 0;

		for( int i = 0; i < quazaaSettings.Gnutella2.UdpBuffers; i++ )
        {
            m_FreeBuffer.push(new QByteArray());
        }

		for( int i = 0; i < quazaaSettings.Gnutella2.UdpInFrames; i++ )
        {
            m_FreeDGIn.push(new DatagramIn());
        }

		for( int i = 0; i < quazaaSettings.Gnutella2.UdpOutFrames; i++ )
		{
			m_FreeDGOut.push(new DatagramOut());
		}

        connect(this, SIGNAL(SendQueueUpdated()), this, SLOT(FlushSendCache()), Qt::QueuedConnection);
        connect(m_tSender, SIGNAL(timeout()), this, SLOT(FlushSendCache()), Qt::QueuedConnection);
		connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(OnDatagram()), Qt::QueuedConnection);

        m_tSender->setInterval(200);

        m_bActive = true;
        m_tMeterTimer.start();
    }
}

void CDatagrams::Listen()
{
    if( m_bActive )
    {
        qDebug() << "CDatagrams::Listen - already listening";
        return;
    }

    SetupThread();
    m_bFirewalled = true;

    if( !m_bActive )
    {
        qDebug() << "Can't bind UDP socket! UDP communication disabled!";
        Disconnect();
    }

}

void CDatagrams::CleanupThread()
{
    if( m_tSender )
    {
        m_tSender->stop();
        m_tSender->disconnect();
        delete m_tSender;
        m_tSender = 0;
    }

    if( m_pSocket )
    {
        m_pSocket->close();
        m_pSocket->disconnect();
        delete m_pSocket;
        m_pSocket = 0;
    }

    disconnect(SIGNAL(SendQueueUpdated()));

    while( !m_SendCache.isEmpty() )
        Remove(m_SendCache.pop());

    while( !m_RecvCache.isEmpty() )
        RemoveOldIn(true);

    while( !m_FreeDGIn.isEmpty() )
        delete m_FreeDGIn.pop();

    while( !m_FreeDGOut.isEmpty() )
        delete m_FreeDGOut.pop();

    while( !m_FreeBuffer.isEmpty() )
        delete m_FreeBuffer.pop();
}

void CDatagrams::Disconnect()
{
    m_bActive = false;

    CleanupThread();
}

void CDatagrams::OnDatagram()
{
    if( !m_bActive )
        return;

	if( !Network.m_pSection.tryLock(50) )
	{
		// receive and discard datagram, g2 reliability layer should retransmit
		m_nBandwidthIn += m_pSocket->pendingDatagramSize();
		m_pSocket->readDatagram(0, 0, 0, 0);

		qWarning() << "Can't get lock in CDatagrams::OnDatagram. Network core overloaded.";
		return;
	}

	quint32 nCounter = 100;

	while( m_pSocket->hasPendingDatagrams() && nCounter-- )
    {
        qint64 nSize = m_pSocket->pendingDatagramSize();
        m_pRecvBuffer->resize(nSize);
        qint64 nReadSize = m_pSocket->readDatagram(m_pRecvBuffer->data(), nSize, m_pHostAddress, &m_nPort);

        m_nBandwidthIn += nReadSize;

        if( nReadSize < 8 )
			continue;
			//return;

        GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();
        if( strncmp((char*)&pHeader->szTag, "GND", 3) == 0 && pHeader->nPart > 0 && (pHeader->nCount == 0 || pHeader->nPart <= pHeader->nCount) )
        {
            if( pHeader->nCount == 0 )
            {
                // ACK
                OnAcknowledgeGND();
            }
            else
            {
                // DG
                OnReceiveGND();
            }           
        }
    }

	Network.m_pSection.unlock();
}

void CDatagrams::OnReceiveGND()
{

    GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();
    quint32 nIp = m_pHostAddress->toIPv4Address();
    quint32 nSeq = ((pHeader->nSequence << 16) & 0xFFFF0000) + (m_nPort & 0x0000FFFF);

	//qDebug("Received GND from %s:%u nSequence = %u nPart = %u nCount = %u", m_pHostAddress->toString().toAscii().constData(), m_nPort, pHeader->nSequence, pHeader->nPart, pHeader->nCount);
    DatagramIn* pDG = 0;

    if( m_RecvCache.contains(nIp) && m_RecvCache[nIp].contains(nSeq) )
    {
        pDG = m_RecvCache[nIp][nSeq];

        // Zeby dac szanse wiekszym pakietom ;)
        if( pDG->m_nLeft )
            pDG->m_tStarted = time(0);
    }
    else
    {
        if( m_FreeDGIn.size() )
        {
            pDG = m_FreeDGIn.pop();
        }
        else
        {
            if( m_FreeDGIn.isEmpty() )
            {
                RemoveOldIn(false);
                if( m_FreeDGIn.isEmpty() )
                {
                    qDebug() << "UDP in frames exhausted";
                    m_nDiscarded++;
                    return;
                }
            }

            pDG = m_FreeDGIn.pop();
        }

        if( m_FreeBuffer.size() < pHeader->nCount )
        {
            m_nDiscarded++;
            m_FreeDGIn.push(pDG);
            RemoveOldIn(false);
            return;
        }

        pDG->Create(IPv4_ENDPOINT(m_pHostAddress->toIPv4Address(), m_nPort), pHeader->nFlags, pHeader->nSequence, pHeader->nCount);

        for( int i = 0; i < pHeader->nCount; i++ )
        {
            Q_ASSERT(pDG->m_pBuffer[i] == 0);
            pDG->m_pBuffer[i] = m_FreeBuffer.pop();
        }

        m_RecvCache[nIp][nSeq] = pDG;
    }

    // Dopiero tutaj, na wypadek gdybysmy nie mieli wolnych datagramow
    // ACK = odebralismy datagram, a jesli odebralismy i odrzucilismy to nie wysylamy ACK-a
    if( pHeader->nFlags & 0x02 )
    {
        GND_HEADER oAck;

        memcpy(&oAck, pHeader, sizeof(GND_HEADER));
        oAck.nCount = 0;
        oAck.nFlags = 0;
		//qDebug() << "Sending UDP ACK to " << m_pHostAddress->toString() << m_nPort;
        m_pSocket->writeDatagram((char*)&oAck, sizeof(GND_HEADER), *m_pHostAddress, m_nPort);
    }

    if( pDG->Add(pHeader->nPart, m_pRecvBuffer->constData() + sizeof(GND_HEADER), m_pRecvBuffer->size() - sizeof(GND_HEADER)) )
    {

		G2Packet* pPacket = 0;
        try
        {
			IPv4_ENDPOINT addr(m_pHostAddress->toIPv4Address(), m_nPort);
			pPacket = pDG->ToG2Packet();
            if( pPacket )
            {
                OnPacket(addr, pPacket);
			}

        }
        catch(...)
        {

        }
		if( pPacket )
			pPacket->Release();

        Remove(pDG, true);
    }

}

void CDatagrams::OnAcknowledgeGND()
{
    GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();

	//qDebug() << "UDP received GND ACK from" << m_pHostAddress->toString().toAscii().constData() << "seq" << pHeader->nSequence << "part" << pHeader->nPart;

    if( !m_SendCacheMap.contains(pHeader->nSequence) )
        return;

    DatagramOut* pDG = m_SendCacheMap.value(pHeader->nSequence);

    if( pDG->Acknowledge(pHeader->nPart) )
        Remove(pDG);
}

void CDatagrams::Remove(DatagramIn *pDG, bool bReclaim)
{

    for( int i = 0; i < pDG->m_nCount; i++ )
    {
        if( pDG->m_pBuffer[i] )
        {
            m_FreeBuffer.push(pDG->m_pBuffer[i]);
            pDG->m_pBuffer[i]->clear();
            pDG->m_pBuffer[i] = 0;
        }
    }

    if( bReclaim )
        return;

    bool bFound = false;

    for( QHash<quint32, QHash<quint32, DatagramIn*> >::iterator it = m_RecvCache.begin(); it != m_RecvCache.end(); it++ )
    {
        for( QHash<quint32, DatagramIn*>::iterator it2 = it.value().begin(); it2 != it.value().end(); it2++ )
        {
            if( it2.value() == pDG )
            {
                bFound = true;
                m_RecvCache[it.key()].remove(it2.key());
                m_FreeDGIn.push(pDG);
                break;
            }
        }

        if( bFound )
        {
            if( it.value().size() == 0 )
            {
                m_RecvCache.remove(it.key());
                break;
            }
        }
    }
}

// Usuwa jeden pakiet z cache'u odbioru
void CDatagrams::RemoveOldIn(bool bForce)
{
    DatagramIn* pOldest = 0;
    quint32 tNow = time(0);

    for( QHash<quint32, QHash<quint32, DatagramIn*> >::iterator it = m_RecvCache.begin(); it != m_RecvCache.end(); it++ )
    {
        for( QHash<quint32, DatagramIn*>::iterator itPacket = it.value().begin(); itPacket != it.value().end(); itPacket++ )
        {
            if( pOldest )
            {
				if( pOldest->m_tStarted < itPacket.value()->m_tStarted && (bForce || tNow - itPacket.value()->m_tStarted > quazaaSettings.Gnutella2.UdpInExpire || itPacket.value()->m_nLeft == 0) )
                {
                    pOldest = itPacket.value();
                }
            }
            else
            {
				if( bForce || tNow - itPacket.value()->m_tStarted > quazaaSettings.Gnutella2.UdpInExpire || itPacket.value()->m_nLeft == 0 )
                {
                    pOldest = itPacket.value();
                }

            }
        }
    }

    if( pOldest )
    {
        Remove(pOldest);
    }


}

void CDatagrams::Remove(DatagramOut *pDG)
{
    if( m_SendCache.isEmpty() )
        return;

    m_SendCacheMap.remove(pDG->m_nSequence);
    int nIndex = m_SendCache.indexOf(pDG);
    if( nIndex == -1 )
        return;
    m_SendCache.remove(nIndex);
    m_FreeDGOut.push(pDG);
    if( pDG->m_pBuffer )
    {
        m_FreeBuffer.push(pDG->m_pBuffer);
        pDG->m_pBuffer->clear();
        pDG->m_pBuffer = 0;
    }
}

void CDatagrams::FlushSendCache()
{
	if( !m_bActive )
		return;

	QMutexLocker l(&Network.m_pSection);

    quint32 tNow = time(0);

    qint32 nMsecs = 1000;
    if( !m_tStopWatch.isNull() )
        nMsecs = qMin(nMsecs, m_tStopWatch.elapsed());

    quint32 nToWrite = (m_nUploadLimit * nMsecs) / 1000;

    quint32 nLastHost = 0;

    while( nToWrite > 0 && !m_SendCache.isEmpty() )
    {
        bool bSent = false;

        char* pPacket;
        quint32 nPacket;

        for( int i = m_SendCache.size() - 1; i >= 0; i--)
        {
            DatagramOut* pDG = m_SendCache[i];

            if( pDG->m_oAddress.ip == nLastHost )
                continue;

			// TODO: sprawdzenie UDP na firewallu - mog? by? 3 stany udp
            if( pDG->GetPacket(tNow, &pPacket, &nPacket, true) )
            {
				//qDebug() << "UDP sending to " << pDG->m_oAddress.toString().toAscii().constData() << "seq" << pDG->m_nSequence << "nPart" << ((GND_HEADER*)&pPacket)->nPart << "count" << pDG->m_nCount;

                m_pSocket->writeDatagram(pPacket, nPacket, QHostAddress(pDG->m_oAddress.ip), pDG->m_oAddress.port);

                nLastHost = pDG->m_oAddress.ip;

                if( nToWrite >= nPacket )
                    nToWrite -= nPacket;
                else
                    nToWrite = 0;

                m_nBandwidthOut += nPacket;

                if( !pDG->m_bAck )
                    Remove(pDG);

                bSent = true;

                break;
            }
        }

        if( m_SendCache.isEmpty() || !bSent )
            break;
    }

    for( int i = 0; i < m_SendCache.size(); )
    {
        DatagramOut* pDG = m_SendCache.at(i);

		if( tNow - pDG->m_tSent > quazaaSettings.Gnutella2.UdpOutExpire )
        {
            Remove(pDG);
        }
        else
        {
            i++;
        }
    }
    
}

void CDatagrams::SendPacket(IPv4_ENDPOINT &oAddr, G2Packet *pPacket, bool bAck, DatagramWatcher *pWatcher, void *pParam)
{
	if( !m_bActive )
		return;

    Q_UNUSED(pWatcher);
    Q_UNUSED(pParam);

    if( m_FreeDGOut.isEmpty() )
    {
        Remove(m_SendCache.first());
        qDebug() << "UDP out frames exhausted";
    }

    if( m_FreeBuffer.isEmpty() )
    {
        RemoveOldIn(false);

        if( m_FreeBuffer.isEmpty() )
        {
            qDebug() << "UDP out discarded, out of buffers";
            return;
        }
    }

    DatagramOut* pDG = m_FreeDGOut.pop();
    pDG->Create(oAddr, pPacket, m_nSequence++, m_FreeBuffer.pop(), bAck);

    m_SendCache.push(pDG);
    m_SendCacheMap[pDG->m_nSequence] = pDG;

    // TODO: Powiadomienia do obiektow nasluchujacych, jesli podano

	//qDebug() << "UDP queued for " << oAddr.toString().toAscii().constData() << "seq" << pDG->m_nSequence << "parts" << pDG->m_nCount;


    emit SendQueueUpdated();

}


void CDatagrams::OnPacket(IPv4_ENDPOINT addr, G2Packet *pPacket)
{
	try
    {
        if(pPacket->IsType("PI"))
            OnPing(addr, pPacket);
        else if(pPacket->IsType("PO"))
            OnPong(addr, pPacket);
        else if(pPacket->IsType("CRAWLR"))
            OnCRAWLR(addr, pPacket);
        else if(pPacket->IsType("QKA"))
            OnQKA(addr, pPacket);
		else if(pPacket->IsType("QA"))
            OnQA(addr, pPacket);
        else if(pPacket->IsType("QH2"))
			OnQH2(addr, pPacket);
        else
		{
			//qDebug() << "UDP RECEIVED unknown packet " << pPacket->GetType();
		}
    }
    catch(...)
    {
        qDebug() << "malformed packet";
	}
}

void CDatagrams::OnPing(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
	G2Packet* pNew = G2Packet::New("PO", false);
    SendPacket(addr, pNew, false);
    pNew->Release();

}
void CDatagrams::OnPong(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
	if( pPacket->m_bCompound )
    {
		char szType[9];
		quint32 nLength = 0, nNext = 0;

		while( pPacket->ReadPacket(&szType[0], nLength) )
		{
			nNext = pPacket->m_nPosition + nLength;

			if( strcmp("RELAY", szType) == 0 )
			{
				if( !Network.IsConnectedTo(addr) )
				{
					m_bFirewalled = false;
				}
			}

			pPacket->m_nPosition = nNext;
		}
    }
}
void CDatagrams::OnCRAWLR(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
	bool bRLeaf = false;
	bool bRNick = false;
	bool bRGPS = false;
	bool bRExt = false;

	if( !pPacket->m_bCompound )
		return;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while( pPacket->ReadPacket(&szType[0], nLength) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp("RLEAF", szType) == 0 )
			bRLeaf = true;
		else if( strcmp("RNAME", szType) == 0 )
			bRNick = true;
		else if( strcmp("RGPS", szType) == 0 )
			bRGPS = true;
		else if( strcmp("REXT", szType) == 0 )
			bRExt = true;

		pPacket->m_nPosition = nNext;
	}


	G2Packet* pCA = G2Packet::New("CRAWLA", true);

	G2Packet* pTmp = G2Packet::New("SELF", true);
    if( Network.isHub() )
		pTmp->WritePacket("HUB", 0);
    else
		pTmp->WritePacket("LEAF", 0);
	pTmp->WritePacket("NA", 6)->WriteHostAddress(&Network.m_oAddress);
	pTmp->WritePacket("CV", quazaaGlobals.UserAgentString().toUtf8().size())->WriteString(quazaaGlobals.UserAgentString(), false);
	pTmp->WritePacket("V", 4)->WriteString(quazaaGlobals.VendorCode(), false);;
	if( !quazaaSettings.Profile.GnutellaScreenName.isEmpty() )
		pTmp->WritePacket("NAME", quazaaSettings.Profile.GnutellaScreenName.left(255).toUtf8().size())->WriteString(quazaaSettings.Profile.GnutellaScreenName.left(255));

	pCA->WritePacket(pTmp);
	pTmp->Release();

	foreach( CG2Node* pNode, Network.m_lNodes )
	{
		if( pNode->m_nState == nsConnected )
		{
			if( pNode->m_nType == G2_HUB )
			{
				G2Packet* pNH = G2Packet::New("NH");
				pNH->WritePacket("NA", 6)->WriteHostAddress(&pNode->m_oAddress);
				pCA->WritePacket(pNH);
				pNH->Release();
			}
			else if( pNode->m_nType == G2_LEAF )
			{
				G2Packet* pNL = G2Packet::New("NL");
				pNL->WritePacket("NA", 6)->WriteHostAddress(&pNode->m_oAddress);
				pCA->WritePacket(pNL);
				pNL->Release();
			}
		}
	}

    SendPacket(addr, pCA, true);

	pCA->Release();
}
void CDatagrams::OnQKA(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
	if( !pPacket->m_bCompound )
        return;

    quint32 nKey = 0;
	quint32 nKeyHost = 0;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while( pPacket->ReadPacket(&szType[0], nLength) )
    {
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp("QK", szType) == 0 && nLength >= 4 )
        {
			pPacket->ReadIntLE(&nKey);
        }
		else if( strcmp("SNA", szType) == 0 && nLength >= 4 )
        {
			pPacket->ReadIntBE(&nKeyHost);
        }
		pPacket->m_nPosition = nNext;
    }

    CHostCacheHost* pCache = HostCache.Add(addr, 0);
    if( pCache )
        pCache->SetKey(nKey);


    qDebug("Got a query key for %s = 0x%x", addr.toString().toAscii().constData(), nKey);

	if( Network.isHub() && nKeyHost != 0 && nKeyHost != Network.m_oAddress.ip )
	{
		if( CG2Node* pNode = Network.FindNode(nKeyHost) )
		{
			qDebug() << "Forwarding Query Key to " << pNode->m_oAddress.toString().toAscii().constData();

			char* pOut = pPacket->WriteGetPointer(11, 0);
			*pOut++ = 0x50;
			*pOut++ = 6;
			*pOut++ = 'Q';
			*pOut++ = 'N';
			*pOut++ = 'A';

			char assm[6];
			*(quint32*)&assm[0] = qToBigEndian(addr.ip);
			*(quint16*)&assm[4] = qToLittleEndian(addr.port);
			memcpy(pOut, &assm[0], 6);

			pPacket->AddRef();
			pNode->SendPacket(pPacket, true, true);
		}
	}
    // TODO Forwarding jesli hub i SNA != lokalny, dodac QNA

}
void CDatagrams::OnQA(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
    CHostCacheHost* pHost = HostCache.Add(addr, 0);
    if( pHost )
    {
        pHost->m_tAck = 0;
    }

    QUuid oGuid;

	// Hubs are only supposed to route UDP /QA - we'll drop it if we're in leaf mode
	if( SearchManager.OnQueryAcknowledge(pPacket, addr, oGuid) && Network.isHub() )
    {
		// Add from address
		// pPacket->WriteChild("FR")->WriteHostAddress(&addr);

		Network.RoutePacket(oGuid, pPacket);
    }

}

void CDatagrams::OnQH2(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
	if( !pPacket->m_bCompound )
		return;

	SearchManager.OnQueryHit(pPacket, 0, &addr);
}

