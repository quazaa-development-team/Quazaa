//
// widgetscheduler.cpp
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

#include "widgetscheduler.h"
#include "ui_widgetscheduler.h"
#include "dialogscheduler.h"

#include "quazaasettings.h"
 

WidgetScheduler::WidgetScheduler(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetScheduler)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.SchedulerToolbar);
}

WidgetScheduler::~WidgetScheduler()
{
	delete ui;
}

void WidgetScheduler::changeEvent(QEvent* e)
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

void WidgetScheduler::saveWidget()
{
	quazaaSettings.WinMain.SchedulerToolbar = saveState();
}

void WidgetScheduler::on_actionAddScheduledTask_triggered()
{
	DialogScheduler* dlgScheduler = new DialogScheduler(this);
	dlgScheduler->show();
}

void WidgetScheduler::on_actionScheduleProperties_triggered()
{
	DialogScheduler* dlgScheduler = new DialogScheduler(this);
	dlgScheduler->show();
}
