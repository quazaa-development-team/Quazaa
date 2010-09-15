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
#include <QList>
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
    QList<CG2Node*>  m_lNodes;
    CRateController* m_pRateController;
    quint16          m_nHubsConnected;
    quint16          m_nLeavesConnected;
    bool             m_bNeedUpdateLNI;
    quint32          m_nLNIWait;
    quint32          m_nKHLWait;
    IPv4_ENDPOINT    m_oAddress;

    CRouteTable      m_oRoutingTable;
    quint32          m_tCleanRoutesNext;

	QList<QPair<QUuid, G2Packet*> > m_lHitsToRoute;

	quint32			 m_nNextCheck;		// secs to next AdatpiveCheckPeriod
	quint32			 m_nBusyPeriods;	// num of busy periods
	quint32			 m_nTotalPeriods;	// how many check periods?

	bool			 m_bSharesReady;

	quint32			 m_nCookie;

	static const quint32 HUB_BALANCING_INTERVAL = 60;
	quint32			m_nSecsToHubBalancing;
	quint32			m_nMinutesBelow50;
	quint32			m_nMinutesAbove90;
	quint32			m_nMinutesTrying;
	quint32			m_tLastModeChange;

public:
    CNetwork(QObject* parent = 0);
    ~CNetwork();

    void Connect();
    void Disconnect();
    void DisconnectAllNodes();

    void RemoveNode(CG2Node* pNode);
    bool NeedMore(G2NodeType nType);
	void OnAccept(CNetworkConnection* pConn);

    void AcquireLocalAddress(QString& sHeader);
    bool IsListening();
    bool IsFirewalled();

	void RouteHits(QUuid& oTarget, G2Packet* pPacket);
	void FlushHits();
    bool RoutePacket(QUuid& pTargetGUID, G2Packet* pPacket);
    bool RoutePacket(G2Packet* pPacket, CG2Node* pNbr = 0);

	CG2Node* FindNode(quint32 nAddress);

    inline bool isHub()
    {
        return (m_nNodeState == G2_HUB);
    }
    inline IPv4_ENDPOINT GetLocalAddress()
    {
        return m_oAddress;
    }

    inline quint32 UploadSpeed()
    {
        if( m_pRateController )
        {
            return m_pRateController->UploadSpeed();
        }
        return 0;
    }
    inline quint32 DownloadSpeed()
    {
        if( m_pRateController )
        {
            return m_pRateController->DownloadSpeed();
        }
        return 0;
    }

    QList<CG2Node*>* List(){ return &m_lNodes; }

    bool IsConnectedTo(IPv4_ENDPOINT addr);

	void HubBalancing();
	bool SwitchClientMode(G2NodeType nRequestedMode);

public slots:
    void OnSecondTimer();

    void OnNodeStateChange();

    void SetupThread();
    void CleanupThread();

	void ConnectTo(IPv4_ENDPOINT& addr);
	void DisconnectFrom(IPv4_ENDPOINT& ip);
	void DisconnectFrom(int index);
	void DisconnectFrom(CG2Node* pNode);

	void OnSharesReady();
	void OnLocalHashTableUpdate();

signals:
    void NodeAdded(CG2Node*);
    void NodeRemoved(CG2Node*);
    void NodeUpdated(CG2Node*);

protected:
    void Maintain();
    void DispatchKHL();
    void DropYoungest(G2NodeType nType, bool bCore = false);
	void AdaptiveHubRun();

    friend class CG2Node;
};

extern CNetwork Network;
extern CThread NetworkThread;
#endif // NETWORK_H
