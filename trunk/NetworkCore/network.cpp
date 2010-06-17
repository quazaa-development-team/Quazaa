#include "network.h"

#include "Thread.h"
#include "webcache.h"
#include "hostcache.h"
#include "g2packet.h"
#include "datagrams.h"
#include <QTimer>
#include <QList>
#include "g2node.h"
#include "NetworkConnection.h"
#include "Handshakes.h"

#include "quazaasettings.h"

#include "queryhashtable.h"
#include "SearchManager.h"
#include "ManagedSearch.h"
#include "Query.h"

#include "geoiplist.h"

CNetwork Network;
CThread NetworkThread;

CNetwork::CNetwork(QObject *parent)
    :QObject(parent)
{
    m_pSecondTimer = 0;
    //m_oAddress.port = 6346;
	m_oAddress.port = quazaaSettings.Connection.Port;

    m_nHubsConnected = 0;
    m_nLeavesConnected = 0;
    m_bNeedUpdateLNI = true;
    m_nLNIWait = 60;
    m_nKHLWait = 60;
    m_tCleanRoutesNext = 60;

	m_nNextCheck = 0;
	m_nBusyPeriods = 0;
	m_nTotalPeriods = 0;

    m_pHashTable = new QueryHashTable(); // do wywalenia

}
CNetwork::~CNetwork()
{
    if( m_bActive )
    {
        Disconnect();
    }

    if( m_pHashTable )
    {
        delete m_pHashTable;
        m_pHashTable = 0;
    }
}

void CNetwork::Connect()
{
    QMutexLocker l(&m_pSection);

    qDebug() << "connect " << QThread::currentThreadId();

    if( m_bActive )
    {
        qDebug() << "Network already started";
        return;
    }

	if( quazaaSettings.Gnutella2.ClientMode < 2 )
		m_nNodeState = G2_LEAF;
	else
		m_nNodeState = G2_HUB;

    m_bActive = true;
	m_oAddress.port = quazaaSettings.Connection.Port;

	m_nNextCheck = quazaaSettings.Gnutella2.AdaptiveCheckPeriod;
	m_nBusyPeriods = 0;
	m_nTotalPeriods = 0;

    Datagrams.moveToThread(&NetworkThread);
    Handshakes.moveToThread(&NetworkThread);
    SearchManager.moveToThread(&NetworkThread);
    m_oRoutingTable.Clear();
    NetworkThread.start(&m_pSection, this);

}
void CNetwork::Disconnect()
{
    QMutexLocker l(&m_pSection);

    qDebug() << "CNetwork::Disconnect() ThreadID:" << QThread::currentThreadId();

    if( m_bActive )
    {
        m_bActive = false;
        NetworkThread.exit(0);
    }

}
void CNetwork::SetupThread()
{
    qWarning("In Network Thread");
    qDebug() << QThread::currentThreadId();

    Q_ASSERT(m_pSecondTimer == 0 && m_pRateController == 0);

    m_pSecondTimer = new QTimer();
	connect(m_pSecondTimer, SIGNAL(timeout()), this, SLOT(OnSecondTimer()));
    m_pSecondTimer->start(1000);

    // Powiedzmy ze mamy lacze 2Mbit/s / 128kbit/s
	quint32 nUploadCapacity = quazaaSettings.Connection.OutSpeed;
	quint32 nDownloadCapacity = quazaaSettings.Connection.InSpeed;

    // Dla polaczen TCP w sieci 1/4 dostepnego pasma
    m_pRateController = new CRateController();
    m_pRateController->setObjectName("CNetwork rate controller");
	m_pRateController->SetDownloadLimit(nDownloadCapacity); // /4
	m_pRateController->SetUploadLimit(nUploadCapacity);	// /4

	Datagrams.Listen();
	Handshakes.Listen();
}
void CNetwork::CleanupThread()
{
    qWarning("Stopping Network Thread");
    qDebug() << "Network ThreadID: " << QThread::currentThreadId();

    m_pSecondTimer->stop();
    delete m_pSecondTimer;
    m_pSecondTimer = 0;
    WebCache.CancelRequests();

    Datagrams.Disconnect();
    Handshakes.Disconnect();

	DisconnectAllNodes();

	delete m_pRateController;
	m_pRateController = 0;

    moveToThread(qApp->thread());
}

void CNetwork::RemoveNode(CG2Node* pNode)
{
    if( pNode->m_nType == G2_HUB )
        m_nHubsConnected--;
    else if( pNode->m_nType == G2_LEAF )
        m_nLeavesConnected--;

    m_pRateController->RemoveSocket(pNode);

    emit NodeRemoved(pNode);
    m_lNodes.removeOne(pNode);
    m_oRoutingTable.Remove(pNode);

    //qDebug() << "List size: " << m_lNodes.size();

}

void CNetwork::OnSecondTimer()
{

	if( !m_pSection.tryLock(150) )
    {
        qWarning() << "WARNING: Network core overloaded!";
        return;
	}

    if( !m_bActive )
    {
		m_pSection.unlock();
        return;
    }

	if( m_nHubsConnected == 0 && !WebCache.isRequesting() && ( HostCache.isEmpty() || HostCache.GetConnectable() == 0 ) )
    {
        WebCache.RequestRandom();
    }

    if( m_tCleanRoutesNext > 0 )
        m_tCleanRoutesNext--;
    else
    {
		//m_oRoutingTable.Dump();
        m_oRoutingTable.ExpireOldRoutes();
        m_tCleanRoutesNext = 60;
    }

	//Datagrams.FlushSendCache();
	emit Datagrams.SendQueueUpdated();
    Handshakes.OnTimer();

	if( isHub() && quazaaSettings.Gnutella2.AdaptiveHub && --m_nNextCheck == 0 )
	{
		AdaptiveHubRun();
		m_nNextCheck = quazaaSettings.Gnutella2.AdaptiveCheckPeriod;
	}

    Maintain();

    SearchManager.OnTimer();

    if( m_nLNIWait == 0 )
    {
        if( m_bNeedUpdateLNI )
        {
			m_nLNIWait = quazaaSettings.Gnutella2.LNIMinimumUpdate;
            m_bNeedUpdateLNI = false;

            foreach( CG2Node* pNode, m_lNodes )
            {
                if( pNode->m_nState == nsConnected )
                    pNode->SendLNI();
            }
        }
    }
    else
        m_nLNIWait--;

    if( m_nKHLWait == 0 )
    {
        HostCache.Save();
        DispatchKHL();
		m_nKHLWait = quazaaSettings.Gnutella2.KHLPeriod;
    }
    else
        m_nKHLWait--;

	m_pSection.unlock();
}

void CNetwork::DisconnectAllNodes()
{
    QListIterator<CG2Node*> it(m_lNodes);
    CG2Node* pNode = 0;
    while( it.hasNext() )
    {
        pNode = it.next();
		RemoveNode(pNode);
        pNode->abort();
		delete pNode;
    }
}
bool CNetwork::NeedMore(G2NodeType nType)
{
    if( nType == G2_HUB ) // potrzeba hubow?
    {
        if( m_nNodeState == G2_HUB ) // jesli hub
			return ( m_nHubsConnected < quazaaSettings.Gnutella2.NumPeers );
        else    // jesli leaf
			return ( m_nLeavesConnected < quazaaSettings.Gnutella2.NumHubs );
    }
    else // potrzeba leaf?
    {
        if( m_nNodeState == G2_HUB )    // jesli hub
			return ( m_nLeavesConnected < quazaaSettings.Gnutella2.NumLeafs );
    }

    return false;
}

void CNetwork::Maintain()
{

    //qDebug() << "CNetwork::Maintain";
    CG2Node* pNode = 0;

    quint32 tNow = time(0);

    QListIterator<CG2Node*> it(m_lNodes);
    while(it.hasNext())
    {
        pNode = it.next();
        pNode->OnTimer(tNow);
    }

    quint32 nHubs = 0, nLeaves = 0, nUnknown = 0;
    quint32 nCoreHubs = 0, nCoreLeaves = 0;

    it.toFront();
    while(it.hasNext())
    {
        pNode = it.next();

        if( pNode->m_nState == nsConnected )
        {
            switch( pNode->m_nType )
            {
            case G2_UNKNOWN:
                nUnknown++;
                break;
            case G2_HUB:
                nHubs++;
                if( pNode->m_bG2Core )
                    nCoreHubs++;
                break;
            case G2_LEAF:
                nLeaves++;
                if( pNode->m_bG2Core )
                    nCoreLeaves++;
            }
        }
        else
        {
            nUnknown++;
        }


    }

	//qDebug("Hubs: %u, Leaves: %u, unknown: %u", nHubs, nLeaves, nUnknown);

    if( m_nHubsConnected != nHubs || m_nLeavesConnected != nLeaves )
        m_bNeedUpdateLNI = true;

    m_nHubsConnected = nHubs;
    m_nLeavesConnected = nLeaves;

    if( m_nNodeState == G2_LEAF )
    {
		if( nHubs > quazaaSettings.Gnutella2.NumHubs )
        {
            // rozlaczyc
            DropYoungest(G2_HUB, (nCoreHubs / nHubs) > 0.5);
        }
		else if( nHubs < quazaaSettings.Gnutella2.NumHubs )
        {
			qint32 nAttempt = qint32((quazaaSettings.Gnutella2.NumHubs - nHubs) * quazaaSettings.Gnutella.ConnectFactor );
            nAttempt = qMin(nAttempt, 8) - nUnknown;

            quint32 tNow = time(0);
			bool bCountry = true;

            for( ; nAttempt > 0; nAttempt-- )
            {
                // nowe polaczenie
				CHostCacheHost* pHost = HostCache.GetConnectable(tNow, (bCountry ? GeoIP.findCountryCode(m_oAddress) : "ZZ"));

                if( pHost )
                {
					ConnectTo(pHost->m_oAddress);
                    pHost->m_tLastConnect = tNow;
                }
                else
				{
					if( !bCountry )
					{
						break;
					}
					else
					{
						bCountry = false;
						nAttempt++;
					}
				}
            }

        }
    }
    else
    {
		if( nHubs > quazaaSettings.Gnutella2.NumPeers )
        {
            // rozlaczyc hub
            DropYoungest(G2_HUB, (nCoreHubs / nHubs) > 0.5);
        }
		else if( nHubs < quazaaSettings.Gnutella2.NumPeers )
        {
			qint32 nAttempt = qint32((quazaaSettings.Gnutella2.NumPeers - nHubs) * quazaaSettings.Gnutella.ConnectFactor );
            nAttempt = qMin(nAttempt, 8) - nUnknown;

            quint32 tNow = time(0);

            for( ; nAttempt > 0; nAttempt-- )
            {
                // nowe polaczenie
                CHostCacheHost* pHost = HostCache.GetConnectable(tNow);

                if( pHost )
                {
					ConnectTo(pHost->m_oAddress);
                    pHost->m_tLastConnect = tNow;
                }
                else
                    break;
            }
        }

		if( nLeaves > quazaaSettings.Gnutella2.NumLeafs )
        {
            DropYoungest(G2_LEAF, (nCoreLeaves / nLeaves) > 0.5);
        }
    }
}

void CNetwork::DispatchKHL()
{
	if( m_lNodes.isEmpty() )
        return;

    G2Packet* pKHL = G2Packet::New("KHL");
    quint32 ts = time(0);
	pKHL->WritePacket("TS", 4)->WriteIntLE(ts);

    foreach(CG2Node* pNode, m_lNodes)
    {
        if( pNode->m_nType == G2_HUB && pNode->m_nState == nsConnected )
        {
			pKHL->WritePacket("NH", 6)->WriteHostAddress(&pNode->m_oAddress);
        }
    }

	quint32 nCount = 0;

	for( ; nCount < (quint32)quazaaSettings.Gnutella2.KHLHubCount && HostCache.size() > nCount; nCount++ )
	{
		pKHL->WritePacket("CH", 6)->WriteHostAddress(&HostCache.m_lHosts.at(nCount)->m_oAddress);
	}


    foreach(CG2Node* pNode, m_lNodes)
    {
        if( pNode->m_nState == nsConnected )
        {
			pNode->SendPacket(pKHL, false, false);
        }
    }
	pKHL->Release();
}

void CNetwork::OnNodeStateChange()
{
    QObject* pSender = QObject::sender();
    if( pSender )
        emit NodeUpdated(qobject_cast<CG2Node*>(pSender));
}

void CNetwork::OnAccept(QTcpSocket* pConn)
{
	if( !m_pSection.tryLock(50) )
	{
		qDebug() << "Not accepting incoming connection. Network overloaded";
		pConn->abort();
		delete pConn;
		return;
	}

    CG2Node* pNew = new CG2Node();
    pNew->moveToThread(&NetworkThread);
    pNew->AttachTo(pConn);
	connect(pNew, SIGNAL(NodeStateChanged()), this, SLOT(OnNodeStateChange()));
    emit NodeAdded(pNew);
    m_pRateController->AddSocket(pNew);
    m_lNodes.append(pNew);
	m_pSection.unlock();
}

bool CNetwork::IsListening()
{
    return Handshakes.isListening() && Datagrams.isListening();
}

bool CNetwork::IsFirewalled()
{
    return Datagrams.IsFirewalled() || Handshakes.IsFirewalled();
}

void CNetwork::DropYoungest(G2NodeType nType, bool bCore)
{
    CG2Node* pNode = 0;

    for( QList<CG2Node*>::iterator i = m_lNodes.begin(); i != m_lNodes.end(); i++ )
    {
        if( (*i)->m_nState == nsConnected )
        {
            if( (*i)->m_nType == nType )
            {
                if( !bCore && (*i)->m_bG2Core )
                    continue;

                if( pNode == 0 )
                {
                    pNode = (*i);
                }
                else
                {
                    if( (*i)->m_tConnected > pNode->m_tConnected )
                        pNode = (*i);
                }
            }
        }
    }

    if( pNode )
        pNode->disconnectFromHost();
}

void CNetwork::AcquireLocalAddress(QString &sHeader)
{
    IPv4_ENDPOINT hostAddr(sHeader + ":0");

    if( hostAddr.ip != 0 )
    {
        m_oAddress.ip = hostAddr.ip;
    }
}
bool CNetwork::IsConnectedTo(IPv4_ENDPOINT addr)
{
    for( int i = 0; i < m_lNodes.size(); i++ )
    {
        if( m_lNodes.at(i)->m_oAddress == addr )
            return true;
    }

    return false;
}

bool CNetwork::RoutePacket(QUuid &pTargetGUID, G2Packet *pPacket)
{
    CG2Node* pNode = 0;
    IPv4_ENDPOINT pAddr;

    if( m_oRoutingTable.Find(pTargetGUID, &pNode, &pAddr) )
    {
        if( pNode )
        {
			pNode->SendPacket(pPacket, true, false);
			qDebug() << "CNetwork::RoutePacket " << pTargetGUID.toString() << " Packet: " << pPacket->GetType() << " routed to neighbour: " << pNode->m_oAddress.toString().toAscii().constData();
            return true;
        }
        else if( pAddr.ip )
        {
            Datagrams.SendPacket(pAddr, pPacket, true);
			qDebug() << "CNetwork::RoutePacket " << pTargetGUID.toString() << " Packet: " << pPacket->GetType() << " routed to remote node: " << pNode->m_oAddress.toString().toAscii().constData();
            return true;
        }
		qDebug() << "CNetwork::RoutePacket - weird thing, should not happen...";
    }

	qDebug() << "CNetwork::RoutePacket " << pTargetGUID.toString() << " Packet: " << pPacket->GetType() << " DROPPED!";
    return false;
}
bool CNetwork::RoutePacket(G2Packet *pPacket, CG2Node *pNbr)
{
	QUuid pGUID;

	if( pPacket->GetTo(pGUID) && pGUID != quazaaSettings.Profile.GUID ) // no i adres != moj adres
    {
        CG2Node* pNode = 0;
        IPv4_ENDPOINT pAddr;

        if( m_oRoutingTable.Find(pGUID, &pNode, &pAddr) )
        {
            bool bForwardTCP = false;
            bool bForwardUDP = false;

            if( pNbr )
            {
                if( pNbr->m_nType == G2_LEAF )  // if received from leaf - can forward anywhere
                {
                    bForwardTCP = bForwardUDP = true;
                }
                else    // if received from a hub - can be forwarded to leaf
                {
                    if( pNode && pNode->m_nType == G2_LEAF )
                    {
                        bForwardTCP = true;
                    }
                }
            }
            else    // received from udp - do not forward via udp
            {
                bForwardTCP = true;
            }

            if( pNode && bForwardTCP )
            {
				pNode->SendPacket(pPacket, true, false);
                return true;
            }
            else if( pAddr.ip && bForwardUDP )
            {
                Datagrams.SendPacket(pAddr, pPacket, true);
                return true;
            }
            // drop
        }
        return true;
	}
    return false;
}

CG2Node* CNetwork::FindNode(quint32 nAddress)
{
	foreach(CG2Node* pNode, m_lNodes)
	{
		if( pNode->m_oAddress.ip == nAddress )
		{
			return pNode;
		}
	}

	return 0;
}

void CNetwork::AdaptiveHubRun()
{
	if( m_nLeavesConnected == 0 )
		return;

	quint32 nBusyLeaves = 0;

	foreach( CG2Node* pNode, m_lNodes)
	{
		if( pNode->m_nState == nsConnected && pNode->m_nType == G2_LEAF && pNode->m_tRTT >= quazaaSettings.Gnutella2.AdaptiveMaxPing )
		{
			nBusyLeaves++;
		}
	}

	if( nBusyLeaves * 100 / m_nLeavesConnected > quazaaSettings.Gnutella2.AdaptiveBusyPercentage )
	{
		m_nBusyPeriods++;
	}
	m_nTotalPeriods++;

	if( m_nTotalPeriods == quazaaSettings.Gnutella2.AdaptiveTimeWindow )
	{
		if( m_nBusyPeriods * 100 / m_nTotalPeriods > quazaaSettings.Gnutella2.AdaptiveBusyPercentage )
		{
			quazaaSettings.Gnutella2.NumLeafs = qMax<quint32>(m_nLeavesConnected / 2, quazaaSettings.Gnutella2.AdaptiveMinimumLeaves);
		}

		m_nTotalPeriods = m_nBusyPeriods = 0;
	}
}

void CNetwork::ConnectTo(IPv4_ENDPOINT &addr)
{
	CG2Node* pNew = new CG2Node();
	connect(pNew, SIGNAL(NodeStateChanged()), this, SLOT(OnNodeStateChange()));
	emit NodeAdded(pNew);
	m_pRateController->AddSocket(pNew);
	m_lNodes.append(pNew);
	pNew->connectToHost(addr);
}

// WARNING: pNode must be a valid pointer
void CNetwork::DisconnectFrom(CG2Node *pNode)
{
	pNode->close();
}
void CNetwork::DisconnectFrom(int index)
{
	if( m_lNodes.size() > index && index >= 0 )
		DisconnectFrom(m_lNodes[index]);
}
void CNetwork::DisconnectFrom(IPv4_ENDPOINT &ip)
{
	for( int i = 0; i < m_lNodes.size(); i++ )
	{
		if( m_lNodes.at(i)->m_oAddress == ip )
		{
			DisconnectFrom(m_lNodes[i]);
			break;
		}
	}
}
