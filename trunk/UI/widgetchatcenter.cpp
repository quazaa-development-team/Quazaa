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

#include "widgetchatcenter.h"
#include "ui_widgetchatcenter.h"
#include "dialogsettings.h"

#include "quazaasettings.h"
#include "quazaairc.h"
#include "QSkinDialog/qskinsettings.h"

WidgetChatCenter::WidgetChatCenter(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatCenter)
{
	ui->setupUi(this);
	if (quazaaSettings.Chat.ConnectOnStartup)
	{
		QuazaaIRC *quazaaIrc = new QuazaaIRC();
		quazaaIrc->startIrc(false, quazaaSettings.Profile.IrcNickname, quazaaSettings.Profile.IrcUserName, quazaaSettings.Chat.IrcServerName, quazaaSettings.Chat.IrcServerPort);
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
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetChatCenter::~WidgetChatCenter()
{
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
	m_oQuazaaIrc = new QuazaaIRC();
	ui->actionConnect->setEnabled(false);
	qDebug() << "Trying to connect to IRC";
}

void WidgetChatCenter::on_actionChatSettings_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSettings->switchSettingsPage(7);
	dlgSkinSettings->show();
}
