//
// quazaairc.cpp
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

#include "types.h"
#include "quazaairc.h"

#include <ircsession.h>
#include <ircbuffer.h>
#include <QSslSocket>
#include <QMetaObject>

QuazaaIRC::QuazaaIRC(QObject* parent) :
		QObject(parent)
{
}

void QuazaaIRC::on_IrcSession_connected()
{
	qDebug() << "IRC connected:";
}

void QuazaaIRC::on_IrcSession_disconnected()
{
	qDebug() << "IRC disconnected:";
}
void QuazaaIRC::on_IrcSession_welcomed()
{
	qDebug() << "IRC welcomed";
	ircSession->join("#quazaa-dev");
}

void QuazaaIRC::on_IrcSession_bufferAdded(Irc::Buffer* buffer)
{
	qDebug() << "buffer added:" << buffer->receiver();
}

void QuazaaIRC::on_IrcSession_bufferRemoved(Irc::Buffer* buffer)
{
	qDebug() << "buffer removed:" << buffer->receiver();
}



void QuazaaIRC::startIrc(bool useSsl, QString ircNick, QString ircRealName, QString ircServer, int ircPort)
{
	qDebug() << "QuazaaIRC::startIrc() " << ircServer;
	ircSession = new Irc::Session(this);

	if (useSsl)
	{
		QSslSocket* socket = new QSslSocket(ircSession);
		socket->ignoreSslErrors();
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);
		ircSession->setSocket(socket);
	}

	// for connectSlotsByName, to get it working, all slots should be named like:
	// on_IrcSession_<signal name>
	ircSession->setObjectName("IrcSession");
	QMetaObject::connectSlotsByName(this);

	ircSession->setNick(ircNick);
	ircSession->setIdent("QuazaaIRC");
	ircSession->setRealName(ircRealName);

	ircSession->connectToServer(ircServer, ircPort);

}

void QuazaaIRC::stopIrc()
{
	qDebug() << "QuazaaIRC::stopIrc()";

	if( ircSession )
	{
		ircSession->part("#quazaa-dev", "Client exited.");
		ircSession->disconnectFromServer();
		ircSession->deleteLater();
		ircSession = 0;
	}
}

void QuazaaIRC::sendIrcMessage(QString message)
{
	ircSession->message("#quazaa-dev", message);
}
void QuazaaIRC::on_IrcSession_messageReceived(QString a, QString b)
{
	qDebug() << "A: "+a+" B: "+b;
	emit appendMessage(a+" --> "+b);
}
