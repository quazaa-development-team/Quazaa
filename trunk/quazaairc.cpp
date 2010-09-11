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

#include <QSslSocket>

QuazaaIRC::QuazaaIRC(QObject* parent)
{

}

void QuazaaIRC::on_connected()
{
	qDebug() << "IRC connected:";
	ircSession->join("#quazaa-dev");
}

void QuazaaIRC::on_disconnected()
{
	qDebug() << "IRC disconnected:";
}

void QuazaaIRC::on_bufferAdded(Irc::Buffer* buffer)
{
	qDebug() << "buffer added:" << buffer->receiver();
}

void QuazaaIRC::on_bufferRemoved(Irc::Buffer* buffer)
{
	qDebug() << "buffer removed:" << buffer->receiver();
}



bool QuazaaIRC::startIrc(bool useSsl, QString ircNick, QString ircRealName, QString ircServer)
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

	connect(ircSession, SIGNAL(connected()), this, SLOT(on_connected()));
	ircSession->setNick(ircNick);
	ircSession->setIdent("Quazaa IRC");
	ircSession->setRealName(ircRealName);

	if (useSsl)
	{
		ircSession->connectToServer(ircServer, 6667);
	} else {
		ircSession->connectToServer(ircServer, 6669);
	}

	return true;
}
