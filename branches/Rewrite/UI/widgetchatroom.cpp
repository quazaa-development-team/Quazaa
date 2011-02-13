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

#include "ircutil.h"

#include <QDesktopServices>
#include <QStringList>

WidgetChatRoom::WidgetChatRoom(QuazaaIRC* quazaaIrc, Irc::Buffer* buffer, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatRoom)
{
	ui->setupUi(this);
	m_oQuazaaIrc = quazaaIrc;
	roomBuffer = buffer;
	chatUserListModel = new ChatUserListModel();
	buffer->names();
	connect(roomBuffer, SIGNAL(messageReceived(QString,QString)), this, SLOT(messageReceived(QString,QString)));
	connect(roomBuffer, SIGNAL(ctcpActionReceived(QString,QString)), this, SLOT(ctcpActionReceived(QString,QString)));
	connect(roomBuffer, SIGNAL(noticeReceived(QString,QString)), this, SLOT(noticeReceived(QString,QString)));
	connect(roomBuffer, SIGNAL(joined(QString)), this, SLOT(joined(QString)));
	connect(roomBuffer, SIGNAL(topicChanged(QString,QString)), this, SLOT(onTopicChanged(QString,QString)));
	connect(roomBuffer, SIGNAL(numericMessageReceived(QString,uint,QStringList)), this ,SLOT(numericMessageReceived(QString,uint,QStringList)));
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

void WidgetChatRoom::userNames(QStringList userNames)
{
	for (int i = 0; i < userNames.size(); i++);
	QString sNameStore		= userNames.at(3);
	QStringList userList	= sNameStore.split(" ");

	chatUserListModel->addUsers(roomBuffer->names());
	//emit updateUserCount(chatUserListModel->nOperatorCount, chatUserListModel->nUserCount);
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


void WidgetChatRoom::appendMessage(Irc::Buffer* buffer, QString sender, QString message, QuazaaIRC::Event event)
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

	//systemLog.postLog(LogSeverity::Debug, QString("Got a message from IRC buffer %1 | sender = %2 | event = %3").arg(buffer->receiver()).arg(sender).arg(evendt));
	//qDebug() << "Got a message from buffer " + (buffer->receiver()) + " | sender = " + sender + "| event = " + evendt;
	QString receiver = buffer->receiver();
	Irc::Util util;

	switch(event)
	{
	case QuazaaIRC::Message:
		ui->textBrowser->append("&lt;" + util.nickFromTarget(sender) + "&gt; " + util.messageToHtml(message, qApp->palette().foreground().color().name(), true, true, true));
		break;

	case QuazaaIRC::Notice:
		ui->textBrowser->append(util.nickFromTarget(sender) + ": " + util.messageToHtml(message, QColor("olive").name(), true, true, true));
		break;

	case QuazaaIRC::Action:
		ui->textBrowser->append("* " + util.nickFromTarget(sender) + " " + util.messageToHtml(message, QColor("purple").name(), true, true, true));
		break;

	case QuazaaIRC::Status:
		//WidgetChatTab *ctab  = qobject_cast<WidgetChatTab*>(ui->tabWidget->widget(0));
		//qDebug() << "STATUSMESSAGE : "+buffer->receiver() + "|"+sender+"|"+message;
		//tab->append(message);
		ui->textBrowser->append(util.messageToHtml(message, qApp->palette().foreground().color().name(), true, true, true));
	break;

	default:
		systemLog.postLog(LogSeverity::Debug, QString("WidgetChatCenter::appendMessage: No event!"));
		//qDebug() << "This should not happen!";
		break;
	}
}

void WidgetChatRoom::onTopicChanged(QString origin, QString topic)
{
	ui->lineEditTopic->setText(topic);
}

void WidgetChatRoom::numericMessageReceived(QString sender, uint code, QStringList list)
{
	switch (code)
	{
		case Irc::Rfc::RPL_NAMREPLY:
			userNames(list);
		break;
		case Irc::Rfc::RPL_BOUNCE:
		{
			for (int i = 0 ; i<list.size() ; ++i) {
				QString opt = list.at(i);
				if (opt.startsWith("PREFIX=", Qt::CaseInsensitive))
				{
					QString prefstr	= opt.split("=")[1];
					QString modes	= prefstr.mid(1, prefstr.indexOf(")")-1);
					QString mprefs	= prefstr.right(modes.length());
					setPrefixes(modes, mprefs);
				}
			}
		}
		default:
		{
			// append to status
			list.removeFirst();
			appendMessage(qobject_cast<Irc::Buffer*>(QObject::sender()), sender, "[" + QString::number(code) + "] " + list.join(" "), QuazaaIRC::Status);
		}
	}
}


void WidgetChatRoom::setPrefixes(QString modes, QString mprefs)
{
	// overkill ?
	prefixModes = modes;
	prefixChars = mprefs;
}

void WidgetChatRoom::noticeReceived(QString sender, QString message)
{
	appendMessage(roomBuffer, sender, message, QuazaaIRC::Notice);
}

void WidgetChatRoom::messageReceived(QString sender, QString message)
{
	//qDebug() << "Emitting messageReceived from quazaairc.cpp";
	appendMessage(roomBuffer, sender, message, QuazaaIRC::Message);
}

void WidgetChatRoom::ctcpActionReceived(QString sender, QString message)
{
	appendMessage(roomBuffer, sender, message, QuazaaIRC::Action);
}
