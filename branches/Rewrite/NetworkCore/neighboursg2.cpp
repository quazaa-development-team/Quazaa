/*
** $Id$
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


#include "neighboursg2.h"
#include "webcache.h"
#include "hostcache.h"
#include "g2node.h"
#include "g2packet.h"

#include "quazaasettings.h"

CNeighboursG2::CNeighboursG2(QObject *parent) :
	CNeighboursConnections(parent),
	m_nNextKHL(30),
	m_nSecsTrying(0),
	m_nHubBalanceWait(0),
	m_nPeriodsLow(0),
	m_nPeriodsHigh(0),
	m_tLastModeChange(0)
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
	m_tLastModeChange = time(0);

	CNeighboursConnections::Connect();

	static bool bStartupRequest = false;

	if( !bStartupRequest && !WebCache.isRequesting() )
	{
		WebCache.RequestRandom();
		bStartupRequest = true;
	}

}

void CNeighboursG2::Maintain()
{
	ASSUME_LOCK(m_pSection);
	CNeighboursConnections::Maintain();

	if(m_nHubsConnectedG2 == 0 && !WebCache.isRequesting() && (HostCache.isEmpty() || HostCache.GetConnectable() == 0))
	{
		WebCache.RequestRandom();
	}

	if( m_nNextKHL == 0 )
	{
		DispatchKHL();
		m_nNextKHL = quazaaSettings.Gnutella2.KHLPeriod;
	}
	else
	{
		m_nNextKHL--;
	}

	if( m_nHubsConnectedG2 == 0 )
	{
		m_nSecsTrying++;

		if( m_nSecsTrying / 60 > 10 && quazaaSettings.Gnutella2.ClientMode == 0 )
		{
			SwitchG2ClientMode(G2_HUB);
			m_nSecsTrying = 0;
		}
	}
	else
	{
		m_nSecsTrying = 0;
	}

	if( time(0) - m_tLastModeChange > quazaaSettings.Gnutella2.HubBalanceGrace )
	{
		if( m_nHubBalanceWait == 0 )
		{
			HubBalancing();
			m_nHubBalanceWait = quazaaSettings.Gnutella2.HubBalancePeriod;
		}
		else
		{
			m_nHubBalanceWait--;
		}
	}
}

void CNeighboursG2::DispatchKHL()
{
	ASSUME_LOCK(m_pSection);

	if( m_nHubsConnectedG2 == 0 && m_nLeavesConnectedG2 == 0 )
		return;

	G2Packet* pKHL = G2Packet::New("KHL");
	quint32 ts = time(0);
	pKHL->WritePacket("TS", 4)->WriteIntLE(ts);

	foreach(CNeighbour* pNode, m_lNodes)
	{
		if( pNode->m_nProtocol != dpGnutella2 )
			continue;

		if( pNode->m_nState == nsConnected && ((CG2Node*)pNode)->m_nType == G2_HUB )
		{
			if( pNode->m_oAddress.protocol() == QAbstractSocket::IPv4Protocol )
				pKHL->WritePacket("NH", 6)->WriteHostAddress(&pNode->m_oAddress);
			else
				pKHL->WritePacket("NH", 18)->WriteHostAddress(&pNode->m_oAddress);
		}
	}

	HostCache.m_pSection.lock();

	quint32 nCount = 0;

	// TODO: IPv6
	for(; nCount < (quint32)quazaaSettings.Gnutella2.KHLHubCount && HostCache.size() > nCount; nCount++)
	{
		pKHL->WritePacket("CH", 10)->WriteHostAddress(&HostCache.m_lHosts.at(nCount)->m_oAddress);
		pKHL->WriteIntLE(&HostCache.m_lHosts.at(nCount)->m_tTimestamp);
	}

	HostCache.m_pSection.unlock();

	foreach(CNeighbour* pNode, m_lNodes)
	{
		if( pNode->m_nState == nsConnected && pNode->m_nProtocol == dpGnutella2 )
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

	foreach( CNeighbour* pNode, m_lNodes )
	{
		if( pNode->m_nProtocol == dpGnutella2 )
			pNode->Close();
	}

	m_nClientMode = nRequestedMode;

	systemLog.postLog(LogSeverity::Notice, "Hub Balancing: Switched to %s mode.", (IsG2Hub() ? "HUB" : "LEAF"));

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

	if( m_nHubsConnectedG2 == 0 )
		return;

	if( quazaaSettings.Gnutella2.ClientMode != 0 )
		return;

	if( m_nClientMode == G2_LEAF )
	{
		// we're a leaf
		// TODO: Check capabilities
		quint32 nLeaves = 0, nCapacity = 0;
		foreach(CNeighbour* pNode, m_lNodes)
		{
			if( pNode->m_nState == nsConnected && pNode->m_nProtocol == dpGnutella2 && ((CG2Node*)pNode)->m_nType == G2_HUB )
			{
				nLeaves += ((CG2Node*)pNode)->m_nLeafCount;
				nCapacity += ((CG2Node*)pNode)->m_nLeafMax;
			}
		}

		if( nLeaves * 100 / nCapacity > quazaaSettings.Gnutella2.HubBalanceHigh )
		{
			m_nPeriodsHigh++;

			if( m_nPeriodsHigh >= quazaaSettings.Gnutella2.HubBalanceHighTime )
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
		foreach(CNeighbour* pNode, m_lNodes)
		{
			if( pNode->m_nState == nsConnected && pNode->m_nProtocol == dpGnutella2 && ((CG2Node*)pNode)->m_nType == G2_HUB )
			{
				nLeaves += ((CG2Node*)pNode)->m_nLeafCount;
				nCapacity += ((CG2Node*)pNode)->m_nLeafMax;
			}
		}

		nLeaves += m_nLeavesConnectedG2;
		nCapacity += quazaaSettings.Gnutella2.NumLeafs;

		if( nLeaves * 100 / nCapacity < quazaaSettings.Gnutella2.HubBalanceLow )
		{
			m_nPeriodsLow++;

			if( m_nPeriodsLow >= quazaaSettings.Gnutella2.HubBalanceLowTime )
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
