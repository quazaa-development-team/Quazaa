//
// widgetchatroom.h
//
// Copyright  Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
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


#ifndef WIDGETCHATROOM_H
#define WIDGETCHATROOM_H

#include <QtGui>
#include "quazaairc.h"
#include "widgetchatinput.h"
#include "chatuserlistmodel.h"

#include "ircbuffer.h"

namespace Ui
{
	class WidgetChatRoom;
}

class WidgetChatRoom : public QMainWindow
{
	Q_OBJECT

public:
	explicit WidgetChatRoom(QuazaaIRC* quazaaIrc, Irc::Buffer* buffer, QWidget* parent = 0);
	~WidgetChatRoom();
	QString roomName;
	int operators, users;
	ChatUserListModel *chatUserListModel;

public slots:
	void saveWidget();
	void setRoomName(QString str);
	void userNames(QStringList names);
	void onSendMessage(QString message);

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetChatRoom* ui;
	QuazaaIRC* m_oQuazaaIrc;
	Irc::Buffer* roomBuffer;
	QString prefixChars, prefixModes;

private slots:
	void on_textBrowser_anchorClicked(QUrl link);
	void appendMessage(Irc::Buffer* buffer, QString sender, QString message, QuazaaIRC::Event event);
	void onTopicChanged(QString origin, QString topic);
	void numericMessageReceived(QString, uint, QStringList);
	void setPrefixes(QString modes, QString mprefs);
};

#endif // WIDGETCHATROOM_H
