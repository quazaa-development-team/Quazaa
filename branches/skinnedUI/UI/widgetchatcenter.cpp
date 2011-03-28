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
#include "widgetchatcenter.h"
#include "ui_widgetchatcenter.h"
#include "dialogsettings.h"
#include "dialogprofile.h"

#include "quazaasettings.h"
#include "ircbuffer.h"
#include "ircutil.h"
#include "ircsession.h"
#include "quazaairc.h"
#include "QSkinDialog/qskinsettings.h"

WidgetChatCenter::WidgetChatCenter(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatCenter)
{
	ui->setupUi(this);
	quazaaIrc = new QuazaaIRC();
	if(quazaaSettings.Chat.ConnectOnStartup)
	{
		quazaaIrc->startIrc(quazaaSettings.Chat.IrcUseSSL, quazaaSettings.Profile.IrcNickname, quazaaSettings.Profile.IrcUserName, quazaaSettings.Chat.IrcServerName, quazaaSettings.Chat.IrcServerPort);
		ui->actionConnect->setEnabled(false);
		ui->actionDisconnect->setEnabled(true);
		qDebug() << "Trying to connect to IRC";
	}
	else
	{
		ui->actionConnect->setEnabled(true);
		ui->actionDisconnect->setEnabled(false);
	}
	restoreState(quazaaSettings.WinMain.ChatToolbars);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	connect(quazaaIrc, SIGNAL(appendMessage(Irc::Buffer*, QString, QString, QuazaaIRC::Event)), this, SLOT(appendMessage(Irc::Buffer*, QString, QString, QuazaaIRC::Event)));
	connect(quazaaIrc, SIGNAL(channelNames(QStringList)), this, SLOT(channelNames(QStringList)));
	connect(quazaaIrc, SIGNAL(bufferAdded(QString)), this, SLOT(addBuffer(QString)));
	connect(quazaaIrc, SIGNAL(setPrefixes(QString, QString)), this, SLOT(setPrefixes(QString, QString)));
	//connect(quazaaIrc, SIGNAL(joined(QString)), this, SLOT(joined(QString)));

	WidgetChatTab* wct = new WidgetChatTab(quazaaIrc, this);
	ui->tabWidget->addTab(wct, "Status");
	wct->setName("*status"); // * is not allowed by RFC (IIRC)
	skinChangeEvent();
}

WidgetChatCenter::~WidgetChatCenter()
{
	if(ui->actionDisconnect->isEnabled())
	{
		quazaaIrc->stopIrc();
	}
	delete ui;
}

void WidgetChatCenter::changeEvent(QEvent* e)
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

void WidgetChatCenter::skinChangeEvent()
{
	ui->toolBarChatControls->setStyleSheet(skinSettings.chatToolbar);
}

void WidgetChatCenter::saveWidget()
{
	quazaaSettings.WinMain.ChatToolbars = saveState();
}

void WidgetChatCenter::on_actionConnect_triggered()
{
	quazaaIrc->startIrc(quazaaSettings.Chat.IrcUseSSL, quazaaSettings.Profile.IrcNickname, quazaaSettings.Profile.IrcUserName, quazaaSettings.Chat.IrcServerName, quazaaSettings.Chat.IrcServerPort);
	ui->actionConnect->setEnabled(false);
	ui->actionDisconnect->setEnabled(true);
	qDebug() << "Trying to connect to IRC";
}

void WidgetChatCenter::on_actionChatSettings_triggered()
{
	QSkinDialog* dlgSkinSettings = new QSkinDialog(true, true, false, false, this);
	DialogSettings* dlgSettings = new DialogSettings(this, SettingsPage::Chat);

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSkinSettings->show();
}

void WidgetChatCenter::on_actionDisconnect_triggered()
{
	quazaaIrc->stopIrc();
	ui->actionConnect->setEnabled(true);
	ui->actionDisconnect->setEnabled(false);
	qDebug() << "Trying to disconnect from IRC";
}

void WidgetChatCenter::appendMessage(Irc::Buffer* buffer, QString sender, QString message, QuazaaIRC::Event event)
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

	qDebug() << "Got a message from buffer " + (buffer->receiver()) + " | sender = " + sender + "| event = " + evendt;
	QString receiver = buffer->receiver();

	switch(event)
	{
		case QuazaaIRC::Message:
			tabByName(receiver)->append("<" + Irc::Util::nickFromTarget(sender) + "> " + message);
			break;
		case QuazaaIRC::Notice:
			currentTab()->append("<html><font color=blue>" + Irc::Util::nickFromTarget(sender) + ": " + message + "</font></html>");
			break;
		case QuazaaIRC::Action:
			tabByName(receiver)->append("<html><font color=purple>* " + Irc::Util::nickFromTarget(sender) + " " + message + "</font></html>");
			break;
		case QuazaaIRC::Status:
			//WidgetChatTab *ctab  = qobject_cast<WidgetChatTab*>(ui->tabWidget->widget(0));
			//qDebug() << "STATUSMESSAGE : "+buffer->receiver() + "|"+sender+"|"+message;
			//tab->append(message);
			tabByName("*status")->append(message);
			break;
		default:
			qDebug() << "This should not happen!";
			break;
	}
}

WidgetChatTab* WidgetChatCenter::tabByName(QString name)
{
	WidgetChatTab* tab;
	QList<WidgetChatTab*> allTabs = ui->tabWidget->findChildren<WidgetChatTab*>();
	for(int i = 0; i < allTabs.size(); ++i)
	{
		if(allTabs.at(i)->name == name)
		{
			tab = allTabs.at(i);
			return tab;
		}
	}
	qDebug() << "CREATING A NEW TAB :: " + name;
	// if the tab doesn't exist, create it
	tab = new WidgetChatTab(quazaaIrc);
	tab->setName(name);
	ui->tabWidget->addTab(tab, Irc::Util::nickFromTarget(name));
	return tab;
}

void WidgetChatCenter::channelNames(QStringList names)
{
	WidgetChatTab* tab	= tabByName(names.at(2));
	QString namestr		= names.at(3);
	QStringList list	= namestr.split(" ");

	list.sort();
	QMultiMap<int, int> map;

	for(int i = 0; i < list.size(); ++i)
	{
		QString user = list.at(i);
		int idx = prefixChars.indexOf(user.at(0));
		if(idx < 0)
		{
			idx = prefixChars.length();    // normal user
		}
		map.insert(idx, i);
	}

	QStringList sorted;
	QList<int> values = map.values();
	for(int i = 0; i < values.size(); ++i)
	{
		sorted.append(list.at(values.at(i)));
	}

	tab->channelNames(sorted);
}

WidgetChatTab* WidgetChatCenter::currentTab()
{
	qDebug() << "getting WidgetChatCenter::currentTab()";
	return qobject_cast<WidgetChatTab*>(ui->tabWidget->currentWidget());
}

void WidgetChatCenter::setPrefixes(QString modes, QString mprefs)
{
	// overkill ?
	prefixModes = modes;
	prefixChars = mprefs;
}

void WidgetChatCenter::addBuffer(QString name)
{
	if(name.at(0) == '#')
	{
		// tab will be auto-created
		ui->tabWidget->setCurrentWidget(tabByName(name));
	}
}

void WidgetChatCenter::on_tabWidget_currentChanged(QWidget*)
{
	qDebug() << "Emitting channel changed.";
	emit channelChanged(currentTab());
}

void WidgetChatCenter::on_actionEditMyProfile_triggered()
{
	QSkinDialog* dlgSkinProfile = new QSkinDialog(true, true, false, false, this);
	DialogProfile* dlgProfile = new DialogProfile;

	dlgSkinProfile->addChildWidget(dlgProfile);

	connect(dlgProfile, SIGNAL(closed()), dlgSkinProfile, SLOT(close()));
	dlgSkinProfile->show();
}
