//
// widgetchatcenter.h
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
	WidgetChatRoom* roomByName(QString);
	WidgetChatRoom* roomByBuffer(Irc::Buffer* buffer);
	WidgetChatRoom* currentRoom();

private slots:
	void on_actionDisconnect_triggered();
	void on_actionConnect_triggered();
	void on_actionChatSettings_triggered();
	void onSendMessage(QTextDocument *message);
	void addBuffer(Irc::Buffer *buffer);
	void on_stackedWidgetChatRooms_currentChanged(QWidget*);
	void on_actionEditMyProfile_triggered();
	void changeRoom(int index);
	//QStringList caseInsensitiveSecondarySort(QStringList list);
};

#endif // WIDGETCHATCENTER_H
