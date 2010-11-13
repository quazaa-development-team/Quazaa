//
// network.h
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

#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QMutex>
#include "types.h"
#include "ratecontroller.h"
#include "routetable.h"

class QTimer;
class CG2Node;
class CThread;
class G2Packet;

class CManagedSearch;

class CNetwork : public QObject
{
	Q_OBJECT

public:
	QMutex  m_pSection;

public:
	bool             m_bActive;
	QTimer*          m_pSecondTimer;
	G2NodeType       m_nNodeState;
	bool             m_bNeedUpdateLNI;
	quint32          m_nLNIWait;
	quint32          m_nKHLWait;
	IPv4_ENDPOINT    m_oAddress;

	CRouteTable      m_oRoutingTable;
	quint32          m_tCleanRoutesNext;

	quint32			 m_nNextCheck;		// secs to next AdatpiveCheckPeriod
	quint32			 m_nBusyPeriods;	// num of busy periods
	quint32			 m_nTotalPeriods;	// how many check periods?

	bool			 m_bSharesReady;

	static const quint32 HUB_BALANCING_INTERVAL = 60;
	quint32			m_nSecsToHubBalancing;
	quint32			m_nMinutesBelow50;
	quint32			m_nMinutesAbove90;
	quint32			m_nMinutesTrying;
	quint32			m_tLastModeChange;

	bool			m_bPacketsPending;

public:
	CNetwork(QObject* parent = 0);
	~CNetwork();

	void Connect();
	void Disconnect();

	bool NeedMore(G2NodeType nType);

	void AcquireLocalAddress(QString& sHeader);
	bool IsListening();
	bool IsFirewalled();

	bool RoutePacket(QUuid& pTargetGUID, G2Packet* pPacket);
	bool RoutePacket(G2Packet* pPacket, CG2Node* pNbr = 0);

	inline bool isHub()
	{
		return (m_nNodeState == G2_HUB);
	}
	inline IPv4_ENDPOINT GetLocalAddress()
	{
		return m_oAddress;
	}


	bool IsConnectedTo(IPv4_ENDPOINT addr);

	void HubBalancing();
	bool SwitchClientMode(G2NodeType nRequestedMode);

public slots:
	void OnSecondTimer();

	void SetupThread();
	void CleanupThread();

	void ConnectTo(IPv4_ENDPOINT& addr);
	/*void DisconnectFrom(IPv4_ENDPOINT& ip);
	void DisconnectFrom(int index);
	void DisconnectFrom(CG2Node* pNode);*/

	void OnSharesReady();
	void RoutePackets();

signals:
	void NodeAdded(CG2Node*);
	void NodeRemoved(CG2Node*);
	void NodeUpdated(CG2Node*);

protected:
	void DispatchKHL();
	void AdaptiveHubRun();

	friend class CG2Node;
};

extern CNetwork Network;
extern CThread NetworkThread;
#endif // NETWORK_H
