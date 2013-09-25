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

#include "neighboursg2.h"
#include "hostcache.h"
#include "g2node.h"
#include "g2packet.h"
#include "network.h"
#include "hubhorizon.h"

#include "quazaasettings.h"
#include "quazaaglobals.h"
#include "commonfunctions.h"

#include "Discovery/discovery.h"

#include "debug_new.h"

CNeighboursG2::CNeighboursG2(QObject* parent) :
	CNeighboursConnections(parent),
	m_nNextKHL(30),
	m_nLNIWait(0),
	m_bNeedLNI(false),
    m_nUpdateWait(0),
	m_nSecsTrying(0),
	m_tLastModeChange(0),
	m_nHubBalanceWait(0),
	m_nPeriodsLow(0),
	m_nPeriodsHigh(0)
{
}
CNeighboursG2::~CNeighboursG2()
{
}

void CNeighboursG2::Connect()
{
	if(quazaaSettings.Gnutella2.ClientMode < 2)
	{
		m_nClientMode = G2_LEAF;
	}
	else
	{
		m_nClientMode = G2_HUB;
	}

	m_nSecsTrying = m_nHubBalanceWait = m_nPeriodsLow = m_nPeriodsHigh = 0;
	m_bNeedLNI = false;
	m_nLNIWait = quazaaSettings.Gnutella2.LNIMinimumUpdate;
	m_tLastModeChange = time(0);

	CNeighboursConnections::Connect();

	static bool bStartupRequest = false;

	// TODO: Test whether already active checking is required
	if ( !bStartupRequest )
	{
		discoveryManager.queryService( CNetworkType( dpG2 ) );
		bStartupRequest = true;
	}

	HubHorizonPool.Setup();

}

void CNeighboursG2::Maintain()
{
	ASSUME_LOCK(m_pSection);

	quint32 nNodes = m_nHubsConnectedG2 + m_nLeavesConnectedG2;

	CNeighboursConnections::Maintain();

	if(m_nHubsConnectedG2 + m_nLeavesConnectedG2 != nNodes)
	{
		m_bNeedLNI = true;
	}

	// TODO: Test whether already active checking is required
	if ( !m_nHubsConnectedG2 && !discoveryManager.isActive( Discovery::stGWC )
		 && ( hostCache.isEmpty() || !hostCache.getConnectable() ) && m_nUnknownInitiated == 0 )
	{
		qDebug() << "GWC query: Active:" << discoveryManager.isActive(Discovery::stGWC) << ", empty cache:" << hostCache.isEmpty() << ", has connectable:" << (hostCache.getConnectable() != 0) << "has unknown initiated:" << (m_nUnknownInitiated != 0);
		discoveryManager.queryService( CNetworkType( dpG2 ) );
	}

	if(m_nNextKHL == 0)
	{
		DispatchKHL();
		m_nNextKHL = quazaaSettings.Gnutella2.KHLPeriod;
	}
	else
	{
		m_nNextKHL--;
	}

	if(m_nLNIWait == 0)
	{
		if(m_bNeedLNI)
		{
			m_bNeedLNI = false;
			m_nLNIWait = quazaaSettings.Gnutella2.LNIMinimumUpdate;

			foreach(CNeighbour * pNode, m_lNodes)
			{
				if(pNode->m_nProtocol == dpG2 && pNode->m_nState == nsConnected)
				{
					((CG2Node*)pNode)->SendLNI();
				}
			}
		}
	}
	else
	{
		--m_nLNIWait;
	}

	if(m_nHubsConnectedG2 == 0)
	{
		m_nSecsTrying++;

		if(m_nSecsTrying / 60 > 10 && quazaaSettings.Gnutella2.ClientMode == 0)
		{
			SwitchG2ClientMode(G2_HUB);
			m_nSecsTrying = 0;
		}
	}
	else
	{
		m_nSecsTrying = 0;
	}

	if(time(0) - m_tLastModeChange > quazaaSettings.Gnutella2.HubBalanceGrace)
	{
		if(m_nHubBalanceWait == 0)
		{
			HubBalancing();
			m_nHubBalanceWait = quazaaSettings.Gnutella2.HubBalancePeriod;
		}
		else
		{
			m_nHubBalanceWait--;
		}
	}


	if ( IsG2Hub() && Network.GetLocalAddress().isValid()
		 && !discoveryManager.isActive( Discovery::stGWC )
		 && m_nUpdateWait-- == 0 )
	{
		if ( m_nLeavesConnectedG2 < 0.7 * quazaaSettings.Gnutella2.NumLeafs ) // if we have less than 70% leaves (no reason to update GWC if we are already full of leaves)
		{
			discoveryManager.updateService(CNetworkType(dpG2));
		}
		m_nUpdateWait = quazaaSettings.Discovery.AccessThrottle * 60;
	}
}

void CNeighboursG2::DispatchKHL()
{
	ASSUME_LOCK(m_pSection);

	if(m_nHubsConnectedG2 == 0 && m_nLeavesConnectedG2 == 0)
	{
		return;
	}

	G2Packet* pKHL = G2Packet::New("KHL");

	QDateTime tNow = common::getDateTimeUTC();

	pKHL->WritePacket("TS", 4)->WriteIntLE<quint32>(tNow.toTime_t());

	foreach(CNeighbour * pNode, m_lNodes)
	{
		if(pNode->m_nProtocol != dpG2)
		{
			continue;
		}

		if(pNode->m_nState == nsConnected && ((CG2Node*)pNode)->m_nType == G2_HUB)
		{
			if(pNode->m_oAddress.protocol() == QAbstractSocket::IPv4Protocol)
			{
				pKHL->WritePacket("NH", 6)->WriteHostAddress(&pNode->m_oAddress);
			}
			else
			{
				pKHL->WritePacket("NH", 18)->WriteHostAddress(&pNode->m_oAddress);
			}
		}
	}

	hostCache.m_pSection.lock();

	quint32 nCount = quazaaSettings.Gnutella2.KHLHubCount;
	CHostCacheIterator itHost = hostCache.m_lHosts.begin();

	for(; nCount > 0 && itHost != hostCache.m_lHosts.end(); itHost++)
	{
		if( (*itHost)->m_nFailures == 0 && (*itHost)->m_tTimestamp.secsTo(tNow) < quazaaSettings.Gnutella2.HostCurrent )
		{
			if( (*itHost)->m_oAddress.protocol() == QAbstractSocket::IPv4Protocol )
			{
				pKHL->WritePacket("CH", 10)->WriteHostAddress(&(*itHost)->m_oAddress);
				pKHL->WriteIntLE<quint32>((*itHost)->m_tTimestamp.toTime_t());
			}
			else
			{
				pKHL->WritePacket("CH", 22)->WriteHostAddress(&(*itHost)->m_oAddress);
				pKHL->WriteIntLE<quint32>((*itHost)->m_tTimestamp.toTime_t());
			}
			--nCount;
		}
	}

	hostCache.m_pSection.unlock();

	foreach(CNeighbour * pNode, m_lNodes)
	{
		if(pNode->m_nState == nsConnected && pNode->m_nProtocol == dpG2)
		{
			((CG2Node*)pNode)->SendPacket(pKHL, false, false);
		}
	}

	pKHL->Release();
}

bool CNeighboursG2::SwitchG2ClientMode(G2NodeType nRequestedMode)
{
	if(!m_bActive)
	{
		return false;
	}

	if(m_nClientMode == nRequestedMode)
	{
		return false;
	}

	m_nPeriodsLow = m_nPeriodsHigh = 0;
	m_tLastModeChange = time(0);

	foreach(CNeighbour * pNode, m_lNodes)
	{
		if(pNode->m_nProtocol == dpG2)
		{
			pNode->Close();
		}
	}

	m_nClientMode = nRequestedMode;
	m_nUpdateWait = 0;

	systemLog.postLog( LogSeverity::Notice, Components::G2,
	                   "Hub Balancing: Switched to %s mode.", ( IsG2Hub() ? "HUB" : "LEAF" ) );

	return true;
}

bool CNeighboursG2::NeedMoreG2(G2NodeType nType)
{
	if(nType == G2_HUB)   // potrzeba hubow?
	{
		if(IsG2Hub())   // jesli hub
		{
			return (m_nHubsConnectedG2 < quazaaSettings.Gnutella2.NumPeers);
		}
		else    // jesli leaf
		{
			return (m_nLeavesConnectedG2 < quazaaSettings.Gnutella2.NumHubs);
		}
	}
	else // potrzeba leaf?
	{
		if(IsG2Hub())      // jesli hub
		{
			return (m_nLeavesConnectedG2 < quazaaSettings.Gnutella2.NumLeafs);
		}
	}

	return false;
}

void CNeighboursG2::HubBalancing()
{
	// NOT TESTED
	ASSUME_LOCK(m_pSection);

	if(m_nHubsConnectedG2 == 0)
	{
		return;
	}

	if(quazaaSettings.Gnutella2.ClientMode != 0)
	{
		return;
	}

    bool bHasQHubs = false;

	if(m_nClientMode == G2_LEAF)
	{
		// we're a leaf
		// TODO: Check capabilities

        if( Network.IsFirewalled() )
            return;

		quint32 nLeaves = 0, nCapacity = 0;

		foreach(CNeighbour * pNode, m_lNodes)
		{
			if(pNode->m_nState == nsConnected && pNode->m_nProtocol == dpG2 && ((CG2Node*)pNode)->m_nType == G2_HUB)
			{
				nLeaves += ((CG2Node*)pNode)->m_nLeafCount;
				nCapacity += ((CG2Node*)pNode)->m_nLeafMax;
                bHasQHubs |= ((CG2Node*)pNode)->m_bG2Core;
			}
		}

        if( !bHasQHubs )
        {
            // Switch to G2 Hub mode if there are no other Quazaa hubs
            systemLog.postLog(LogSeverity::Notice, "Switching to G2 HUB mode (no Quazaa hubs)");
            SwitchG2ClientMode(G2_HUB);
            return;
        }

		if(nLeaves * 100 / nCapacity > quazaaSettings.Gnutella2.HubBalanceHigh)
		{
			m_nPeriodsHigh++;

			if(m_nPeriodsHigh >= quazaaSettings.Gnutella2.HubBalanceHighTime)
			{
				systemLog.postLog(LogSeverity::Notice, "Switching to G2 HUB mode");
				SwitchG2ClientMode(G2_HUB);
				return;
			}
		}
        else
		{
			m_nPeriodsHigh = m_nPeriodsLow = 0;
		}
	}
	else
	{
		// we're hub
		quint32 nLeaves = 0, nCapacity = 0;

		foreach(CNeighbour * pNode, m_lNodes)
		{
			if(pNode->m_nState == nsConnected && pNode->m_nProtocol == dpG2 && ((CG2Node*)pNode)->m_nType == G2_HUB)
			{
				nLeaves += ((CG2Node*)pNode)->m_nLeafCount;
				nCapacity += ((CG2Node*)pNode)->m_nLeafMax;
                bHasQHubs |= ((CG2Node*)pNode)->m_bG2Core;
			}
		}

		nLeaves += m_nLeavesConnectedG2;
		nCapacity += quazaaSettings.Gnutella2.NumLeafs;

        if(nLeaves * 100 / nCapacity < quazaaSettings.Gnutella2.HubBalanceLow && bHasQHubs) // Downgrade if there are other Quazaa hubs
		{
			m_nPeriodsLow++;

			if(m_nPeriodsLow >= quazaaSettings.Gnutella2.HubBalanceLowTime)
			{
				systemLog.postLog(LogSeverity::Notice, "Switching to G2 LEAF mode");
				SwitchG2ClientMode(G2_LEAF);
				return;
			}
		}
		else
		{
			m_nPeriodsHigh = m_nPeriodsLow = 0;
		}

	}
}

G2Packet* CNeighboursG2::CreateQueryAck(QUuid oGUID, bool bWithHubs, CNeighbour* pExcept, bool bDone)
{
	G2Packet* pPacket = G2Packet::New("QA", true);

	pPacket->WritePacket("TS", 4)->WriteIntLE<quint32>( common::getTNowUTC() );
	pPacket->WritePacket("FR", (Network.m_oAddress.protocol() == QAbstractSocket::IPv4Protocol ? 6 : 18))->WriteHostAddress(&Network.m_oAddress);
	pPacket->WritePacket("RA", 4)->WriteIntLE<quint32>(30 + 30 * m_nHubsConnectedG2);
	pPacket->WritePacket("V", 4)->WriteString(QuazaaGlobals::VENDOR_CODE(), false);

	if(bDone)
	{
		pPacket->WritePacket("D", (Network.m_oAddress.protocol() == QAbstractSocket::IPv4Protocol ? 8 : 20))->WriteHostAddress(&Network.m_oAddress);

		if(bWithHubs)
		{
			pPacket->WriteIntLE<quint16>(m_nLeavesConnectedG2);

			foreach(CNeighbour * pNode, m_lNodes)
			{
				if(pNode->m_nProtocol == dpG2 && pNode->m_nState == nsConnected && ((CG2Node*)pNode)->m_nType == G2_HUB && pNode != pExcept)
				{
					pPacket->WritePacket("D", (pNode->m_oAddress.protocol() == QAbstractSocket::IPv4Protocol ? 8 : 20))->WriteHostAddress(&pNode->m_oAddress);
					pPacket->WriteIntLE<quint16>(((CG2Node*)pNode)->m_nLeafCount);
				}
			}

			/*int nCount = */HubHorizonPool.AddHorizonHubs(pPacket);

			// TODO Add hubs from HostCache
			/*if( nCount < 10 )
			{
				HostCache.m_pSection.lock();

				foreach( CHostCacheHost* pHost, HostCache.m_lHosts )
				{

				}

				HostCache.m_pSection.unlock();
			}*/
		}
		else
		{
			pPacket->WriteIntLE<quint16>(0);
		}
	}

	pPacket->WriteByte(0);
	pPacket->WriteGUID(oGUID);

	return pPacket;
}

