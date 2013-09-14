/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
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
//#include "webcache.h"
#include "g2packet.h"
#include "datagrams.h"
#include <QTimer>
#include "g2node.h"
#include "handshakes.h"
#include "neighbours.h"

#include "quazaasettings.h"

#include "queryhashmaster.h"
#include "searchmanager.h"
#include "sharemanager.h"

#include "geoiplist.h"

#include "debug_new.h"

CNetwork Network;
CThread NetworkThread;

CNetwork::CNetwork(QObject* parent)
	: QObject(parent)
{
	m_pSecondTimer = 0;
	//m_oAddress.port = 6346;
	m_oAddress.setPort(quazaaSettings.Connection.Port);

	m_tCleanRoutesNext = 60;

	m_bSharesReady = false;

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

	m_bActive = true;
	m_oAddress.setPort(quazaaSettings.Connection.Port);

	Handshakes.Listen();

	m_oRoutingTable.Clear();

	connect(&ShareManager, SIGNAL(sharesReady()), this, SLOT(OnSharesReady()), Qt::UniqueConnection);

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
//	WebCache.CancelRequests();

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

	if(!QueryHashMaster.IsValid())
	{
		QueryHashMaster.Build();
	}

	Neighbours.Maintain();

	SearchManager.OnTimer();

	m_pSection.unlock();

	emit Datagrams.SendQueueUpdated();
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
	CEndPoint hostAddr(sHeader, m_oAddress.port());

	if(hostAddr.isValid())
	{
		if( hostAddr != m_oAddress )
		{
			m_oAddress = hostAddr;
			emit LocalAddressChanged();
		}
	}
}
bool CNetwork::IsConnectedTo(CEndPoint /*addr*/)
{
	return false;
}

bool CNetwork::RoutePacket(QUuid& pTargetGUID, G2Packet* pPacket, bool bLockNeighbours, bool bBuffered)
{
	CG2Node* pNode = 0;
	CEndPoint pAddr;

	if(m_oRoutingTable.Find(pTargetGUID, &pNode, &pAddr))
	{
		if(pNode)
		{
			if( bLockNeighbours )
			{
				Neighbours.m_pSection.lock();
			}

			if( Neighbours.NeighbourExists(pNode) )
			{
				pNode->SendPacket(pPacket, bBuffered, false);
				systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket %1 Packet: %2 routed to neighbour: %3").arg(pTargetGUID.toString()).arg(pPacket->GetType()).arg(pNode->m_oAddress.toString().toLocal8Bit().constData()));
			}

			if( bLockNeighbours )
			{
				Neighbours.m_pSection.unlock();
			}
			return true;
		}
		else if(pAddr.isValid())
		{
			Datagrams.SendPacket(pAddr, pPacket, true);
			systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket %1 Packet: %2 routed to remote node: %3").arg(pTargetGUID.toString()).arg(pPacket->GetType()).arg(pAddr.toString().toLocal8Bit().constData()));
			return true;
		}
		systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket - No node and no address!"));
		//qDebug() << "CNetwork::RoutePacket - weird thing, should not happen...";
	}

    systemLog.postLog(LogSeverity::Debug, QString("CNetwork::RoutePacket %1 Packet: %2 DROPPED!").arg(pTargetGUID.toString()).arg(pPacket->GetType()));
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
			else if(pAddr.isValid() && bForwardUDP)
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

void CNetwork::ConnectTo(CEndPoint& /*addr*/)
{
	// TODO: Verify network is connected before attempting connection and create connection if it is not
	/*CG2Node* pNew = Neighbours.ConnectTo(addr);

	if(pNew)
	{
		pNew->moveToThread(&NetworkThread);
	}*/
}

void CNetwork::OnSharesReady()
{
	m_bSharesReady = true;
}

