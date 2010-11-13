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
#include "widgetchattab.h"

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
	emit bufferAdded(buffer->receiver());
	buffer->names();
	connect(buffer, SIGNAL(messageReceived(QString, QString)), this, SLOT(messageReceived(QString, QString)));
	connect(buffer, SIGNAL(numericMessageReceived(QString, uint, QStringList)), this, SLOT(numericMessageReceived(QString, uint, QStringList)));
	connect(buffer, SIGNAL(ctcpActionReceived(QString, QString)), this, SLOT(ctcpActionReceived(QString, QString)));
	connect(buffer, SIGNAL(noticeReceived(QString, QString)), this, SLOT(noticeReceived(QString, QString)));
	connect(buffer, SIGNAL(joined(QString)), this, SIGNAL(joined(QString)));
}

void QuazaaIRC::on_IrcSession_bufferRemoved(Irc::Buffer* buffer)
{
	qDebug() << "buffer removed:" << buffer->receiver();
}

void QuazaaIRC::startIrc(bool useSsl, QString ircNick, QString ircRealName, QString ircServer, int ircPort)
{
	qDebug() << "QuazaaIRC::startIrc() " << ircServer;
	ircSession = new Irc::Session(this);
	// stripNicks / echoMessages
	ircSession->setOptions(Irc::Session::EchoMessages);

	if(useSsl)
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

	if(ircSession)
	{
		ircSession->disconnectFromServer();
		ircSession->deleteLater();
		ircSession = 0;
	}
}

void QuazaaIRC::sendIrcMessage(QString channel, QString message)
{
	//qDebug() << "Sending message to: " + tab->name + " ("+message+")";
	ircSession->message(channel, message);
}

void QuazaaIRC::noticeReceived(QString sender, QString message)
{
	emit appendMessage(qobject_cast<Irc::Buffer*>(QObject::sender()), sender, message, QuazaaIRC::Notice);
}

void QuazaaIRC::messageReceived(QString sender, QString message)
{
	qDebug() << "Emitting messageReceived from quazaairc.cpp";
	emit appendMessage(qobject_cast<Irc::Buffer*>(QObject::sender()), sender, message, QuazaaIRC::Message);
}

void QuazaaIRC::ctcpActionReceived(QString sender, QString message)
{
	emit appendMessage(qobject_cast<Irc::Buffer*>(QObject::sender()), sender, message, QuazaaIRC::Action);
}

void QuazaaIRC::numericMessageReceived(QString sender, uint code, QStringList list)
{
	switch(code)
	{
		case Irc::Rfc::RPL_NAMREPLY:
			emit channelNames(list);
			break;
		case Irc::Rfc::RPL_BOUNCE:
		{
			for(int i = 0 ; i < list.size() ; ++i)
			{
				QString opt = list.at(i);
				if(opt.startsWith("PREFIX=", Qt::CaseInsensitive))
				{
					QString prefstr	= opt.split("=")[1];
					QString modes	= prefstr.mid(1, prefstr.indexOf(")") - 1);
					QString mprefs	= prefstr.right(modes.length());
					emit setPrefixes(modes, mprefs);
				}
			}
		}
		default:
		{
			// append to status
			list.removeFirst();
			emit appendMessage(qobject_cast<Irc::Buffer*>(QObject::sender()), sender, "[" + QString::number(code) + "] " + list.join(" "), QuazaaIRC::Status);
		}
	}
}
