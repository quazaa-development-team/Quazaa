/*
** quazaairc.cpp
**
** Copyright  Quazaa Development Team, 2009-2011.
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

#include "types.h"
#include "quazaairc.h"
#include "ircutil.h"
#include "widgetchatroom.h"
#include "quazaaglobals.h"
#include "quazaasettings.h"

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
	bLoginCompleted = true;
}

void QuazaaIRC::on_IrcSession_bufferAdded(Irc::Buffer* buffer)
{
	//qDebug() << "buffer added:" << buffer->receiver();
	emit ircBufferAdded(buffer);
	buffer->names();
	connect(buffer, SIGNAL(ctcpRequestReceived(QString, QString)), this, SLOT(ctcpReply(QString, QString)));
	connect(buffer, SIGNAL(numericMessageReceived(QString, uint, QStringList)), this, SLOT(numericMessageReceived(QString, uint, QStringList)));
}

void QuazaaIRC::on_IrcSession_bufferRemoved(Irc::Buffer* buffer)
{
	emit ircBufferRemoved(buffer);
	//qDebug() << "buffer removed:" << buffer->receiver();
}

void QuazaaIRC::startIrc()
{
	systemLog.postLog(LogSeverity::Debug, QString("QuazaaIRC::startIRC() %1").arg(quazaaSettings.Chat.IrcServerName));
	//qDebug() << "QuazaaIRC::startIrc() " << ircServer;
	ircSession = new Irc::Session(this);
	sServer = quazaaSettings.Chat.IrcServerName;
	bLoginCompleted = false;



	// stripNicks / echoMessages
	ircSession->setOptions(Irc::Session::EchoMessages);

	if(quazaaSettings.Chat.IrcUseSSL)
	{
		QSslSocket* socket = new QSslSocket(ircSession);
		socket->ignoreSslErrors();
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);
		ircSession->setSocket(socket);
	}

	ircSession->setObjectName("IrcSession");
	connect(ircSession, SIGNAL(connected()), this, SLOT(on_IrcSession_connected()));
	connect(ircSession, SIGNAL(disconnected()), this, SLOT(on_IrcSession_disconnected()));
	connect(ircSession, SIGNAL(welcomed()), this, SLOT(on_IrcSession_welcomed()));
	connect(ircSession, SIGNAL(bufferAdded(Irc::Buffer*)), this, SLOT(on_IrcSession_bufferAdded(Irc::Buffer*)));
	connect(ircSession, SIGNAL(bufferRemoved(Irc::Buffer*)), this, SLOT(on_IrcSession_bufferRemoved(Irc::Buffer*)));

	ircSession->setNick(quazaaSettings.Profile.IrcNickname);
	sNick = quazaaSettings.Profile.IrcNickname;
	ircSession->setIdent("QuazaaIRC");
	ircSession->setRealName(quazaaSettings.Profile.RealName);
	sRealName = quazaaSettings.Profile.RealName;
	ircSession->setAutoJoinChannels(quazaaSettings.Chat.AutoJoinChannels);

	ircSession->connectToServer(quazaaSettings.Chat.IrcServerName, quazaaSettings.Chat.IrcServerPort);
}

void QuazaaIRC::stopIrc()
{
	systemLog.postLog(LogSeverity::Debug, QString("QuazaaIRC::stopIRC()"));
	//qDebug() << "QuazaaIRC::stopIrc()";

	if(ircSession)
	{
		ircSession->buffers().clear();
		ircSession->defaultBuffer()->deleteLater();
		ircSession->quit();
		ircSession = 0;
	}
}

void QuazaaIRC::sendIrcMessage(QString channel, QString message)
{
	//qDebug() << "Sending message to: " + tab->name + " ("+message+")";
	if(m_connected)
	{
		ircSession->message(channel, message);
	}

}

void QuazaaIRC::ctcpReply(QString nick, QString request)
{
	QStringList ctcp	= request.toUpper().split(" ");
	QString action		= ctcp.at(0);
	QString reply		= action + " ";

	if(action == "VERSION")
	{
		reply += QuazaaGlobals::USER_AGENT_STRING();
	}
	else if(action == "TIME")
	{
		reply += QDateTime::currentDateTime().toString("ddd MMM dd HH:mm:ss yyyy");
	}
	else if(action == "PING")
	{
		reply = request;
	}
	else if(action == "FINGER")
	{
		qsrand(time(0));
		QStringList fingers = QStringList()
		                      << tr("Go finger someone else...")
		                      << tr("You naughty!")
		                      << tr("Oh yeah baby :o")
		                      << tr("Quazaa really turns me on!");
		reply += fingers.at(qrand() % fingers.size());
	}
	else if(action == "SOURCE")
	{
		reply += "http://sourceforge.net/scm/?type=svn&group_id=286623";
	}
	else if(action == "USERINFO")
	{
		reply += sRealName;
	}
	else if(action == "CLIENTINFO")
	{
		reply += "ACTION CLIENTINFO PING TIME VERSION FINGER SOURCE USERINFO";
	}
	else
	{
		reply = "ERRMSG CTCP " + action + tr(" is an unknown request.");
	}

	ircSession->ctcpReply(nick, reply);
}

void QuazaaIRC::numericMessageReceived(QString sender, uint code, QStringList list)
{
	switch(code)
	{
		case Irc::Rfc::ERR_NICKNAMEINUSE:
			sNick = quazaaSettings.Profile.IrcAlternateNickname;
			ircSession->setNick(quazaaSettings.Profile.IrcAlternateNickname);
			break;
	}
}

void QuazaaIRC::addRoom(QString room)
{
	ircSession->join(room);
}

void QuazaaIRC::removeRoom(QString room)
{
	ircSession->part(room);
}
