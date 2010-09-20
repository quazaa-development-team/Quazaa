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
#include "dialogsettings.h"

#include <QSslSocket>

QuazaaIRC::QuazaaIRC(QObject* parent)
{
	Q_UNUSED(parent);
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
        connect(buffer, SIGNAL(receiverChanged(QString)), this, SLOT(receiverChanged()));
        connect(buffer, SIGNAL(joined(QString)), this, SLOT(msgJoined(QString)));
        connect(buffer, SIGNAL(parted(QString, QString)), this, SLOT(msgParted(QString, QString)));
        connect(buffer, SIGNAL(quit(QString, QString)), this, SLOT(msgQuit(QString, QString)));
        connect(buffer, SIGNAL(nickChanged(QString, QString)), this, SLOT(msgNickChanged(QString, QString)));
        connect(buffer, SIGNAL(modeChanged(QString, QString, QString)), this, SLOT(msgModeChanged(QString, QString, QString)));
        connect(buffer, SIGNAL(topicChanged(QString, QString)), this, SLOT(msgTopicChanged(QString, QString)));
        connect(buffer, SIGNAL(invited(QString, QString, QString)), this, SLOT(msgInvited(QString, QString, QString)));
        connect(buffer, SIGNAL(kicked(QString, QString, QString)), this, SLOT(msgKicked(QString, QString, QString)));
        connect(buffer, SIGNAL(messageReceived(QString, QString)), this, SLOT(on_messageReceived(QString, QString)));
        connect(buffer, SIGNAL(noticeReceived(QString, QString)), this, SLOT(msgNoticeReceived(QString, QString)));
        connect(buffer, SIGNAL(ctcpRequestReceived(QString, QString)), this, SLOT(msgCtcpRequestReceived(QString, QString)));
        connect(buffer, SIGNAL(ctcpReplyReceived(QString, QString)), this, SLOT(msgCtcpReplyReceived(QString, QString)));
        connect(buffer, SIGNAL(ctcpActionReceived(QString, QString)), this, SLOT(msgCtcpActionReceived(QString, QString)));
        connect(buffer, SIGNAL(numericMessageReceived(QString, uint, QStringList)), this, SLOT(msgNumericMessageReceived(QString, uint, QStringList)));
        connect(buffer, SIGNAL(unknownMessageReceived(QString, QStringList)), this, SLOT(msgUnknownMessageReceived(QString, QStringList)));

	qDebug() << "buffer added:" << buffer->receiver();
}

void QuazaaIRC::on_bufferRemoved(Irc::Buffer* buffer)
{
	qDebug() << "buffer removed:" << buffer->receiver();
}

void QuazaaIRC::on_messageReceived(QString a, QString b) {
    qDebug() << "A: "+a+" B: "+b;
    emit appendMessage(a+" --> "+b);
}

void QuazaaIRC::startIrc(bool useSsl, QString ircNick, QString ircRealName, QString ircServer, int ircPort)
{
	qDebug() << "QuazaaIRC::startIrc() " << ircServer;
	ircSession = new Irc::Session(this);
        // echomessages , stripnicks
        ircSession->setOptions(ircSession->EchoMessages);

	if (useSsl)
	{
		QSslSocket* socket = new QSslSocket(ircSession);
		socket->ignoreSslErrors();
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);
		ircSession->setSocket(socket);
	}

        connect(ircSession, SIGNAL(bufferAdded(Irc::Buffer*)), this, SLOT(on_bufferAdded(Irc::Buffer*)));
	connect(ircSession, SIGNAL(connected()), this, SLOT(on_connected()));
	ircSession->setNick(ircNick);
	ircSession->setIdent("QuazaaIRC");
	ircSession->setRealName(ircRealName);

	if (useSsl)
	{
		ircSession->connectToServer(ircServer, ircPort);
	} else {
		ircSession->connectToServer(ircServer, ircPort);
	}
}

void QuazaaIRC::stopIrc()
{
        qDebug() << "QuazaaIRC::stopIrc()";
	ircSession->disconnectFromServer();
}

void QuazaaIRC::sendIrcMessage(QString message)
{
	ircSession->message("#quazaa-dev", message);
}
