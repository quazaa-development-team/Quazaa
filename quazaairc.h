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

#include <ircsession.h>
#include <ircbuffer.h>

class QuazaaIRC : public QObject
{
public:
	QuazaaIRC();
};

class MyIrcSession : public Irc::Session
{
    Q_OBJECT

public:
    MyIrcSession(QObject* parent = 0);

protected Q_SLOTS:
    void on_connected();
    void on_disconnected();

    void on_bufferAdded(Irc::Buffer* buffer);
    void on_bufferRemoved(Irc::Buffer* buffer);

protected:
    virtual Irc::Buffer* createBuffer(const QString& receiver);
};

class MyIrcBuffer : public Irc::Buffer
{
    Q_OBJECT

public:
    MyIrcBuffer(const QString& receiver, Irc::Session* parent);

protected Q_SLOTS:
    void on_receiverChanged(const QString& receiver);
    void on_joined(const QString& origin);
    void on_parted(const QString& origin, const QString& message);
    void on_quit(const QString& origin, const QString& message);
    void on_nickChanged(const QString& origin, const QString& nick);
    void on_modeChanged(const QString& origin, const QString& mode, const QString& args);
    void on_topicChanged(const QString& origin, const QString& topic);
    void on_invited(const QString& origin, const QString& receiver, const QString& channel);
    void on_kicked(const QString& origin, const QString& nick, const QString& message);
    void on_messageReceived(const QString& origin, const QString& message);
    void on_noticeReceived(const QString& origin, const QString& notice);
    void on_ctcpRequestReceived(const QString& origin, const QString& request);
    void on_ctcpReplyReceived(const QString& origin, const QString& reply);
    void on_ctcpActionReceived(const QString& origin, const QString& action);
    void on_numericMessageReceived(const QString& origin, uint code, const QStringList& params);
    void on_unknownMessageReceived(const QString& origin, const QStringList& params);
};

#endif // QUAZAAIRC_H
