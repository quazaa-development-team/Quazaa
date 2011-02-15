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
	connect(roomBuffer, SIGNAL(parted(QString,QString)), this, SLOT(parted(QString,QString)));
	connect(roomBuffer, SIGNAL(topicChanged(QString,QString)), this, SLOT(onTopicChanged(QString,QString)));
	connect(roomBuffer, SIGNAL(numericMessageReceived(QString,uint,QStringList)), this ,SLOT(numericMessageReceived(QString,uint,QStringList)));
	connect(roomBuffer, SIGNAL(namesReceived(QStringList)), this, SLOT(updateUsers()));
	connect(roomBuffer, SIGNAL(quit(QString,QString)), this, SLOT(leftServer(QString,QString)));
	connect(roomBuffer, SIGNAL(modeChanged(QString,QString,QString)), this, SLOT(updateUserMode(QString,QString,QString)));
	connect(roomBuffer, SIGNAL(nickChanged(QString,QString)), this, SLOT(nickChanged(QString,QString)));
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

void WidgetChatRoom::setRoomName(QString roomName)
{
	sRoomName = roomName;
}

void WidgetChatRoom::saveWidget()
{
}

void WidgetChatRoom::onSendMessage(QString message)
{
	m_oQuazaaIrc->sendIrcMessage(sRoomName, message);
}

void WidgetChatRoom::onSendAction(QString message)
{
	roomBuffer->ctcpAction(message);
}

void WidgetChatRoom::on_textBrowser_anchorClicked(QUrl link)
{
	QDesktopServices::openUrl(link);
}


void WidgetChatRoom::appendMessage(QString sender, QString message, IrcEvent::IrcEvent event)
{	//systemLog.postLog(LogSeverity::Debug, QString("Got a message from IRC buffer %1 | sender = %2 | event = %3").arg(buffer->receiver()).arg(sender).arg(evendt));
	//qDebug() << "Got a message from buffer " + (buffer->receiver()) + " | sender = " + sender + "| event = " + evendt;
	Irc::Util util;

	switch(event)
	{
	case IrcEvent::Command:

		break;
	case IrcEvent::Message:
		ui->textBrowser->append("&lt;" + util.nickFromTarget(sender) + "&gt; " + util.messageToHtml(message, qApp->palette().foreground().color().name(), true, true, true));
		break;
	case IrcEvent::Notice:
		ui->textBrowser->append(wrapWithColor(util.nickFromTarget(sender) + ": " + util.messageToHtml(message, qApp->palette().foreground().color().name(), true, true, true), QColor("red").name()));
		break;
	case IrcEvent::Action:
		ui->textBrowser->append(wrapWithColor("* " + util.nickFromTarget(sender) + " " + util.messageToHtml(message, qApp->palette().foreground().color().name(), true, true, true), QColor("blue").name()));
		break;
	case IrcEvent::Server:
		//WidgetChatTab *ctab  = qobject_cast<WidgetChatTab*>(ui->tabWidget->widget(0));
		//qDebug() << "STATUSMESSAGE : "+buffer->receiver() + "|"+sender+"|"+message;
		//tab->append(message);
		ui->textBrowser->append(wrapWithColor(util.messageToHtml(message, qApp->palette().foreground().color().name(), true, true, true), QColor("olive").name()));
		break;
	case IrcEvent::Status:
		ui->textBrowser->append(wrapWithColor(util.messageToHtml(message, qApp->palette().foreground().color().name(), true, true, true), QColor("purple").name()));
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
			emit userNames(list);
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
			appendMessage(sender, "[" + QString::number(code) + "] " + list.join(" "), IrcEvent::Server);
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
	appendMessage(sender, message, IrcEvent::Notice);
}

void WidgetChatRoom::messageReceived(QString sender, QString message)
{
	//qDebug() << "Emitting messageReceived from quazaairc.cpp";
	appendMessage(sender, message, IrcEvent::Message);
}

void WidgetChatRoom::ctcpActionReceived(QString sender, QString message)
{
	appendMessage(sender, message, IrcEvent::Action);
}

void WidgetChatRoom::joined(QString name)
{
	Irc::Util util;

	name = Irc::Util::nickFromTarget(name);
	ui->textBrowser->append(wrapWithColor(util.messageToHtml(
		 tr("%1 has joined this channel (%2).").arg(Irc::Util::nickFromTarget(name)).arg(name),
		 qApp->palette().foreground().color().name(), true, true, true), QColor("purple").name()));
	qDebug() << "User " << name << " joined room with mode " << roomBuffer->modes(name);
	chatUserListModel->addUser(name, roomBuffer->modes(name));
}

void WidgetChatRoom::parted(QString name, QString reason)
{
	Irc::Util util;
	name = Irc::Util::nickFromTarget(name);
	ui->textBrowser->append(wrapWithColor(util.messageToHtml(
		 tr("%1 has left this channel (%2).").arg(Irc::Util::nickFromTarget(name)).arg(reason),
		 qApp->palette().foreground().color().name(), true, true, true), QColor("purple").name()));
	chatUserListModel->removeUser(name);
}

void WidgetChatRoom::leftServer(QString name, QString reason)
{
	Irc::Util util;

	name = Irc::Util::nickFromTarget(name);
	ui->textBrowser->append(wrapWithColor(util.messageToHtml(
		tr("%1 has left this server (%2).").arg(Irc::Util::nickFromTarget(name)).arg(reason),
		qApp->palette().foreground().color().name(), true, true, true), QColor("purple").name()));
	chatUserListModel->removeUser(name);
}

void WidgetChatRoom::updateUsers()
{
	chatUserListModel->clear();
	chatUserListModel->addUsers(roomBuffer);
}

QString WidgetChatRoom::wrapWithColor(QString message, QString wrapColor)
{
	message.prepend("<font color=\"" + wrapColor + "\">");
	message.append("</font>");

	return message;
}

void WidgetChatRoom::updateUserMode(QString hostMask,QString mode,QString name)
{
	Irc::Util util;

	ui->textBrowser->append(wrapWithColor(util.messageToHtml(
		 tr("%1 sets mode %2 on %3.").arg(util.nickFromTarget(hostMask)).arg(mode).arg(name),
		 qApp->palette().foreground().color().name(), true, true, true), QColor("purple").name()));
	chatUserListModel->updateUserMode(hostMask, mode, name);
}

void WidgetChatRoom::nickChanged(QString oldNick, QString newNick)
{

	Irc::Util util;

	ui->textBrowser->append(wrapWithColor(util.messageToHtml(
		 tr("%1 is now known as %2.").arg(util.nickFromTarget(oldNick)).arg(newNick),
		 qApp->palette().foreground().color().name(), true, true, true), QColor("purple").name()));
	chatUserListModel->changeNick(util.nickFromTarget(oldNick), newNick);
}
