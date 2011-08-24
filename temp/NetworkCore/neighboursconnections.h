/*
** $Id: neighboursconnections.h 592 2011-05-29 22:29:44Z brov $
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


#ifndef NEIGHBOURSCONNECTIONS_H
#define NEIGHBOURSCONNECTIONS_H

#include "neighboursrouting.h"

class CNetworkConnection;
class CRateController;

class CNeighboursConnections : public CNeighboursRouting
{
	Q_OBJECT
protected:
	CRateController* m_pController;
public:
	quint32 m_nHubsConnectedG2;
	quint32 m_nLeavesConnectedG2;
public:
	CNeighboursConnections(QObject* parent = 0);
	virtual ~CNeighboursConnections();

	virtual void Connect();
	virtual void Disconnect();

	void AddNode(CNeighbour* pNode);
	void RemoveNode(CNeighbour* pNode);

	void DisconnectYoungest(DiscoveryProtocol nProtocol, int nType = 0, bool bCore = false);

	virtual quint32 DownloadSpeed();
	virtual quint32 UploadSpeed();

signals:

public slots:
	CNeighbour* ConnectTo(CEndPoint& oAddress, DiscoveryProtocol nProtocol);
	CNeighbour* OnAccept(CNetworkConnection* pConn);

	virtual void Maintain();
};

#endif // NEIGHBOURSCONNECTIONS_H
