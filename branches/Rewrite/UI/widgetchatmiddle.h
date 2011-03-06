/*
** widgetchatmiddle.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
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

#ifndef WIDGETCHATCENTER_H
#define WIDGETCHATCENTER_H

#include <QMainWindow>
#include <QLineEdit>
#include <QToolButton>
#include <QTextDocument>

#include "quazaairc.h"
#include "widgetchatroom.h"
#include "widgetchatinput.h"

namespace Ui
{
	class WidgetChatMiddle;
}

class WidgetChatMiddle : public QMainWindow
{
	Q_OBJECT
public:
	WidgetChatMiddle(QWidget* parent = 0);
	~WidgetChatMiddle();
	QuazaaIRC* quazaaIrc;
	WidgetChatInput *widgetChatInput;
	QStringListModel *channelListModel;

signals:
	void roomChanged(WidgetChatRoom* room);
	void messageSent(QString *text);
	void updateUserCount(int operators, int users);

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetChatMiddle* ui;
	QStringList channelList;

public slots:
	void saveWidget();
	WidgetChatRoom* roomByName(QString roomName,Irc::Buffer *buffer);
	WidgetChatRoom* roomByBuffer(Irc::Buffer* buffer);
	WidgetChatRoom* currentRoom();

private slots:
	void on_actionDisconnect_triggered();
	void on_actionConnect_triggered();
	void on_actionChatSettings_triggered();
	void onSendMessage(QTextDocument *message);
	void addBuffer(Irc::Buffer *buffer);
	void removeBuffer(Irc::Buffer* buffer);
	void on_stackedWidgetChatRooms_currentChanged(int);
	void on_actionEditMyProfile_triggered();
	void changeRoom(int index);
	void userCountUpdated(ChatUserListModel* chatUserListModel, int operators, int users);
};

#endif // WIDGETCHATCENTER_H
