//
// widgetchatcenter.cpp
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

#include "widgetchat.h"
#include "ui_widgetchat.h"
#include "widgetchatmiddle.h"
#include "ui_widgetchatmiddle.h"
#include "dialogsettings.h"
#include "dialogprofile.h"
#include "systemlog.h"
#include "chatconverter.h"

#include "quazaasettings.h"
#include "ircbuffer.h"
#include "ircutil.h"
#include "ircsession.h"
#include "quazaairc.h"
 

WidgetChatMiddle::WidgetChatMiddle(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatMiddle)
{
	ui->setupUi(this);
	quazaaIrc = new QuazaaIRC();
	if(quazaaSettings.Chat.ConnectOnStartup)
	{
		quazaaIrc->startIrc(quazaaSettings.Chat.IrcUseSSL, quazaaSettings.Profile.IrcNickname, quazaaSettings.Profile.IrcUserName, quazaaSettings.Chat.IrcServerName, quazaaSettings.Chat.IrcServerPort);
		ui->actionConnect->setEnabled(false);
		ui->actionDisconnect->setEnabled(true);
		systemLog.postLog(LogSeverity::Debug, QString("Trying to connect to IRC"));
		//qDebug() << "Trying to connect to IRC";
	}
	else
	{
		ui->actionConnect->setEnabled(true);
		ui->actionDisconnect->setEnabled(false);
	}
	restoreState(quazaaSettings.WinMain.ChatToolbars);
	connect(quazaaIrc, SIGNAL(bufferAdded(Irc::Buffer*)), this, SLOT(addBuffer(Irc::Buffer*)));
	connect(quazaaIrc, SIGNAL(setPrefixes(QString, QString)), this, SLOT(setPrefixes(QString, QString)));
	//connect(quazaaIrc, SIGNAL(joined(QString)), this, SLOT(joined(QString)));

	widgetChatInput = new WidgetChatInput(this, true);
	connect(widgetChatInput, SIGNAL(messageSent(QTextDocument*)), this, SLOT(onSendMessage(QTextDocument*)));
	ui->horizontalLayoutTextInput->addWidget(widgetChatInput);
	channelListModel = new QStringListModel();
}

WidgetChatMiddle::~WidgetChatMiddle()
{
	if(ui->actionDisconnect->isEnabled())
	{
		quazaaIrc->stopIrc();
	}
	delete ui;
}

void WidgetChatMiddle::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WidgetChatMiddle::saveWidget()
{
	quazaaSettings.WinMain.ChatToolbars = saveState();
}

void WidgetChatMiddle::on_actionConnect_triggered()
{
	quazaaIrc->startIrc(quazaaSettings.Chat.IrcUseSSL, quazaaSettings.Profile.IrcNickname, quazaaSettings.Profile.IrcUserName, quazaaSettings.Chat.IrcServerName, quazaaSettings.Chat.IrcServerPort);
	ui->actionConnect->setEnabled(false);
	ui->actionDisconnect->setEnabled(true);
	systemLog.postLog(LogSeverity::Debug, QString("Trying to connect to IRC"));
	//qDebug() << "Trying to connect to IRC";
}

void WidgetChatMiddle::on_actionChatSettings_triggered()
{
	DialogSettings* dlgSettings = new DialogSettings(this, SettingsPage::Chat);
	dlgSettings->show();
}

void WidgetChatMiddle::on_actionDisconnect_triggered()
{
	quazaaIrc->stopIrc();
	ui->actionConnect->setEnabled(true);
	ui->actionDisconnect->setEnabled(false);
	systemLog.postLog(LogSeverity::Debug, QString("Trying to disconnect from IRC"));
	//qDebug() << "Trying to disconnect from IRC";
}

WidgetChatRoom* WidgetChatMiddle::roomByName(QString roomName)
{
	QList<WidgetChatRoom*> allRooms = ui->stackedWidgetChatRooms->findChildren<WidgetChatRoom*>();
	for(int i = 0; i < allRooms.size(); ++i)
	{
		if(allRooms.at(i)->roomName == roomName)
		{
			return allRooms.at(i);
		}
	}

	return 0;
}

WidgetChatRoom* WidgetChatMiddle::roomByBuffer(Irc::Buffer* buffer)
{
	QList<WidgetChatRoom*> allRooms = ui->stackedWidgetChatRooms->findChildren<WidgetChatRoom*>();
	for(int i = 0; i < allRooms.size(); ++i)
	{
		if(allRooms.at(i)->roomName == buffer->receiver())
		{
			return allRooms.at(i);
		}
	}
	systemLog.postLog(LogSeverity::Debug, QString("WidgetChatMiddle Creating a new room: %1").arg(buffer->receiver()));
	//qDebug() << "CREATING A NEW TAB :: " + name;
	// if the tab doesn't exist, create it
	WidgetChatRoom *room = new WidgetChatRoom(quazaaIrc, buffer);
	room->setRoomName(buffer->receiver());
	ui->stackedWidgetChatRooms->addWidget(room);
	channelList << buffer->receiver();
	channelListModel->setStringList(channelList);
	emit roomChanged(room);
	return room;
}

WidgetChatRoom* WidgetChatMiddle::currentRoom()
{
	//systemLog.postLog(LogSeverity::Debug, QString("getting WidgetChatMiddle::currentTab()"));
	//qDebug() << "getting WidgetChatCenter::currentTab()";
	return qobject_cast<WidgetChatRoom*>(ui->stackedWidgetChatRooms->currentWidget());
}

void WidgetChatMiddle::addBuffer(Irc::Buffer* buffer)
{
	ui->stackedWidgetChatRooms->setCurrentWidget(roomByBuffer(buffer));
}



void WidgetChatMiddle::on_stackedWidgetChatRooms_currentChanged(QWidget*)
{
	//qDebug() << "Emitting channel changed.";
	emit roomChanged(currentRoom());
}

void WidgetChatMiddle::on_actionEditMyProfile_triggered()
{
	DialogProfile* dlgProfile = new DialogProfile(this);
	dlgProfile->show();
}


void WidgetChatMiddle::onSendMessage(QTextDocument *message)
{
	CChatConverter oConv(message);
	currentRoom()->onSendMessage(oConv.toIRC());
}

void WidgetChatMiddle::changeRoom(int index)
{
	ui->stackedWidgetChatRooms->setCurrentIndex(index);
	emit roomChanged(currentRoom());
}
