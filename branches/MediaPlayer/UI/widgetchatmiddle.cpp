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
	connect(quazaaIrc, SIGNAL(appendMessage(Irc::Buffer*, QString, QString, QuazaaIRC::Event)), this, SLOT(appendMessage(Irc::Buffer*, QString, QString, QuazaaIRC::Event)));
	connect(quazaaIrc, SIGNAL(userNames(QStringList)), this, SLOT(userNames(QStringList)));
	connect(quazaaIrc, SIGNAL(bufferAdded(QString)), this, SLOT(addBuffer(QString)));
	connect(quazaaIrc, SIGNAL(setPrefixes(QString, QString)), this, SLOT(setPrefixes(QString, QString)));
	//connect(quazaaIrc, SIGNAL(joined(QString)), this, SLOT(joined(QString)));

	widgetChatInput = new WidgetChatInput(this);
	connect(widgetChatInput, SIGNAL(messageSent(QTextDocument*)), this, SLOT(onSendMessage(QTextDocument*)));
	ui->horizontalLayoutTextInput->addWidget(widgetChatInput);
	WidgetChatRoom* wct = new WidgetChatRoom(quazaaIrc, this);
	channelListModel = new QStringListModel();
	ui->stackedWidgetChatRooms->addWidget(wct);
	wct->setRoomName("*status"); // * is not allowed by RFC (IIRC)
	channelList << "Status";
	channelListModel->setStringList(channelList);
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

void WidgetChatMiddle::appendMessage(Irc::Buffer* buffer, QString sender, QString message, QuazaaIRC::Event event)
{
	QString evendt;
	if(event == QuazaaIRC::Message)
	{
		evendt = "message";
	}
	else if(event == QuazaaIRC::Notice)
	{
		evendt = "notice";
	}
	else if(event == QuazaaIRC::Action)
	{
		evendt = "action";
	}
	else
	{
		evendt = "server";
	}

	systemLog.postLog(LogSeverity::Debug, QString("Got a message from IRC buffer %1 | sender = %2 | event = %3").arg(buffer->receiver()).arg(sender).arg(evendt));
	//qDebug() << "Got a message from buffer " + (buffer->receiver()) + " | sender = " + sender + "| event = " + evendt;
	QString receiver = buffer->receiver();

	switch(event)
	{
	case QuazaaIRC::Message:
		roomByName(receiver)->append("&lt;" + Irc::Util::nickFromTarget(sender) + "&gt; " + message);
		break;

	case QuazaaIRC::Notice:
		currentRoom()->append("<html><font color=blue>" + Irc::Util::nickFromTarget(sender) + ": " + message + "</font></html>");
		break;

	case QuazaaIRC::Action:
		roomByName(receiver)->append("<html><font color=purple>* " + Irc::Util::nickFromTarget(sender) + " " + message + "</font></html>");
		break;

	case QuazaaIRC::Status:
		//WidgetChatTab *ctab  = qobject_cast<WidgetChatTab*>(ui->tabWidget->widget(0));
		//qDebug() << "STATUSMESSAGE : "+buffer->receiver() + "|"+sender+"|"+message;
		//tab->append(message);
		roomByName("*status")->append(message);
	break;

	default:
		systemLog.postLog(LogSeverity::Debug, QString("WidgetChatCenter::appendMessage: No event!"));
		//qDebug() << "This should not happen!";
		break;
	}
}

WidgetChatRoom* WidgetChatMiddle::roomByName(QString roomName)
{
	WidgetChatRoom* room;
	QList<WidgetChatRoom*> allRooms = ui->stackedWidgetChatRooms->findChildren<WidgetChatRoom*>();
	for(int i = 0; i < allRooms.size(); ++i)
	{
		if(allRooms.at(i)->roomName == roomName)
		{
			room = allRooms.at(i);
			return room;
		}
	}
	systemLog.postLog(LogSeverity::Debug, QString("WidgetChatMiddle Creating a new tab: %1").arg(roomName));
	//qDebug() << "CREATING A NEW TAB :: " + name;
	// if the tab doesn't exist, create it
	room = new WidgetChatRoom(quazaaIrc);
	room->setRoomName(roomName);
	ui->stackedWidgetChatRooms->addWidget(room);
	channelList << roomName;
	channelListModel->setStringList(channelList);
	emit roomChanged(room);
	return room;
}

void WidgetChatMiddle::userNames(QStringList names)
{
	int operators;
	WidgetChatRoom* room	= roomByName(names.at(2));
	QString namestr		= names.at(3);
	QStringList userList	= namestr.split(" ");

	userList.sort();
	QStringList ownerList;
	QStringList administratorList;
	QStringList operatorList;
	QStringList halfOperatorList;
	QStringList voiceList;
	QStringList regularList;

	for(int i = 0; i < userList.size(); ++i)
	{
		QString user = userList.at(i);
		if (user.at(0) == '~')
		{
			operators ++;
			ownerList << user;
		}
		else if (user.at(0) == '&')
		{
			operators ++;
			administratorList << user;
		}
		else if (user.at(0) == '@')
		{
			operators ++;
			operatorList << user;
		}
		else if (user.at(0) == '%')
		{
			operators ++;
			halfOperatorList << user;
		}
		else if (user.at(0) == '+')
		{
			voiceList << user;
		}
		else
		{
			regularList << user;
		}
	}
	
	QStringList sortedUserList;
	sortedUserList << caseInsensitiveSecondarySort(ownerList) << caseInsensitiveSecondarySort(administratorList)
			<< caseInsensitiveSecondarySort(operatorList) << caseInsensitiveSecondarySort(halfOperatorList)
			<< caseInsensitiveSecondarySort(voiceList) << caseInsensitiveSecondarySort(regularList);

	room->operators = operators;
	room->users = sortedUserList.count();
	emit updateUserCount(operators, sortedUserList.count());
	room->userNames(sortedUserList);
}

QStringList WidgetChatMiddle::caseInsensitiveSecondarySort(QStringList list)
{
	 QMap<QString, QString> map;
	 foreach (QString str, list)
		 map.insert(str.toLower(), str);

	 list = map.values();
	 return list;
}

WidgetChatRoom* WidgetChatMiddle::currentRoom()
{
	systemLog.postLog(LogSeverity::Debug, QString("getting WidgetChatMiddle::currentTab()"));
	//qDebug() << "getting WidgetChatCenter::currentTab()";
	return qobject_cast<WidgetChatRoom*>(ui->stackedWidgetChatRooms->currentWidget());
}

void WidgetChatMiddle::setPrefixes(QString modes, QString mprefs)
{
	// overkill ?
	prefixModes = modes;
	prefixChars = mprefs;
}

void WidgetChatMiddle::addBuffer(QString name)
{
	if(name.at(0) == '#')
	{
		// tab will be auto-created
		ui->stackedWidgetChatRooms->setCurrentWidget(roomByName(name));
	}
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
	qDebug() << "WidgetchatCenter::onSendMessage triggered";
	currentRoom()->onSendMessage(message->toPlainText());
}

void WidgetChatMiddle::changeRoom(int index)
{
	ui->stackedWidgetChatRooms->setCurrentIndex(index);
	emit roomChanged(currentRoom());
}
