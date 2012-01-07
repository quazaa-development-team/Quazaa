/*
** neighbour.h
**
** Copyright © Quazaa Development Team, 2009-2012.
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

#ifndef NEIGHBOUR_H
#define NEIGHBOUR_H

#include "compressedconnection.h"
#include "types.h"

class CNeighbour : public CCompressedConnection
{
	Q_OBJECT

public:
	DiscoveryProtocol	m_nProtocol;

	quint32         m_tLastPacketIn;
	quint32         m_tLastPacketOut;
	quint32         m_nPacketsIn;
	quint32         m_nPacketsOut;
	QString         m_sUserAgent;

	quint32         m_tLastPingOut;
	quint32         m_nPingsWaiting;
	QElapsedTimer   m_tRTTTimer;
	qint64          m_tRTT;
	quint32         m_tLastQuery;

	NodeState       m_nState;


public:
	CNeighbour(QObject* parent = 0);
	virtual ~CNeighbour();

	virtual void ConnectTo(CEndPoint oAddress)
	{
		systemLog.postLog(LogCategory::Network, LogSeverity::Information, "Initiating neighbour connection to %s...", qPrintable(oAddress.toString()));
		m_nState = nsConnecting;
		CNetworkConnection::ConnectTo(oAddress);
	}
	void AttachTo(CNetworkConnection* pOther)
	{
		m_nState = nsHandshaking;
		CCompressedConnection::AttachTo(pOther);
	}

	virtual void OnTimer(quint32 tNow);
	void Close(bool bDelayed = false);

signals:

public slots:
	void OnDisconnect();
	void OnError(QAbstractSocket::SocketError e);
	void OnStateChange(QAbstractSocket::SocketState s);

};

#endif // NEIGHBOUR_H
