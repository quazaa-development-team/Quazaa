//
// Handshake.cpp
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

#include "Handshake.h"
#include "Handshakes.h"
#include "network.h"

#include <QTcpSocket>

#include "quazaaglobals.h"

CHandshake::CHandshake(QObject* parent)
    :QObject(parent)
{
    m_pSocket = 0;
}
void CHandshake::acceptFrom(int handle)
{
    m_tConnected = time(0);
    m_pSocket = new QTcpSocket();
    m_pSocket->setReadBufferSize(1024);

	connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(OnRead()), Qt::QueuedConnection);
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));

    m_pSocket->setSocketDescriptor(handle);

	if( m_pSocket->peerAddress() == QHostAddress::Any || m_pSocket->peerAddress() == QHostAddress::Null )
	{
		m_pSocket->abort();
		deleteLater();
		return;
	}

    qDebug() << "Accepted an incoming connection from " << m_pSocket->peerAddress().toString().toAscii().constData();
    OnRead();
}
CHandshake::~CHandshake()
{
    if( m_pSocket )
    {
        m_pSocket->abort();
        m_pSocket->deleteLater();
    }

    Handshakes.RemoveHandshake(this);
}

void CHandshake::OnTimer(quint32 tNow)
{
    //qDebug() << "handshake timer";
	if( m_pSocket && tNow - m_tConnected > 15 )
    {
        qDebug() << "Timed out handshaking with " << m_pSocket->peerAddress().toString().toAscii().constData();
        m_pSocket->close();
        deleteLater();
    }
}
void CHandshake::OnRead()
{
    //qDebug() << "CHandshake::OnRead()";

    if( m_pSocket->bytesAvailable() < 8 )
        return;

    //qDebug() << "Handshake received: " << m_pSocket->peek(m_pSocket->bytesAvailable());

    if( m_pSocket->peek(8).startsWith("GNUTELLA") )
    {
        qDebug("Incoming connection from %s is Gnutella Neighbour connection", m_pSocket->peerAddress().toString().toAscii().constData());
        Network.OnAccept(m_pSocket);
        m_pSocket = 0;
        deleteLater();
    }
    else
    {
        qDebug("Closing connection with %s - unknown protocol", m_pSocket->peerAddress().toString().toAscii().constData());

		QByteArray baResp;
		baResp += "HTTP/1.1 501 Not Implemented\r\n";
		baResp += "Server: " + quazaaGlobals.UserAgentString() + "\r\n";
		baResp += "\r\n";

		m_pSocket->write(baResp);
		m_pSocket->flush();

		m_pSocket->abort();
        deleteLater();
    }
}
