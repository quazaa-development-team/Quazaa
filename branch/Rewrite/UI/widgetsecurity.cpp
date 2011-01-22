//
// widgetsecurity.cpp
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

#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"

#include "quazaasettings.h"
 

WidgetSecurity::WidgetSecurity(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetSecurity)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.SecurityToolbars);
}

WidgetSecurity::~WidgetSecurity()
{
	delete ui;
}

void WidgetSecurity::changeEvent(QEvent* e)
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

void WidgetSecurity::saveWidget()
{
	quazaaSettings.WinMain.SecurityToolbars = saveState();
}

void WidgetSecurity::on_actionSecurityAddRule_triggered()
{
	DialogAddRule* dlgAddRule = new DialogAddRule;
	dlgAddRule->show();
}

void WidgetSecurity::on_actionSubscribeSecurityList_triggered()
{
	DialogSecuritySubscriptions* dlgSecuritySubscriptions = new DialogSecuritySubscriptions;
	dlgSecuritySubscriptions->show();
}
