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
#include "ircutil.h"
#include "widgetchatroom.h"

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
	systemLog.postLog(LogSeverity::Debug, QString("IRC connected."));
	//qDebug() << "IRC connected:";
	m_connected = true;
}

void QuazaaIRC::on_IrcSession_disconnected()
{
	systemLog.postLog(LogSeverity::Debug, QString("IRC disconnected."));
	//qDebug() << "IRC disconnected:";
	m_connected = false;
}

void QuazaaIRC::on_IrcSession_welcomed()
{
	systemLog.postLog(LogSeverity::Debug, QString("IRC welcomed."));
	//qDebug() << "IRC welcomed";
	ircSession->join("#quazaa-dev");
}

void QuazaaIRC::on_IrcSession_bufferAdded(Irc::Buffer* buffer)
{
	systemLog.postLog(LogSeverity::Debug, QString("IRC buffer added: %1").arg(buffer->receiver()));
	//qDebug() << "buffer added:" << buffer->receiver();
	emit bufferAdded(buffer);
	buffer->names();
	connect(buffer, SIGNAL(joined(QString)), this, SIGNAL(joined(QString)));
	connect(buffer, SIGNAL(ctcpRequestReceived(QString,QString)), this, SLOT(ctcpReply(QString,QString)));
}

void QuazaaIRC::on_IrcSession_bufferRemoved(Irc::Buffer* buffer)
{
	systemLog.postLog(LogSeverity::Debug, QString("IRC buffer removed: %1").arg(buffer->receiver()));
	//qDebug() << "buffer removed:" << buffer->receiver();
}

void QuazaaIRC::startIrc(bool useSsl, QString ircNick, QString ircRealName, QString ircServer, int ircPort)
{
	systemLog.postLog(LogSeverity::Debug, QString("QuazaaIRC::startIRC() %1").arg(ircServer));
	//qDebug() << "QuazaaIRC::startIrc() " << ircServer;
	ircSession = new Irc::Session(this);

	

	// stripNicks / echoMessages
	ircSession->setOptions(Irc::Session::EchoMessages);

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
	systemLog.postLog(LogSeverity::Debug, QString("QuazaaIRC::stopIRC()"));
	//qDebug() << "QuazaaIRC::stopIrc()";

	if( ircSession )
	{
		ircSession->disconnectFromServer();
		ircSession->deleteLater();
		ircSession = 0;
	}
}

void QuazaaIRC::sendIrcMessage(QString channel, QString message)
{
	//qDebug() << "Sending message to: " + tab->name + " ("+message+")";
	if( m_connected ) ircSession->message(channel, message);

}

void QuazaaIRC::ctcpReply(QString nick, QString reply)
{
	reply = "VERSION Quazaa 0.1a Copyright© Quazaa Development Team, 2009-2011";
	ircSession->ctcpReply(nick, reply);
}
