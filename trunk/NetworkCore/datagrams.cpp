#include "datagrams.h"
#include "network.h"
#include "datagramfrags.h"
#include "g2packet.h"
#include <QTimer>
#include "hostcache.h"
#include "SearchManager.h"

#include "Thread.h"

CDatagrams Datagrams;
CThread DatagramsThread;

CDatagrams::CDatagrams()
{
    m_nUploadLimit = 8192;

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
        m_nDiscarded = 0;

        for( int i = 0; i < 512; i++ )
        {
            m_FreeBuffer.push(new QByteArray());
        }

        for( int i = 0; i < 256; i++ )
        {
            m_FreeDGIn.push(new DatagramIn());
            m_FreeDGOut.push(new DatagramOut());
        }

        connect(this, SIGNAL(SendQueueUpdated()), this, SLOT(FlushSendCache()), Qt::QueuedConnection);
        connect(m_tSender, SIGNAL(timeout()), this, SLOT(FlushSendCache()), Qt::QueuedConnection);
        connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(OnDatagram()));

        m_tSender->setInterval(200);
        //m_tSender->start();

        m_bActive = true;
        m_tMeterTimer.start();

        //QTimer::singleShot(60000, this, SLOT(Flood()));

    }
}

void CDatagrams::Listen()
{
    /*QMutexLocker l(&m_pSection);
    qDebug() << "Locked in Listen";*/

    qDebug() << "Listen Thread id: " << QThread::currentThreadId();

    if( m_bActive )
    {
        qDebug() << "CDatagrams::Listen - already listening";
        return;
    }

    //DatagramsThread.start(&m_pSection, this);
    SetupThread();
    m_bFirewalled = true;

    if( !m_bActive )
    {
        qDebug() << "Can't bind UDP socket! UDP communication disabled!";
        //l.unlock();
        Disconnect();
    }

}

void CDatagrams::CleanupThread()
{
    qDebug() << "CleanupThread";
    qDebug() << "Thread id: " << QThread::currentThreadId();

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
    //QMutexLocker l(&m_pSection);
    //qDebug() << "Locked in Disconnect";
    //qDebug() << "Thread id: " << QThread::currentThreadId();

    m_bActive = false;

    CleanupThread();
    /*if( !DatagramsThread.isRunning() )
        return;

    DatagramsThread.exit(0);*/

}

void CDatagrams::OnDatagram()
{
    //QMutexLocker l(&m_pSection);
    //qDebug() << "Locked in OnDatagram";
    qDebug() << "Thread id: " << QThread::currentThreadId();

    if( !m_bActive )
        return;

    //qDebug() << "OnDatagram " << QThread::currentThreadId();
    //while( m_pSocket->hasPendingDatagrams() )
    {
        qint64 nSize = m_pSocket->pendingDatagramSize();
        m_pRecvBuffer->resize(nSize);
        qint64 nReadSize = m_pSocket->readDatagram(m_pRecvBuffer->data(), nSize, m_pHostAddress, &m_nPort);

        m_nBandwidthIn += nReadSize;

        if( nReadSize < 8 )
            //continue;
            return;

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
}

void CDatagrams::OnReceiveGND()
{

    GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();
    quint32 nIp = m_pHostAddress->toIPv4Address();
    quint32 nSeq = ((pHeader->nSequence << 16) & 0xFFFF0000) + (m_nPort & 0x0000FFFF);

    qDebug("Received GND from %s:%u nSequence = %u nPart = %u nCount = %u", m_pHostAddress->toString().toAscii().constData(), m_nPort, pHeader->nSequence, pHeader->nPart, pHeader->nCount);
    DatagramIn* pDG = 0;

    if( m_RecvCache.contains(nIp) && m_RecvCache[nIp].contains(nSeq) )
    {
        //qDebug() << "Reusing existing datagram";
        pDG = m_RecvCache[nIp][nSeq];

        // Zeby dac szanse wiekszym pakietom ;)
        if( pDG->m_nLeft )
            pDG->m_tStarted = time(0);
    }
    else
    {
        //qDebug() << "Creating new datagram";
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
            //qDebug() << "Discarding datagram, out of buffers";
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
        qDebug() << "Sending UDP ACK to " << m_pHostAddress->toString() << m_nPort;
        m_pSocket->writeDatagram((char*)&oAck, sizeof(GND_HEADER), *m_pHostAddress, m_nPort);
    }

    if( pDG->Add(pHeader->nPart, m_pRecvBuffer->constData() + sizeof(GND_HEADER), m_pRecvBuffer->size() - sizeof(GND_HEADER)) )
    {

        try
        {
            IPv4_ENDPOINT addr(m_pHostAddress->toIPv4Address(), m_nPort);
            G2Packet* pPacket = pDG->ToG2Packet();
            if( pPacket )
            {
                OnPacket(addr, pPacket);
                pPacket->Release();
            }

        }
        catch(...)
        {

        }

        Remove(pDG, true);
    }

}

void CDatagrams::OnAcknowledgeGND()
{
    GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();

    qDebug() << "UDP received GND ACK from" << m_pHostAddress->toString().toAscii().constData() << "seq" << pHeader->nSequence << "part" << pHeader->nPart;

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
                if( pOldest->m_tStarted < itPacket.value()->m_tStarted && (bForce || tNow - itPacket.value()->m_tStarted > UdpInExpire || itPacket.value()->m_nLeft == 0) )
                {
                    pOldest = itPacket.value();
                }
            }
            else
            {
                if( bForce || tNow - itPacket.value()->m_tStarted > UdpInExpire || itPacket.value()->m_nLeft == 0 )
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
    //QMutexLocker l(&m_pSection);

    //qDebug() << "FLUSHING UDP SEND CACHE";
    //qDebug() << "Thread id: " << QThread::currentThreadId();

    quint32 tNow = time(0);

    /*
    for( QStack<DatagramOut*>::iterator it = m_SendCache.begin(); it != m_SendCache.end(); it++ )
    {
        DatagramOut* pDG = (*it);

        if( tNow - pDG->m_tSent > UdpOutExpire )
        {
            Remove(pDG);    // TODO: powiadomic nasluchujace obiekty ze tego datagramu nie udalo sie dostarczyc
            it = m_SendCache.begin();
        }
    }*/

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
        //for( QStack<DatagramOut*>::iterator it = m_SendCache.begin(); it != m_SendCache.end(); it++ )

        for( int i = m_SendCache.size() - 1; i >= 0; i--)
        {
            //DatagramOut* pDG = (*it);
            DatagramOut* pDG = m_SendCache[i];

            if( pDG->m_oAddress.ip == nLastHost )
                continue;

            if( pDG->GetPacket(tNow, &pPacket, &nPacket, true) )
            {
                qDebug() << "UDP sending to " << pDG->m_oAddress.toString().toAscii().constData() << "seq" << pDG->m_nSequence << "nPart" << ((GND_HEADER*)&pPacket)->nPart << "count" << pDG->m_nCount;

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

        if( tNow - pDG->m_tSent > UdpOutExpire )
        {
            Remove(pDG);
        }
        else
        {
            i++;
        }
    }
    
    /*if( m_SendCache.size() && !m_tSender->isActive() )
    {
        qDebug() << "Starting timer";
        m_tSender->setSingleShot(true);
        m_tSender->start();
    }*/

    /*static bool bTested = false;

    if( !bTested )
    {
        bTested = true;

        for( int i = 0; i < HostCache.m_lHosts.size() && i < 50; i++ )
        {
            G2Packet* pQKR = G2Packet::New("QKR");
            SendPacket(HostCache.m_lHosts[i]->m_oAddress, pQKR, true);
            pQKR->Release();
        }
    }*/

}

void CDatagrams::SendPacket(IPv4_ENDPOINT &oAddr, G2Packet *pPacket, bool bAck, DatagramWatcher *pWatcher, void *pParam)
{
    //QMutexLocker l(&m_pSection);
    //qDebug() << "Locked in SendPacket";

    //if( bLock )
       // m_pSection.lock();

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
            //if( bLock )
                //m_pSection.unlock();
            return;
        }
    }

    DatagramOut* pDG = m_FreeDGOut.pop();
    pDG->Create(oAddr, pPacket, m_nSequence++, m_FreeBuffer.pop(), bAck);

    m_SendCache.push(pDG);
    m_SendCacheMap[pDG->m_nSequence] = pDG;

    // TODO: Powiadomienia do obiektow nasluchujacych, jesli podano

    qDebug() << "UDP queued for " << oAddr.toString().toAscii().constData() << "seq" << pDG->m_nSequence << "parts" << pDG->m_nCount;

    //if( bLock )
       // m_pSection.unlock();

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
            qDebug() << "UDP RECEIVED unknown packet " << pPacket->GetType();
    }
    catch(...)
    {
        qDebug() << "malformed packet";
    }
}

void CDatagrams::OnPing(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
    G2Packet* pNew = G2Packet::New("PO");
    SendPacket(addr, pNew, false);
    pNew->Release();

}
void CDatagrams::OnPong(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
    if( pPacket->HasChildren() )
    {
        while( G2Packet* pChild = pPacket->ReadNextChild() )
        {
            if( pChild->IsType("RELAY") )
            {
                if( !Network.IsConnectedTo(addr) )
                {
                    m_bFirewalled = false;
                }
                return;
            }
        }
    }
}
void CDatagrams::OnCRAWLR(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
    G2Packet* pCA = G2Packet::New("CRAWLA");
    G2Packet* pTmp = pCA->WriteChild("SELF");
    if( Network.isHub() )
        pTmp->WriteChild("HUB");
    else
        pTmp->WriteChild("LEAF");
    pTmp->WriteChild("NA")->WriteHostAddress(&Network.GetLocalAddress());
    pTmp->WriteChild("CV")->WriteString(QString("G2Core/0.1"));
    pTmp->WriteChild("V")->WriteBytes("CORE", 4);
    SendPacket(addr, pCA, true);

    pCA->Release();
}
void CDatagrams::OnQKA(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{
    if( !pPacket->HasChildren() )
        return;

    quint32 nKey = 0;
    quint32 nKeyHost = addr.ip;

    while( G2Packet* pChild = pPacket->ReadNextChild() )
    {
        if( pChild->IsType("QK") && pChild->PayloadLength() >= 4 )
        {
            pChild->ReadIntLE(&nKey);
        }
        else if( pChild->IsType("SNA") && pChild->PayloadLength() >= 4 )
        {
            pChild->ReadIntBE(&nKeyHost);
        }
    }

    CHostCacheHost* pCache = HostCache.Add(addr, 0);
    if( pCache )
        pCache->SetKey(nKey);


    qDebug("Got a query key for %s = 0x%x", addr.toString().toAscii().constData(), nKey);
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

    if( SearchManager.OnQueryAcknowledge(pPacket, addr, oGuid) )
    {
        // TODO: routing
    }

}
void CDatagrams::OnQH2(IPv4_ENDPOINT &addr, G2Packet *pPacket)
{

}
