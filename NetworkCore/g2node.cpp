#include "g2node.h"
#include "network.h"
#include "g2packet.h"
#include "hostcache.h"
#include "parser.h"
#include "datagrams.h"
#include "SearchManager.h"
#include "QueryHit.h"

/*CG2Node::CG2Node(QObject *parent) :
    CNetworkConnection(parent)*/
CG2Node::CG2Node(QObject *parent) :
    CCompressedConnection(parent)
{
    m_nState = nsClosed;
    m_nType = G2_UNKNOWN;
    m_tLastPacketIn = m_tLastPacketOut = 0;
    m_tLastPingOut = 0;
    m_nPingsWaiting = 0;
    m_tRTT = 0;
    m_nPacketsIn = m_nPacketsOut = 0;
    m_nLeafCount = m_nLeafMax = 0;
    m_bAcceptDeflate = false;
    m_tLastQuery = 0;
    m_tKeyRequest= 0;
}
CG2Node::~CG2Node()
{
    while( m_lSendQueue.size() )
        m_lSendQueue.dequeue()->Release();

    Network.RemoveNode(this);
}

void CG2Node::connectToHost(IPv4_ENDPOINT oAddress)
{
    m_nState = nsConnecting;
    CNetworkConnection::connectToHost(oAddress);
    SetupSlots();
}

void CG2Node::SendPacket(G2Packet* pPacket, bool bBuffered)
{
    m_nPacketsOut++;

    m_lSendQueue.enqueue(pPacket);

    FlushSendQueue(!bBuffered);
}
void CG2Node::FlushSendQueue(bool bFullFlush)
{
    QByteArray* pOutput = GetOutputBuffer();

    while( bytesToWrite() == 0 && pOutput->size() < 4096 && m_lSendQueue.size() )
    {
        G2Packet* pPacket = m_lSendQueue.dequeue();
        pPacket->ToBuffer(pOutput);
        pPacket->Release();
    }

    if( bFullFlush )
    {
        if( m_bCompressedOutput && pOutput->size() )
            m_bOutputPending = true;
    }

    if( pOutput->size() )
        emit readyToTransfer();
}

void CG2Node::SetupSlots()
{
    connect(this, SIGNAL(connected()), this, SLOT(OnConnect()));
    connect(this, SIGNAL(disconnected()), this, SLOT(OnDisconnect()));
    connect(this, SIGNAL(readyRead()), this, SLOT(OnRead()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(OnStateChange(QAbstractSocket::SocketState)));
}

void CG2Node::OnConnect()
{
    //qDebug("OnConnect()");

    m_nState = nsHandshaking;
    emit NodeStateChanged();

    QByteArray sHs;

    sHs += "GNUTELLA CONNECT/0.6\r\n";
    sHs += "Accept: application/x-gnutella2\r\n";
    sHs += "User-Agent: G2Core/0.1\r\n";
    sHs += "Remote-IP: " + m_oAddress.toStringNoPort() + "\r\n";
    sHs += "Listen-IP: " + Network.GetLocalAddress().toString() + "\r\n";
    if( Network.isHub() )
        sHs += "X-Ultrapeer: True\r\n";
    else
        sHs += "X-Ultrapeer: False\r\n";
    sHs += "Accept-Encoding: deflate\r\n";

    sHs += "\r\n";

    //qDebug() << "Handshake send:\n" << sHs;

    write(sHs);
}
void CG2Node::OnDisconnect()
{
    //qDebug("OnDisconnect()");
    deleteLater();
}
void CG2Node::OnRead()
{
    //qDebug() << "CG2Node::OnRead";
    if( m_nState == nsHandshaking )
    {
        if( peek(bytesAvailable()).indexOf("\r\n\r\n") != -1 )
        {
            if( m_bInitiated )
            {
                ParseOutgoingHandshake();
            }
            else
            {
                ParseIncomingHandshake();
            }
        }
    }
    else if ( m_nState == nsConnected )
    {

        try
        {
            while(!GetInputBuffer()->isEmpty())
            {
                G2Packet* pPacket = G2Packet::FromBuffer(GetInputBuffer());
                m_tLastPacketIn = time(0);
                m_nPacketsIn++;

                //try
                {
                    OnPacket(pPacket);
                }
                /*catch(packet_error)
                {
                    qDebug() << "Packet error in child packets!";
                    throw;
                }
                catch(packet_incomplete)
                {
                    qDebug() << "Incomplete packet in child packets!";
                    throw packet_error();
                }
                catch(stream_end)
                {
                    qDebug() << "Tego tu nie powinno byc...";
                }*/

                pPacket->Release();
            }
        }
        catch(packet_error)
        {
            qDebug() << "Packet error - " << m_oAddress.toString().toAscii();
            m_nState = nsClosing;
            emit NodeStateChanged();
            deleteLater();
            return;
        }
        catch(stream_end)
        {
            qDebug() << "Stream end in root, closing" << m_oAddress.toString().toAscii() << m_pInput->size();
            m_nState = nsClosing;
            emit NodeStateChanged();
            deleteLater();
            return;
        }
        catch(packet_incomplete)
        {
            //
        }

    }
}
void CG2Node::OnError(QAbstractSocket::SocketError e)
{
    if( e != QAbstractSocket::RemoteHostClosedError )
    {
        HostCache.OnFailure(m_oAddress);
    }


    //qDebug() << "OnError(" << e << ")" << m_oAddress.toString();
    deleteLater();
}
void CG2Node::OnStateChange(QAbstractSocket::SocketState s)
{
    //qDebug() << "OnStateChange(" << s << ")";
}

void CG2Node::OnTimer(quint32 tNow)
{

    if( m_nState < nsConnected )
    {
        if( tNow - m_tConnected > 15 )
        {
            if( m_bInitiated )
                HostCache.OnFailure(m_oAddress);
            m_nState = nsClosing;
            disconnectFromHost();
            return;
        }
    }
    else if( m_nState == nsConnected )
    {
        if( tNow - m_tLastPacketIn > 60 )
        {
            qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "minute dead";
            m_nState = nsClosing;
            emit NodeStateChanged();
            deleteLater();
            return;
        }

        if( m_nPingsWaiting == 0 && (tNow - m_tLastPacketIn >= 30 || tNow - m_tLastPingOut >= 120) )
        {
            // Jesli dostalismy ostatni pakiet co najmniej 30 sekund temu
            // lub wyslalismy ostatniego pinga co najmniej 2 minuty temu
            // no i nie oczekujemy odpowiedzi na wczesniejszego pinga
            // to wysylamy keep-alive ping, przy okazji merzac RTT
            G2Packet* pPacket = G2Packet::New("PI");
            SendPacket(pPacket, false); // niebuforowany, zeby dokladniej zmierzyc RTT
            m_nPingsWaiting++;
            m_tLastPingOut = tNow;
            m_tRTTTimer.start();
        }

        if( m_nPingsWaiting > 0 && tNow - m_tLastPingOut > 120 && tNow - m_tLastPacketIn > 60 )
        {
            qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "ping timed out";
            m_nState = nsClosing;
            emit NodeStateChanged();
            deleteLater();
            return;
        }

        if( m_tKeyRequest > 0 && tNow - m_tKeyRequest > 90 )
        {
            qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "QueryKey wait timeout reached";
            m_nState = nsClosing;
            emit NodeStateChanged();
            deleteLater();
            return;
        }

        FlushSendQueue(true);
    }
    else if( m_nState == nsClosing )
    {
        if( m_nType == G2_UNKNOWN && tNow - m_tConnected > 20 )
        {
            deleteLater();
        }
        else if( tNow - m_tLastPacketIn > 20)
        {
            deleteLater();
        }
    }
}

void CG2Node::ParseIncomingHandshake()
{
    qint32 nIndex = peek(bytesAvailable()).indexOf("\r\n\r\n");
    QString sHs = read(nIndex + 4);

    //qDebug() << "Handshake receive:\n" << sHs;

    if( m_sUserAgent.isEmpty() )
        m_sUserAgent = Parser::GetHeaderValue(sHs, "User-Agent");

    if( m_sUserAgent.isEmpty() )
    {
        Send_ConnectError("503 Anonymous clients are not allowed here");
        return;
    }


    if( sHs.startsWith("GNUTELLA CONNECT/0.6") )
    {
        QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
        bool bAcceptG2 = sAccept.contains("application/x-gnutella2");

        if( !bAcceptG2 )
        {
            Send_ConnectError("503 Required network not accepted");
            return;
        }

        m_bAcceptDeflate = false;
        QString sAcceptEnc = Parser::GetHeaderValue(sHs, "Accept-Encoding");
        if( sAcceptEnc.contains("deflate") && Network.isHub() )
        {
            m_bAcceptDeflate = true;
        }

        QString sUltra = Parser::GetHeaderValue(sHs, "X-Ultrapeer").toLower();
        //QString sUltraNeeded = Parser::GetHeaderValue(sHs, "X-Ultrapeer-Needed").toLower();
        if( sUltra.isEmpty() )
        {
            Send_ConnectError("503 No hub mode specified");
            return;
        }

        QString sRemoteIP = Parser::GetHeaderValue(sHs, "Remote-IP");
        if( !sRemoteIP.isEmpty() )
            Network.AcquireLocalAddress(sRemoteIP);
        else
        {
            Send_ConnectError("503 Remote-IP header missing");
            return;
        }

        bool bUltra = (sUltra == "true");

        if( bUltra )
        {
            if( !Network.NeedMore(G2_HUB) )
            {
                Send_ConnectError("503 Maximum hub connections reached");
                return;
            }
            m_nType = G2_HUB;
        }
        else
        {
            if( !Network.NeedMore(G2_LEAF) )
            {
                Send_ConnectError("503 Maximum leaf connections reached");
                return;
            }
            m_nType = G2_LEAF;
        }

        Send_ConnectOK(false, m_bAcceptDeflate);

    }
    else if( sHs.contains(" 200 OK") )
    {
        QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
        bool bG2Provided = sContentType.contains("application/x-gnutella2");

        if( !bG2Provided )
        {
            Send_ConnectError("503 Required network not provided");
            return;
        }

        QString sContentEnc = Parser::GetHeaderValue(sHs, "Content-Encoding");
        if( sContentEnc.contains("deflate") )
        {
            if( !EnableInputCompression() )
            {
                qDebug() << "Inflate init error!";
                abort();
                deleteLater();
                return;
            }
        }

        if( m_bAcceptDeflate )
        {
            if( !EnableOutputCompression() )
            {
                qDebug() << "Deflate init error!";
                abort();
                deleteLater();
                return;
            }
        }

        m_nState = nsConnected;
        emit NodeStateChanged();

        SendStartups();
        m_tLastPacketIn = m_tLastPacketOut = time(0);


    }
    else
    {
        qDebug() << "Connection rejected: " << sHs.left(sHs.indexOf("\r\n"));
        m_nState = nsClosing;
        emit NodeStateChanged();
        close();
    }
}

void CG2Node::ParseOutgoingHandshake()
{
    QString sHs = read(peek(bytesAvailable()).indexOf("\r\n\r\n") + 4);

    //qDebug() << "Handshake receive:\n" << sHs;

    QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
    bool bAcceptG2 = sAccept.contains("application/x-gnutella2");

    if( !bAcceptG2 )
    {
        Send_ConnectError("503 Required network not accepted");
        return;
    }

    QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
    bool bG2Provided = sContentType.contains("application/x-gnutella2");

    if( !bG2Provided )
    {
        Send_ConnectError("503 Required network not provided");
        return;
    }

    QString sTry = Parser::GetHeaderValue(sHs, "X-Try-Hubs");
    if( bAcceptG2 && bG2Provided && sTry.size() )
    {
        HostCache.AddXTry(sTry);
    }

    QString sRemoteIP = Parser::GetHeaderValue(sHs, "Remote-IP");
    if( !sRemoteIP.isEmpty() )
        Network.AcquireLocalAddress(sRemoteIP);
    else
    {
        Send_ConnectError("503 Remote-IP header missing");
        return;
    }

    if( sHs.left(16) != "GNUTELLA/0.6 200" )
    {
        qDebug() << "Connection rejected: " << sHs.left(sHs.indexOf("\r\n"));
        disconnectFromHost();
        return;
    }

    m_sUserAgent = Parser::GetHeaderValue(sHs, "User-Agent");

    if( m_sUserAgent.isEmpty() )
    {
        Send_ConnectError("503 Anonymous clients are not allowed here");
        return;
    }

    QString sUltra = Parser::GetHeaderValue(sHs, "X-Ultrapeer").toLower();
    //QString sUltraNeeded = Parser::GetHeaderValue(sHs, "X-Ultrapeer-Needed").toLower();

    bool bUltra = (sUltra == "true");
    //bool bUltraNeeded = (sUltraNeeded == "true");

    QString sContentEnc = Parser::GetHeaderValue(sHs, "Content-Encoding");
    if( sContentEnc.contains("deflate") )
    {
        if( !EnableInputCompression() )
        {
            qDebug() << "Inflate init error!";
            abort();
            deleteLater();
            return;
        }
    }

    bool bAcceptDeflate = false;
    QString sAcceptEnc = Parser::GetHeaderValue(sHs, "Accept-Encoding");
    if( sAcceptEnc.contains("deflate") && Network.isHub() )
    {
        bAcceptDeflate = true;
    }

    if( bUltra )
    {
        if( !Network.NeedMore(G2_HUB) )
        {
            Send_ConnectError("503 Maximum hub connections reached");
            return;
        }
        m_nType = G2_HUB;
    }
    else
    {
        if( !Network.NeedMore(G2_LEAF) )
        {
            Send_ConnectError("503 Maximum leaf connections reached");
            return;
        }
        m_nType = G2_LEAF;
    }

    Send_ConnectOK(true, bAcceptDeflate);

    if( bAcceptDeflate )
    {
        if( !EnableOutputCompression() )
        {
            qDebug() << "Deflate init error!";
            abort();
            deleteLater();
            return;
        }
    }

    m_nState = nsConnected;
    emit NodeStateChanged();

    SendStartups();
    m_tLastPacketIn = m_tLastPacketOut = time(0);

}
void CG2Node::Send_ConnectError(QString sReason)
{
    qDebug() << "Rejecting connection:" << sReason;

    QByteArray sHs;

    sHs += "GNUTELLA/0.6 " + sReason + "\r\n";
    sHs += "User-Agent: G2Core/0.1\r\n";
    sHs += "Accept: application/x-gnutella2\r\n";
    sHs += "Content-Type: application/x-gnutella2\r\n";
    sHs += HostCache.GetXTry();
    sHs += "\r\n";

    //qDebug() << "Handshake send:\n" << sHs;
    write(sHs);
    flush();

    disconnectFromHost();
}
void CG2Node::Send_ConnectOK(bool bReply, bool bDeflated)
{
    QByteArray sHs;

    sHs += "GNUTELLA/0.6 200 OK\r\n";
    sHs += "User-Agent: G2Core/0.1\r\n";
    if( Network.isHub() )
        sHs += "X-Ultrapeer: True\r\n";
    else
        sHs += "X-Ultrapeer: False\r\n";
    sHs += "Content-Type: application/x-gnutella2\r\n";

    if( !bReply )
    {
        // 2-handshake
        if( Network.isHub() && m_nType == G2_HUB )
            sHs += "Accept-Encoding: deflate\r\n";

        if( bDeflated )
        {
            sHs += "Content-Encoding: deflate\r\n";
        }
        sHs += "Accept: application/x-gnutella2\r\n";
        sHs += "Remote-IP: ";
        sHs += m_oAddress.toStringNoPort();
        sHs += "\r\n";
        sHs += "Listen-IP: " + Network.GetLocalAddress().toString() + "\r\n";
    }
    else
    {
        if( bDeflated )
            sHs += "Content-Encoding: deflate\r\n";
    }

    sHs += "\r\n";

    //qDebug() << "Handshake send:\n" << sHs;

    write(sHs);

}

void CG2Node::SendStartups()
{
    if( Network.IsListening() )
    {
        IPv4_ENDPOINT addr = Network.GetLocalAddress();
        G2Packet* pPacket = G2Packet::New("PI");
        pPacket->WriteChild("UDP")->WriteHostAddress(&addr);
        pPacket->WriteChild("TFW");
        SendPacket(pPacket, false);
        qDebug() << "Sent /PI/UDP";
    }

    SendLNI();
}

void CG2Node::SendLNI()
{
    G2Packet* pLNI = G2Packet::New("LNI");
    G2Packet* pTmp  = pLNI->WriteChild("NA");
    pTmp->WriteHostAddress(&Network.m_oAddress);
    pTmp = pLNI->WriteChild("GU");
    pTmp->WriteBytes("xxxxxxxxxxxxxxxx", 16);

    if( Network.isHub() )
    {
        G2Packet* pHS = pLNI->WriteChild("HS");
        pHS->WriteIntLE(Network.m_nLeavesConnected);
        pHS->WriteIntLE(HubToLeaf);
    }

    pLNI->WriteChild("g2core");

    SendPacket(pLNI, true);
}


void CG2Node::OnPacket(G2Packet* pPacket)
{
    try
    {
        if( !Network.RoutePacket(pPacket) )
        {

            if( pPacket->IsType("PI") )
            {
                OnPing(pPacket);
            }
            else if( pPacket->IsType("PO") )
            {
                OnPong(pPacket);
            }
            else if( pPacket->IsType("LNI") )
            {
                OnLNI(pPacket);
            }
            else if( pPacket->IsType("KHL") )
            {
                OnKHL(pPacket);
            }
            else if( pPacket->IsType("QHT") )
            {
                //OnQHT(pPacket);
            }
            else if( pPacket->IsType("Q2") )
            {

                /*qDebug() << "Q2!";
                while( G2Packet* pChild = pPacket->ReadNextChild() )
                {
                    if( pChild->IsType("URN") )
                    {
                        char buff[256];
                        pChild->ReadBytes((void*)&buff, qMin(quint32(256), pChild->PayloadLength()));
                        qDebug() << "Q2 URN " << buff;
                    }
                    else if( pChild->IsType("DN") )
                    {
                        char buff[256];
                        pChild->ReadBytes((void*)&buff, qMin(quint32(256), pChild->PayloadLength()));
                        qDebug() << "Q2 DN " << buff;
                    }
                }*/

            }
            else if( pPacket->IsType("QKA") )
            {
                OnQKA(pPacket);
            }
            else if( pPacket->IsType("QA") )
            {
                OnQA(pPacket);
            }
            else if( pPacket->IsType("QH2") )
            {
                OnQH2(pPacket);
            }
            else
            {
                qDebug() << "Unknown packet " << pPacket->GetType();
            }
        }
    }
    catch(...)
    {
        qDebug() << "Packet error in child packets";
    }
}

void CG2Node::OnPing(G2Packet* pPacket)
{
   // return;
    bool bUdp = false;
    bool bRelay = false;
    bool bTestFirewall = false;
    IPv4_ENDPOINT addr;

    if( pPacket->HasChildren() )
    {
        while( G2Packet* pChild = pPacket->ReadNextChild() )
        {
            if( pChild->IsType("UDP") )
            {
                pChild->ReadHostAddress(&addr);
                if( addr.ip != 0 && addr.port != 0)
                    bUdp = true;
            }
            else if( pChild->IsType("RELAY") )
            {
                bRelay = true;
            }
            else if( pChild->IsType("TFW") )
            {
                bTestFirewall = true;
            }
        }
    }

    if( !bUdp && !bRelay )
    {
        // direct ping
        G2Packet* pPong = G2Packet::New("PO");
        SendPacket(pPong, false);
        return;
    }

    if( bUdp && !bRelay )
    {
        // /PI/UDP

        if( Network.isHub() )
        {
            pPacket->WriteChild("RELAY");

            QList<CG2Node*> lToRelay;

            for( int i = 0; i < Network.m_lNodes.size(); i++ )
            {
                if( Network.m_lNodes.at(i)->m_nState == nsConnected )
                    lToRelay.append(Network.m_lNodes[i]);
            }

            for( int nCount = 0; nCount < 10 && lToRelay.size(); nCount++ )
            {
                int nIndex = qrand() % lToRelay.size();
                pPacket->AddRef();
                CG2Node* pNode = lToRelay.at(nIndex);
                pNode->SendPacket(pPacket, false);
                lToRelay.removeAt(nIndex);
            }

            return;
        }
    }

    if( bUdp && bRelay )
    {
        G2Packet* pPong = G2Packet::New("PO");
        pPong->WriteChild("RELAY");
        Datagrams.SendPacket(addr, pPong, true);
        pPong->Release();
    }
}
void CG2Node::OnPong(G2Packet* pPacket)
{
   if( m_nPingsWaiting > 0 )
   {
       m_nPingsWaiting--;

       if( m_nPingsWaiting == 0 )
       {
            m_tRTT = m_tRTTTimer.elapsed();
       }
   }
}

void CG2Node::OnLNI(G2Packet* pPacket)
{
    if( !pPacket->HasChildren() )
        return;

    bool hasNA = false;
    QUuid pGUID;
    bool hasGUID = false;

    IPv4_ENDPOINT hostAddr;

    while( G2Packet* pChild = pPacket->ReadNextChild() )
    {
        if( pChild->IsType("NA") && pChild->PayloadLength() >= 6 )
        {
            pChild->ReadHostAddress(&hostAddr);
            if( hostAddr.ip != 0 && hostAddr.port != 0 )
            {
                hasNA = true;
                if( !m_bInitiated )
                    m_oAddress.ip = hostAddr.ip;
                m_oAddress.port = hostAddr.port;
            }

        }
        else if( pChild->IsType("GU") && pChild->PayloadLength() >= 16 )
        {
            hasGUID = true;
            pGUID = pChild->ReadGUID();
        }
        else if( pChild->IsType("HS") && pChild->PayloadLength() >= 4 )
        {
            if( m_nType == G2_HUB )
            {
                pChild->ReadIntLE(&m_nLeafCount);
                pChild->ReadIntLE(&m_nLeafMax);
            }
        }
        else if( pChild->IsType("g2core") )
        {
            m_bG2Core = true;
        }
    }

    if( hasNA && hasGUID )
    {
        Network.m_oRoutingTable.Add(pGUID, this, true);
    }

}
void CG2Node::OnKHL(G2Packet* pPacket)
{
    if( !pPacket->HasChildren() )
        return;

    quint32 tNow = time(0);
    quint32 nTimestamp = tNow;
    qint64 nDiff = 0;

    while( G2Packet* pChild = pPacket->ReadNextChild() )
    {
        if( pChild->IsType("NH") )
        {
            QUuid pGUID;

            while( G2Packet* pInner = pChild->ReadNextChild() )
            {
                if( pInner->IsType("GU") && pInner->PayloadLength() >= 16 )
                {
                    pGUID = pInner->ReadGUID();
                }
            }

            if( !pGUID.isNull() && pChild->PayloadLength() >= 6 )
            {
                IPv4_ENDPOINT pAddr;
                pChild->ReadHostAddress(&pAddr);

                Network.m_oRoutingTable.Add(pGUID, this, &pAddr, false);
            }
        }
        else if( pChild->IsType("CH") && pChild->PayloadLength() >= 10 )
        {
            IPv4_ENDPOINT ip4;
            quint32 nTs = 0;
            pChild->ReadHostAddress(&ip4);
            pChild->ReadIntLE(&nTs);

            HostCache.Add(ip4, tNow + nDiff);
        }
        else if( pChild->IsType("TS") && pChild->PayloadLength() >= 4 )
        {
            pChild->ReadIntLE(&nTimestamp);
            nDiff = tNow - nTimestamp;
        }
    }
}

void CG2Node::OnQHT(G2Packet* pPacket)
{
    if( !Network.isHub() )
    {
        qDebug() << "Received unexpected Query Routing Table, ignoring";
        return;
    }
}

void CG2Node::OnQKA(G2Packet *pPacket)
{
    if( !pPacket->HasChildren() )
        return;

    m_tKeyRequest = 0;

    quint32 nKey;
    IPv4_ENDPOINT addr;

    while( G2Packet* pChild = pPacket->ReadNextChild() )
    {
        if( pChild->IsType("QK") && pChild->PayloadLength() >= 4 )
        {
            pChild->ReadIntLE(&nKey);
        }
        else if( pChild->IsType("QNA") )
        {
            if( pChild->PayloadLength() >= 6 )
            {
                pChild->ReadHostAddress(&addr);
            }
            else if( pChild->PayloadLength() >= 4 )
            {
                pChild->ReadIntBE(&addr.ip);
                addr.port = 6346;
            }
        }
    }

    CHostCacheHost* pCache = HostCache.Add(addr, 0);
    if( pCache )
    {
        pCache->SetKey(nKey, &m_oAddress);

        qDebug("Got a query key from %s via %s = 0x%x", addr.toString().toAscii().constData(), m_oAddress.toString().toAscii().constData(), nKey);
    }
}
void CG2Node::OnQA(G2Packet *pPacket)
{
    QUuid oGUID;

    if( SearchManager.OnQueryAcknowledge(pPacket, m_oAddress, oGUID) )
    {
        Network.RoutePacket(oGUID, pPacket);
    }
}

void CG2Node::OnQH2(G2Packet *pPacket)
{
    QueryHitSharedPtr pHit( CQueryHit::ReadPacket(pPacket) );
    //CQueryHit* pHit = CQueryHit::ReadPacket(pPacket);

    if( !pHit )
    {
        qDebug() << "Received malformatted query hit packet";
        return;
    }

    qDebug() << "Received QueryHit:";
    CQueryHit* pHit2 = pHit.data();
    qDebug() << "Search GUID:" << pHit2->m_pHitInfo->m_oGUID.toString().toAscii();
    qDebug() << "Node GUID:" << pHit2->m_pHitInfo->m_oNodeGUID.toString().toAscii();
    qDebug() << "Node address: " << pHit2->m_pHitInfo->m_oNodeAddress.toString().toAscii();
    qDebug() << "Hops:" << pHit2->m_pHitInfo->m_nHops;
    for( int i = 0; i < pHit2->m_pHitInfo->m_lNeighbouringHubs.size(); i++ )
    {
        qDebug() << "Neighbouring hub: " << pHit2->m_pHitInfo->m_lNeighbouringHubs[i].toString().toAscii();
    }

    while( pHit2 != 0 )
    {
        qDebug() << "Descriptive name: " << pHit2->m_sDescriptiveName;
        qDebug() << "URL:" << pHit2->m_sURL;
        qDebug() << "Size: " << pHit2->m_nObjectSize;
        qDebug() << "Is partial: " << pHit2->m_bIsPartial;
        qDebug() << "Partial bytes avail:" << pHit2->m_nPartialBytesAvailable;
        qDebug() << "Cached sources:" << pHit2->m_nCachedSources;
        qDebug() << "URN:" << pHit2->m_oSha1.ToURN();


        qDebug() << "Metadata: " << pHit2->m_sMetadata;

        pHit2 = pHit2->m_pNext;
    }
    qDebug() << "---------------------------------";

    SearchManager.OnQueryHit(pHit);
    //pHit->Delete();
}
