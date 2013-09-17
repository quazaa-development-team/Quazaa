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

#include "handshake.h"
#include "handshakes.h"
#include "network.h"
#include "neighbours.h"
#include "neighbour.h"
#include "g2node.h"

#include <QTcpSocket>
#include <QFile>

#include "quazaaglobals.h"

#include "debug_new.h"

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
		systemLog.postLog(LogSeverity::Debug, QString("Timed out handshaking with  %1").arg(m_pSocket->peerAddress().toString().toLocal8Bit().constData()));
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
		systemLog.postLog(LogSeverity::Debug, QString("Incoming connection from %1 is Gnutella Neighbour connection").arg(m_pSocket->peerAddress().toString().toLocal8Bit().constData()));
		//qDebug("Incoming connection from %s is Gnutella Neighbour connection", m_pSocket->peerAddress().toString().toLocal8Bit().constData());
		Handshakes.processNeighbour(this);
		delete this;
	}
    else if(Peek(5).startsWith("GET /"))
    {
        if( Peek(bytesAvailable()).indexOf("\r\n\r\n") != -1 )
        {
            systemLog.postLog(LogSeverity::Debug, QString("Incoming connection from %1 is a Web request").arg(m_pSocket->peerAddress().toString().toLocal8Bit().constData()));
            OnWebRequest();
        }
    }
	else
	{
		systemLog.postLog(LogSeverity::Debug, QString("Closing connection with %1 - unknown protocol").arg(m_pSocket->peerAddress().toString().toLocal8Bit().constData()));
		//qDebug("Closing connection with %s - unknown protocol", m_pSocket->peerAddress().toString().toLocal8Bit().constData());

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

void CHandshake::OnWebRequest()
{
    QString sRequest;

    sRequest = Read(bytesAvailable());
    sRequest = sRequest.left(sRequest.indexOf("\r\n\r\n"));

    QStringList arrLines = sRequest.split("\r\n");

    if( arrLines[0].startsWith("GET / HTTP") )
    {
        QByteArray baResp;

        baResp += "HTTP/1.1 200 OK\r\n";
        baResp += "Server: " + QuazaaGlobals::USER_AGENT_STRING() + "\r\n";
        baResp += "Connection: close\r\n";
        baResp += "Content-Type: text/html; charset=utf-8\r\n";


        QByteArray baHtml;
        baHtml += "<!DOCTYPE html><html><head><title>Quazaa</title><meta name=\"robots\" content=\"noindex,nofollow\"></head><body>";
        baHtml += "<h1>This server operates Quazaa node</h2>";
        baHtml += "<p><a href=\"http://quazaa.sf.net\">Quazaa</a> is a P2P file sharing client</p>";
        baHtml += "<p>This node is currently connected to the following hosts:<table border=\"1\">";
        baHtml += "<tr><th>Address</th><th>Time</th><th>Mode</th><th>Leaves</th><th>Client</th></tr>";

        Neighbours.m_pSection.lock();

        quint32 tNow = time(0);

        for( QList<CNeighbour*>::iterator it = Neighbours.begin(); it != Neighbours.end(); it++ )
        {
            if( (*it)->m_nState != nsConnected )
                continue;

            baHtml += "<tr><td>" + (*it)->GetAddress().toStringWithPort() + "</td>";

            quint32 tConnected = tNow - (*it)->m_tConnected;
            baHtml += "<td>" + QString().sprintf( "%.2u:%.2u:%.2u", tConnected / 3600,
                                                 tConnected % 3600 / 60, ( tConnected % 3600 ) % 60 ) + "</td>";

            if( (*it)->m_nProtocol == dpG2 )
            {
                CG2Node* pG2 = static_cast<CG2Node*>(*it);

                baHtml += "<td>" + QString((pG2->m_nType == G2_HUB ? "G2 Hub" : "G2 Leaf")) + "</td>";

                baHtml += "<td>";
                if( pG2->m_nType == G2_HUB )
                {
                    baHtml += QString::number(pG2->m_nLeafCount) + "/" + QString::number(pG2->m_nLeafMax);
                }
                else
                {
                    baHtml += "&nbsp;";
                }
                baHtml += "</td>";
            }
            else
            {
                baHtml += "<td>&nbsp;</td><td>&nbsp;</td>";
            }
            baHtml += "<td>" + (*it)->m_sUserAgent + "</td>";
            baHtml += "</tr>";
        }

        Neighbours.m_pSection.unlock();

        baHtml += "</table></p></body></html>";

        baResp += "Content-length: " + QString(baHtml.length()) + "\r\n";
        baResp += "\r\n";

        Write(baResp);
        Write(baHtml);
    }
    else
    {
        QByteArray baResp;

        QString sPath = arrLines[0].mid(4, arrLines[0].length() - 12).trimmed();

        bool bFound = false;

		if(sPath.startsWith("/res")) // redirect /res prefixed URIs to the resource system, anything not prefixed will go to uploads
        {
			sPath = ":/Resource/Web" + sPath.mid(4);

            QFile f(sPath);

			if( f.exists() )
            {
                if( f.open(QIODevice::ReadOnly) )
                {
                    bFound = true;

                    baResp += "HTTP/1.1 200 OK\r\n";
                    baResp += "Server: " + QuazaaGlobals::USER_AGENT_STRING() + "\r\n";
                    baResp += "Connection: close\r\n";
                    baResp += "Content-Length: " + QString::number(f.size()) + "\r\n";
                    baResp += "\r\n";

                    baResp += f.readAll();
                    f.close();

                    Write(baResp);
                }
            }
        }

        // TODO: Handle P2P uploads here

        if( !bFound )
        {
            baResp += "HTTP/1.1 404 Not found\r\n";
            baResp += "Server: " + QuazaaGlobals::USER_AGENT_STRING() + "\r\n";
            baResp += "Connection: close\r\n";
			baResp += "Content-Type: text/plain\r\n";
            baResp += "\r\n";
			baResp += "The requested file was not found on this server.\r\n";
#ifdef _DEBUG
			baResp += sPath;
#endif
            Write(baResp);
        }
    }
    Close(true);

}
