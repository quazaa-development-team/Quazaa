/*
** handshake.cpp
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

#include "handshake.h"
#include "handshakes.h"
#include "network.h"
#include "neighbours.h"

#include <QTcpSocket>

#include "quazaaglobals.h"

CHandshake::CHandshake(QObject* parent)
	: CNetworkConnection(parent)
{
}
CHandshake::~CHandshake()
{
	ASSUME_LOCK(Handshakes.m_pSection);

	Handshakes.RemoveHandshake(this);
}

void CHandshake::OnTimer(quint32 tNow)
{
	if(tNow - m_tConnected > 15)
	{
		systemLog.postLog(LogSeverity::Debug, QString("Timed out handshaking with  %1").arg(m_pSocket->peerAddress().toString().toAscii().constData()));
		Close();
	}
}
void CHandshake::OnRead()
{
	QMutexLocker l(&Handshakes.m_pSection);

	//qDebug() << "CHandshake::OnRead()";

	if(bytesAvailable() < 8)
	{
		return;
	}

	if(Peek(8).startsWith("GNUTELLA"))
	{
		systemLog.postLog(LogSeverity::Debug, QString("Incoming connection from %1 is Gnutella Neighbour connection").arg(m_pSocket->peerAddress().toString().toAscii().constData()));
		//qDebug("Incoming connection from %s is Gnutella Neighbour connection", m_pSocket->peerAddress().toString().toAscii().constData());
		Handshakes.processNeighbour(this);
		delete this;
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
}

void CHandshake::OnConnect()
{

}
void CHandshake::OnDisconnect()
{
	Handshakes.m_pSection.lock();
	delete this;
	Handshakes.m_pSection.unlock();
}
void CHandshake::OnError(QAbstractSocket::SocketError e)
{
	Q_UNUSED(e);
	Handshakes.m_pSection.lock();
	delete this;
	Handshakes.m_pSection.unlock();
}

void CHandshake::OnStateChange(QAbstractSocket::SocketState s)
{
	Q_UNUSED(s);
}
