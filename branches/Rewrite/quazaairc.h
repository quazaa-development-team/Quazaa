//
// quazaairc.h
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

#ifndef QUAZAAIRC_H
#define QUAZAAIRC_H

#include <QtGui>

namespace Irc
{
	class Session;
	class Buffer;
}

namespace IrcEvent {
	enum IrcEvent { Command, Status, Notice, Message, Action, Server };
}

class QuazaaIRC : public QObject
{
	Q_OBJECT
	Q_ENUMS(Event)

protected:
	bool m_connected;


public:
	QuazaaIRC(QObject* parent = 0);
	QString sServer;

public slots:
	void startIrc( bool useSsl, QString ircNick, QString ircRealName, QString ircServer, int ircPort );
	void stopIrc();
	void sendIrcMessage(QString channel, QString message);

signals:
	void bufferAdded(Irc::Buffer* buffer);
	void joined(QString chan);

protected slots:
	void on_IrcSession_connected();
	void on_IrcSession_disconnected();
	void on_IrcSession_welcomed();

	void on_IrcSession_bufferAdded(Irc::Buffer* buffer);
	void on_IrcSession_bufferRemoved(Irc::Buffer* buffer);
	void ctcpReply(QString nick, QString reply);

protected:
	Irc::Session *ircSession;
};

#endif //QUAZAAIRC_H
