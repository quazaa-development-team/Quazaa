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

#include "quazaasettings.h"
#include "ircbuffer.h"
#include "ircutil.h"
#include "quazaairc.h"
#include "QSkinDialog/qskinsettings.h"

WidgetChatCenter::WidgetChatCenter(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatCenter)
{
	ui->setupUi(this);
	quazaaIrc = new QuazaaIRC();
	if (quazaaSettings.Chat.ConnectOnStartup)
	{
		quazaaIrc->startIrc(quazaaSettings.Chat.IrcUseSSL, quazaaSettings.Profile.IrcNickname, quazaaSettings.Profile.IrcUserName, quazaaSettings.Chat.IrcServerName, quazaaSettings.Chat.IrcServerPort);
		ui->actionConnect->setEnabled(false);
		ui->actionDisconnect->setEnabled(true);
		qDebug() << "Trying to connect to IRC";
	} else {
		ui->actionConnect->setEnabled(true);
		ui->actionDisconnect->setEnabled(false);
	}
	lineEditTextInput = new QLineEdit();
	lineEditTextInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	toolButtonSmilies = new QToolButton();
	toolButtonSmilies->setToolTip("Smilies");
	toolButtonSmilies->setPopupMode(QToolButton::MenuButtonPopup);
	toolButtonSmilies->setIcon(QIcon(":/Resource/Smileys/Happy.png"));
	toolButtonOp = new QToolButton();
	toolButtonOp->setToolTip("Operator Commands");
	toolButtonOp->setPopupMode(QToolButton::MenuButtonPopup);
	toolButtonOp->setIcon(QIcon(":/Resource/Generic/QuazaaForums.png"));
	ui->toolBarChatMessage->insertWidget(ui->actionSend, lineEditTextInput);
	ui->toolBarOperator->addWidget(toolButtonSmilies);
	ui->toolBarOperator->addWidget(toolButtonOp);
	restoreState(quazaaSettings.WinMain.ChatToolbars);
	connect(lineEditTextInput, SIGNAL(returnPressed()), this, SLOT(on_actionSend_triggered()));
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	connect(quazaaIrc, SIGNAL(appendMessage(Irc::Buffer*, QString, QString)), this, SLOT(appendMessage(Irc::Buffer*, QString, QString)));
	connect(quazaaIrc, SIGNAL(channelNames(QStringList)), this, SLOT(channelNames(QStringList)));
	connect(quazaaIrc, SIGNAL(bufferAdded(QString)), this, SLOT(addBuffer(QString)));

	ui->tabWidget->addTab(new WidgetChatTab(), "Status");
	skinChangeEvent();
}

WidgetChatCenter::~WidgetChatCenter()
{
	if (ui->actionDisconnect->isEnabled())
		quazaaIrc->stopIrc();
	delete ui;
}

void WidgetChatCenter::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
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
	ui->toolBarChatMessage->setStyleSheet(skinSettings.chatToolbar);
	ui->toolBarOperator->setStyleSheet(skinSettings.chatToolbar);
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
	QSkinDialog *dlgSkinSettings = new QSkinDialog(true, true, false, false, this);
	DialogSettings *dlgSettings = new DialogSettings(this, SettingsPage::Chat);

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

void WidgetChatCenter::on_actionSend_triggered()
{
	if (lineEditTextInput->text() != "")
	{
		quazaaIrc->sendIrcMessage(qobject_cast<WidgetChatTab *>(ui->tabWidget->currentWidget()), lineEditTextInput->text());
		lineEditTextInput->setText("");
	}
}

void WidgetChatCenter::appendMessage(Irc::Buffer* buffer, QString sender, QString message)
{
	WidgetChatTab *tab = tabByName(buffer->receiver());
	tab->append("<"+ Irc::Util::nickFromTarget(sender) + "> " + message);
}

WidgetChatTab* WidgetChatCenter::tabByName(QString name) {
	QList<WidgetChatTab *> allTabs = ui->tabWidget->findChildren<WidgetChatTab *>();
	for (int i = 0; i < allTabs.size(); ++i) {
		if (allTabs.at(i)->name == name) {
			return allTabs.at(i);
		}
	}
	// fail safe, remove later
	return new WidgetChatTab(this);
}

void WidgetChatCenter::addBuffer(QString name) {
	WidgetChatTab *tab = new WidgetChatTab();
	tab->setName(name);
	ui->tabWidget->addTab(tab, Irc::Util::nickFromTarget(name));
}

void WidgetChatCenter::channelNames(QStringList names)
{
	WidgetChatTab *tab = tabByName(names.at(2));
	QString namestr		= names.at(3);
	QStringList list	= namestr.split(" ");
	//qSort(list.begin(), list.end(), WidgetChatCenter::prefixSort);


	tab->channelNames(list);
}

bool WidgetChatCenter::prefixSort(const QString &s1, const QString &s2)
{
	/*if (int idx = prefixChars.indexOf(s1.at(0)) > -1) {
		if (int idx2 = prefixChars.indexOf(s2.at(0)) > -1) {
			if (idx > idx2) {
				return true;
			}
		}
	}*/
	return false;
}

void WidgetChatCenter::setPrefixes(QString modes, QString mprefs) {
	//prefixModes = modes;
	//prefixChars = mprefs;
}
