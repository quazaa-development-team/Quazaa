/*
** widgetchatmiddle.cpp
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
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

#include "widgetchat.h"
#include "ui_widgetchat.h"
#include "widgetchatmiddle.h"
#include "ui_widgetchatmiddle.h"
#include "dialogsettings.h"
#include "dialogprofile.h"
#include "systemlog.h"
#include "chatconverter.h"

#include "quazaasettings.h"
#include "ircutil.h"
#include "ircsession.h"
 

WidgetChatMiddle::WidgetChatMiddle(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatMiddle)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.ChatToolbars);

	widgetChatInput = new WidgetChatInput(this, true);
	ui->horizontalLayoutTextInput->addWidget(widgetChatInput);
}

WidgetChatMiddle::~WidgetChatMiddle()
{
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
	quazaaSettings.saveSettings();
	quazaaSettings.WinMain.ChatToolbars = saveState();
}

void WidgetChatMiddle::on_actionChatSettings_triggered()
{
	DialogSettings* dlgSettings = new DialogSettings(this, SettingsPage::Chat);
	dlgSettings->show();
}

void WidgetChatMiddle::on_actionEditMyProfile_triggered()
{
	DialogProfile* dlgProfile = new DialogProfile(this);
	dlgProfile->show();
}
