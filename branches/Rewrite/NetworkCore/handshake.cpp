//
// handshake.cpp
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

#include "handshake.h"
#include "handshakes.h"
#include "network.h"
#include "neighbours.h"

#include <QTcpSocket>

#include "quazaaglobals.h"

CHandshake::CHandshake(QObject* parent)
	: CNetworkConnection(parent)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(OnRead()), Qt::QueuedConnection);
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
}
CHandshake::~CHandshake()
{
	Handshakes.RemoveHandshake(this);
}

void CHandshake::OnTimer(quint32 tNow)
{
	//qDebug() << "handshake timer";
	if(tNow - m_tConnected > 15)
	{
		systemLog.postLog(LogSeverity::Debug, QString("Timed out handshaking with  %1").arg(m_pSocket->peerAddress().toString().toAscii().constData()));
		//qDebug() << "Timed out handshaking with " << m_pSocket->peerAddress().toString().toAscii().constData();
		Close();
		deleteLater();
	}
}
void CHandshake::OnRead()
{
	//qDebug() << "CHandshake::OnRead()";

	if(bytesAvailable() < 8)
	{
		return;
	}

	if(Peek(8).startsWith("GNUTELLA"))
	{
		systemLog.postLog(LogSeverity::Debug, QString("Incoming connection from %1 is Gnutella Neighbour connection").arg(m_pSocket->peerAddress().toString().toAscii().constData()));
		//qDebug("Incoming connection from %s is Gnutella Neighbour connection", m_pSocket->peerAddress().toString().toAscii().constData());
		Handshakes.RemoveHandshake(this);
		Neighbours.OnAccept(this);
	}
	else
	{
		systemLog.postLog(LogSeverity::Debug, QString("Closing connection with %1 - unknown protocol").arg(m_pSocket->peerAddress().toString().toAscii().constData()));
		//qDebug("Closing connection with %s - unknown protocol", m_pSocket->peerAddress().toString().toAscii().constData());

		QByteArray baResp;
		baResp += "HTTP/1.1 501 Not Implemented\r\n";
                baResp += "Server: " + QuazaaGlobals::USER_AGENT_STRING() + "\r\n";
		baResp += "\r\n";

		Write(baResp);
		Close(true);
	}

	delete this;
}

void CHandshake::OnConnect()
{

}
void CHandshake::OnDisconnect()
{

}
void CHandshake::OnError(QAbstractSocket::SocketError e)
{
	Q_UNUSED(e);
}

void CHandshake::OnStateChange(QAbstractSocket::SocketState s)
{
	Q_UNUSED(s);
}
