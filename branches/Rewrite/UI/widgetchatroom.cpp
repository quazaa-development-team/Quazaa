//
// widgetchatroom.cpp
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


#include "widgetchatroom.h"
#include "ui_widgetchatroom.h"
#include "systemlog.h"

#include <QDesktopServices>
#include <QStringList>

WidgetChatRoom::WidgetChatRoom(QuazaaIRC* quazaaIrc, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatRoom)
{
	ui->setupUi(this);
	m_oQuazaaIrc = quazaaIrc;
	chatUserListModel = new ChatUserListModel();
	operators = 0;
	users = 0;
}

WidgetChatRoom::~WidgetChatRoom()
{
	delete ui;
}

void WidgetChatRoom::changeEvent(QEvent* e)
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

void WidgetChatRoom::setRoomName(QString str)
{
	roomName = str;
}

void WidgetChatRoom::saveWidget()
{
}

void WidgetChatRoom::append(QString text)
{
	ui->textBrowser->append(text);
}

void WidgetChatRoom::userNames(QStringList userNames)
{
	//systemLog.postLog(LogSeverity::Debug, QString("WidgetChatRoom name is: %1").arg(roomName));
	//qDebug() << "My name is >> " + name;
	/*for (int i = 0; i < names.size(); ++i) {
		qDebug() << "CName: "+names.at(i);
		//ui->listWidgetChatUsers->addItem(names.at(i));
		//new QListWidgetItem(names.at(i), ui->listWidgetChatUsers);
	}
	*/
	chatUserListModel->addUsers(userNames);
	//userList->setStringList(userNames);
	//ui->listWidgetChatUsers->addItem()
}


void WidgetChatRoom::onSendMessage(QString message)
{
	m_oQuazaaIrc->sendIrcMessage(roomName, message);
}

void WidgetChatRoom::on_textBrowser_anchorClicked(QUrl link)
{
	QDesktopServices::openUrl(link);
}
