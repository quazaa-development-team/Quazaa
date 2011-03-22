/*
** network.cpp
**
** Copyright © Quazaa Development Team, 2009-2011.
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

#include "network.h"

#include "thread.h"
#include "webcache.h"
#include "hostcache.h"
#include "g2packet.h"
#include "datagrams.h"
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <QList>
#include "g2node.h"
#include "networkconnection.h"
#include "handshakes.h"
#include "neighbours.h"

#include "quazaasettings.h"

#include "queryhashtable.h"
#include "queryhashmaster.h"
#include "searchmanager.h"
#include "sharemanager.h"
#include "managedsearch.h"
#include "query.h"
#include "queryhit.h"

#include "geoiplist.h"

CNetwork Network;
CThread NetworkThread;

CNetwork::CNetwork(QObject* parent)
	: QObject(parent)
{
	m_pSecondTimer = 0;
	//m_oAddress.port = 6346;
	m_oAddress.setPort ( quazaaSettings.Connection.Port );

	m_bNeedUpdateLNI = true;
	m_nLNIWait = 60;
	m_nKHLWait = 60;
	m_tCleanRoutesNext = 60;

	m_nNextCheck = 0;
	m_nBusyPeriods = 0;
	m_nTotalPeriods = 0;

	m_bSharesReady = false;

	m_nSecsToHubBalancing = 0;
	m_tLastModeChange = 0;
	m_nMinutesAbove90 = m_nMinutesBelow50 = 0;
	m_nMinutesTrying = 0;

	m_bPacketsPending = false;

}
CNetwork::~CNetwork()
{
	if(m_bActive)
	{
		Disconnect();
	}
}

void CNetwork::Connect()
{
	QMutexLocker l(&m_pSection);

	if(m_bActive)
	{
		systemLog.postLog(LogSeverity::Debug, "Network already started");
		//qDebug() << "Network already started";
		return;
	}

	if(quazaaSettings.Gnutella2.ClientMode < 2)
	{
		m_nNodeState = G2_LEAF;
	}
	else
	{
		m_nNodeState = G2_HUB;
	}

	m_bActive = true;
	m_oAddress.setPort(quazaaSettings.Connection.Port);

	m_nNextCheck = quazaaSettings.Gnutella2.AdaptiveCheckPeriod;
	m_nBusyPeriods = 0;
	m_nTotalPeriods = 0;

	Handshakes.Listen();

	m_oRoutingTable.Clear();

	m_nSecsToHubBalancing = HUB_BALANCING_INTERVAL;
	m_tLastModeChange = time(0);
	m_nMinutesAbove90 = m_nMinutesBelow50 = 0;
	m_nMinutesTrying = 0;

	connect(&ShareManager, SIGNAL(sharesReady()), this, SLOT(OnSharesReady()), Qt::UniqueConnection);
	connect(&Datagrams, SIGNAL(PacketQueuedForRouting()), this, SLOT(RoutePackets()), Qt::QueuedConnection);

	NetworkThread.start("Network", &m_pSection, this);

	Datagrams.moveToThread(&NetworkThread);

	SearchManager.moveToThread(&NetworkThread);
	Neighbours.moveToThread(&NetworkThread);
	Neighbours.Connect();

}
void CNetwork::Disconnect()
{
	QMutexLocker l(&m_pSection);

	if(m_bActive)
	{
		m_bActive = false;
		disconnect(&Datagrams, SIGNAL(PacketQueuedForRouting()));
		NetworkThread.exit(0);
	}

}
void CNetwork::SetupThread()
{
	Q_ASSERT(m_pSecondTimer == 0);

	m_pSecondTimer = new QTimer();
	connect(m_pSecondTimer, SIGNAL(timeout()), this, SLOT(OnSecondTimer()));
	m_pSecondTimer->start(1000);

	Datagrams.Listen();
	Handshakes.Listen();

	m_bSharesReady = ShareManager.SharesReady();
}
void CNetwork::CleanupThread()
{
	m_pSecondTimer->stop();
	delete m_pSecondTimer;
	m_pSecondTimer = 0;
	WebCache.CancelRequests();

	Handshakes.Disconnect();
	Datagrams.Disconnect();
	Neighbours.Disconnect();

	moveToThread(qApp->thread());
}


void CNetwork::OnSecondTimer()
{
	if(!m_pSection.tryLock(150))
	{
		systemLog.postLog(LogSeverity::Warning, tr("WARNING: Network core overloaded!"));
		//qWarning() << "WARNING: Network core overloaded!";
		return;
	}

	if(!m_bActive)
	{
		m_pSection.unlock();
		return;
	}

	if(Neighbours.m_nHubsConnected == 0 && !WebCache.isRequesting() && (HostCache.isEmpty() || HostCache.GetConnectable() == 0 || m_nMinutesTrying == 1))
	{
		WebCache.RequestRandom();
	}

	if(m_tCleanRoutesNext > 0)
	{
		m_tCleanRoutesNext--;
	}
	else
	{
		//m_oRoutingTable.Dump();
		m_oRoutingTable.ExpireOldRoutes();
		m_tCleanRoutesNext = 60;
	}

	if(isHub() && quazaaSettings.Gnutella2.AdaptiveHub && --m_nNextCheck == 0)
	{
		AdaptiveHubRun();
		m_nNextCheck = quazaaSettings.Gnutella2.AdaptiveCheckPeriod;
	}

	if(!QueryHashMaster.IsValid())
	{
		QueryHashMaster.Build();
	}

	Neighbours.Maintain();

	m_nSecsToHubBalancing--;

	if(m_nSecsToHubBalancing == 0)
	{
		m_nSecsToHubBalancing = HUB_BALANCING_INTERVAL;
		HubBalancing();
	}

	SearchManager.OnTimer();

	if(m_bPacketsPending)
	{
		RoutePackets();
	}

	if(m_nLNIWait == 0)
	{
		if(m_bNeedUpdateLNI)
		{
			QMutexLocker l(&Neighbours.m_pSection);

			m_bNeedUpdateLNI = false;

			for(QList<CNeighbour*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode)
			{
				CNeighbour* pNode = *itNode;

				if( pNode->m_nProtocol != dpGnutella2 )
					continue;

				if(pNode->m_nState == nsConnected)
				{
					((CG2Node*)pNode)->SendLNI();
				}
			}
		}

		m_nLNIWait = quazaaSettings.Gnutella2.LNIMinimumUpdate;
	}
	else
	{
		m_nLNIWait--;
	}

	if(m_nKHLWait == 0)
	{
		HostCache.m_pSection.lock();
		HostCache.Save();
		DispatchKHL();
		m_nKHLWait = quazaaSettings.Gnutella2.KHLPeriod;
		HostCache.m_pSection.unlock();
	}
	else
	{
		m_nKHLWait--;
	}

	m_pSection.unlock();

	emit Datagrams.SendQueueUpdated();
}

void CNetwork::DispatchKHL()
{
	QMutexLocker l(&Neighbours.m_pSection);

	if(Neighbours.m_lNodes.isEmpty())
	{
		return;
	}

	G2Packet* pKHL = G2Packet::New("KHL");
	quint32 ts = time(0);
	pKHL->WritePacket("TS", 4)->WriteIntLE(ts);

	for(QList<CNeighbour*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode)
	{
		CNeighbour* pNode = *itNode;

		if( pNode->m_nProtocol != dpGnutella2 )
				continue;

		if(((CG2Node*)pNode)->m_nType == G2_HUB && pNode->m_nState == nsConnected)
		{
			pKHL->WritePacket("NH", 6)->WriteHostAddress(&pNode->m_oAddress);
		}
	}

	quint32 nCount = 0;

	for(; nCount < (quint32)quazaaSettings.Gnutella2.KHLHubCount && HostCache.size() > nCount; nCount++)
	{
		pKHL->WritePacket("CH", 10)->WriteHostAddress(&HostCache.m_lHosts.at(nCount)->m_oAddress);
		pKHL->WriteIntLE(&HostCache.m_lHosts.at(nCount)->m_tTimestamp);
	}


	for(QList<CNeighbour*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode)
	{
		CNeighbour* pNode = *itNode;

		if( pNode->m_nProtocol != dpGnutella2 )
			continue;

		if(pNode->m_nState == nsConnected)
		{
			((CG2Node*)pNode)->SendPacket(pKHL, false, false);
		}
	}
	pKHL->Release();
}

bool CNetwork::IsListening()
{
	return Handshakes.isListening() && Datagrams.isListening();
}

bool CNetwork::IsFirewalled()
{
	return Datagrams.IsFirewalled() || Handshakes.IsFirewalled();
}

void CNetwork::AcquireLocalAddress(QString& sHeader)
{
	CEndPoint hostAddr(sHeader + ":0");

	if(!hostAddr.isNull())
	{
		m_oAddress.setAddress(sHeader);
	}
}
bool CNetwork::IsConnectedTo(CEndPoint addr)
{
	for(int i = 0; i < Neighbours.m_lNodes.size(); i++)
	{
		if(Neighbours.m_lNodes.at(i)->m_oAddress == addr)
		{
			return true;
		}
	}

	return false;
}

bool CNetwork::RoutePacket(QUuid& pTargetGUID, G2Packet* pPacket)
{
	CG2Node* pNode = 0;
	CEndPoint pAddr;

	if(m_oRoutingTable.Find(pTargetGUID, &pNode, &pAddr))
	{
		if(pNode)
		{
			pNode->SendPacket(pPacket, true, false);
			systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket %1 Packet: %2 routed to neighbour: %3").arg(pTargetGUID.toString()).arg(pPacket->GetType()).arg(pNode->m_oAddress.toString().toAscii().constData()));
			//qDebug() << "CNetwork::RoutePacket " << pTargetGUID.toString() << " Packet: " << pPacket->GetType() << " routed to neighbour: " << pNode->m_oAddress.toString().toAscii().constData();
			return true;
		}
		else if(!pAddr.isNull())
		{
			Datagrams.SendPacket(pAddr, pPacket, true);
			systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket %1 Packet: %2 routed to remote node: %3").arg(pTargetGUID.toString()).arg(pPacket->GetType()).arg(pNode->m_oAddress.toString().toAscii().constData()));
			//qDebug() << "CNetwork::RoutePacket " << pTargetGUID.toString() << " Packet: " << pPacket->GetType() << " routed to remote node: " << pNode->m_oAddress.toString().toAscii().constData();
			return true;
		}
		systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket - No node and no address!"));
		//qDebug() << "CNetwork::RoutePacket - weird thing, should not happen...";
	}

	systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket %1 Packet: %2 DROPED!").arg(pTargetGUID.toString()).arg(pPacket->GetType()));
	//qDebug() << "CNetwork::RoutePacket " << pTargetGUID.toString() << " Packet: " << pPacket->GetType() << " DROPPED!";
	return false;
}
bool CNetwork::RoutePacket(G2Packet* pPacket, CG2Node* pNbr)
{
	QUuid pGUID;

	if(pPacket->GetTo(pGUID) && pGUID != quazaaSettings.Profile.GUID)   // no i adres != moj adres
	{
		CG2Node* pNode = 0;
		CEndPoint pAddr;

		if(m_oRoutingTable.Find(pGUID, &pNode, &pAddr))
		{
			bool bForwardTCP = false;
			bool bForwardUDP = false;

			if(pNbr)
			{
				if(pNbr->m_nType == G2_LEAF)    // if received from leaf - can forward anywhere
				{
					bForwardTCP = bForwardUDP = true;
				}
				else    // if received from a hub - can be forwarded to leaf
				{
					if(pNode && pNode->m_nType == G2_LEAF)
					{
						bForwardTCP = true;
					}
				}
			}
			else    // received from udp - do not forward via udp
			{
				bForwardTCP = true;
			}

			if(pNode && bForwardTCP)
			{
				pNode->SendPacket(pPacket, true, false);
				return true;
			}
			else if(!pAddr.isNull() && bForwardUDP)
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


void CNetwork::AdaptiveHubRun()
{
	/*if( m_nLeavesConnected == 0 )
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
	}*/
}

void CNetwork::ConnectTo(CEndPoint& addr)
{
	// TODO: Verify network is connected before attempting connection and create connection if it is not
	/*CG2Node* pNew = Neighbours.ConnectTo(addr);

	if(pNew)
	{
		pNew->moveToThread(&NetworkThread);
	}*/
}

// WARNING: pNode must be a valid pointer
/*void CNetwork::DisconnectFrom(CG2Node *pNode)
{
	pNode->Close();
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
}*/
void CNetwork::OnSharesReady()
{
	m_bSharesReady = true;
}

void CNetwork::HubBalancing()
{
	QMutexLocker l(&Neighbours.m_pSection);

	//systemLog.postLog(LogSeverity::Notice, "*** HUB BALANCING REPORT ***");

	// get the local hub cluster load
	quint32 nLeaves = 0, nMaxLeaves = 0, nClusterLoad = 0, nLocalLoad = 0;

	const quint32 MINUTES_TRYING_BEFORE_SWITCH = 10; // emergency hub switch
	if(!isHub())
	{
		if(Neighbours.m_nHubsConnected == 0)   // if we're not connected to any hub
		{
			m_nMinutesTrying++;
			if(m_nMinutesTrying > MINUTES_TRYING_BEFORE_SWITCH)   // if no hub connects in this time
			{
				// emergency switch to hub mode, normal downgrades will filter out bad upgrades
				systemLog.postLog(LogSeverity::Notice, "Hub Balancing: No HUB connections for %u minutes, switching to HUB mode.", MINUTES_TRYING_BEFORE_SWITCH);
				SwitchClientMode(G2_HUB);
			}
			return;
		}
		else
		{
			m_nMinutesTrying = 0;
		}
	}

	// check how many leaves are in local hub cluster
	for(QList<CNeighbour*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode)
	{
		CNeighbour* pNode = *itNode;
		if( pNode->m_nProtocol == dpGnutella2 && pNode->m_nState == nsConnected && ((CG2Node*)pNode)->m_nType == G2_HUB)
		{
			nLeaves += ((CG2Node*)pNode)->m_nLeafCount;
			nMaxLeaves += ((CG2Node*)pNode)->m_nLeafMax;
		}
	}

	if(isHub())
	{
		// add our numbers to cluster load
		nLeaves += Neighbours.m_nLeavesConnected;
		nMaxLeaves += quazaaSettings.Gnutella2.NumLeafs;
		// and calculate local hub load percentage
		nLocalLoad = Neighbours.m_nLeavesConnected * 100 / quazaaSettings.Gnutella2.NumLeafs;
		systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Local Hub load: %u%%, leaves connected: %u, capacity: %u", nLocalLoad, Neighbours.m_nLeavesConnected, quazaaSettings.Gnutella2.NumLeafs);
	}

	// calculate local cluster load percentage
	nClusterLoad = nLeaves * 100 / nMaxLeaves;

	systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Local Hub Cluster load: %u%%, leaves connected: %u, capacity: %u", nClusterLoad, nLeaves, nMaxLeaves);

	if(nClusterLoad < 50)
	{
		// if local cluster load is below 50%, increment counter
		m_nMinutesBelow50++;
		systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Cluster loaded below 50%% for %u minutes.", m_nMinutesBelow50);
	}
	else if(nClusterLoad > 90)
	{
		// if local cluster load is above 90%, increment counter
		m_nMinutesAbove90++;
		systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Cluster loaded above 90%% for %u minutes.", m_nMinutesAbove90);
	}
	else
	{
		// reset counters
		m_nMinutesAbove90 = 0;
		m_nMinutesBelow50 = 0;
	}

	if(quazaaSettings.Gnutella2.ClientMode != 0)   // if client mode is forced in settings
	{
		systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Not checking for mode change possibility: current client mode forced.");
		return;
	}

	quint32 tNow = time(0);

	const quint32 MODE_CHANGE_WAIT = 1800; // grace period since last mode change
	const quint32 UPGRADE_TIMEOUT = 30; // if cluster loaded for this time - upgrade
	const quint32 DOWNGRADE_TIMEOUT = 60; // if cluster not loaded for this time - downgrade

	if(tNow - m_tLastModeChange < MODE_CHANGE_WAIT)
	{
		// too early for mode change
		systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Not checking for mode change possibility: too early from last mode change.");
		return;
	}

	if(isHub() && m_nMinutesBelow50 > DOWNGRADE_TIMEOUT)
	{
		// if we're running in hub mode and timeout passed

		if(Neighbours.m_nHubsConnected > 0)   // if we have connections to other hubs
		{
			if(nLocalLoad > 50)   // and our load is below 50%
			{
				systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Cluster load too low for too long, staying in HUB mode, we are above 50%% of our capacity.");
			}
			else
			{
				// switch to leaf mode
				systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Cluster load too low for too long, switching to LEAF mode.");
				SwitchClientMode(G2_LEAF);
			}
		}
		else
		{
			// no connections to other hubs - stay in HUB mode
			systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Cluster load too low for too long, staying in HUB mode due to lack of HUB connections.");
		}
	}
	else if(!isHub() && m_nMinutesAbove90 > UPGRADE_TIMEOUT)
	{
		// if we're running in leaf mode and timeout passed

		// TODO: Analyze computers performance and upgrade only if it meets minimum requirements

		if(!IsFirewalled())
		{
			// switch to HUB mode
			systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Cluster load too high for too long, switching to HUB mode.");
			SwitchClientMode(G2_HUB);
		}
	}
	else
	{
		systemLog.postLog(LogSeverity::Notice, "Hub Balancing: No need for mode change.");
	}

}

bool CNetwork::SwitchClientMode(G2NodeType nRequestedMode)
{
	if(!m_bActive)
	{
		return false;
	}

	if(m_nNodeState == nRequestedMode)
	{
		return false;
	}

	m_nMinutesBelow50 = 0;
	m_nMinutesAbove90 = 0;
	m_tLastModeChange = time(0);

	Neighbours.Clear();
	m_nNodeState = nRequestedMode;

	systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Switched to %s mode.", (isHub() ? "HUB" : "LEAF"));

	return true;
}

void CNetwork::RoutePackets()
{
	m_bPacketsPending = true;

	static quint32 nWhatToRoute = 0;

	if(Datagrams.m_pSection.tryLock(50))
	{
		if(Neighbours.m_pSection.tryLock(50))
		{
			QElapsedTimer oTimer;

			oTimer.start();

			switch(nWhatToRoute)
			{
				case 0:
					while(!Datagrams.m_lPendingQH2.isEmpty() && !oTimer.hasExpired(250))
					{
						QueuedQueryHit pHit = Datagrams.m_lPendingQH2.takeFirst();

						if(pHit.m_pInfo->m_oNodeAddress == pHit.m_pInfo->m_oSenderAddress)
						{
							// hits node address matches sender address
							Network.m_oRoutingTable.Add(pHit.m_pInfo->m_oNodeGUID, pHit.m_pInfo->m_oSenderAddress);
						}
						else if(!pHit.m_pInfo->m_lNeighbouringHubs.isEmpty())
						{
							// hits address does not match sender address (probably forwarded by a hub)
							// and there are neighbouring hubs available, use them instead (sender address can be used instead...)
							Network.m_oRoutingTable.Add(pHit.m_pInfo->m_oNodeGUID, pHit.m_pInfo->m_lNeighbouringHubs[0], false);
						}

						RoutePacket(pHit.m_pInfo->m_oGUID, pHit.m_pPacket);

						pHit.m_pPacket->Release();
						delete pHit.m_pInfo;
					}

					nWhatToRoute = 1;
					break;
				case 1:

					while(!Datagrams.m_lPendingQA.isEmpty() && !oTimer.hasExpired(250))
					{
						QPair<QUuid, G2Packet*> oPair = Datagrams.m_lPendingQA.takeFirst();

						RoutePacket(oPair.first, oPair.second);

						oPair.second->Release();
					}

					while(!Datagrams.m_lPendingQKA.isEmpty() && !oTimer.hasExpired(250))
					{
						QPair<QHostAddress, G2Packet*> oPair = Datagrams.m_lPendingQKA.takeFirst();

						if(CNeighbour* pNode = Neighbours.Find(oPair.first, dpGnutella2))
						{
							((CG2Node*)pNode)->SendPacket(oPair.second, true, true);
						}
						else
						{
							oPair.second->Release();
						}
					}

					nWhatToRoute = 0;
					break;
			}

			m_bPacketsPending = (!Datagrams.m_lPendingQA.isEmpty() || !Datagrams.m_lPendingQH2.isEmpty() || !Datagrams.m_lPendingQKA.isEmpty());

			Neighbours.m_pSection.unlock();
		}

		systemLog.postLog(LogSeverity::Debug, QString("Datagrams left to be routed: QA:%1 QH2:%2 QKA:%3").arg(Datagrams.m_lPendingQA.size()).arg(Datagrams.m_lPendingQH2.size()).arg(Datagrams.m_lPendingQKA.size()));
		//qDebug() << "Datagrams left to be routed: QA:" << Datagrams.m_lPendingQA.size() << " QH2:" << Datagrams.m_lPendingQH2.size() << " QKA:" << Datagrams.m_lPendingQKA.size();

		Datagrams.m_pSection.unlock();
	}
}
