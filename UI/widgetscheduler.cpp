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
#include "QSkinDialog/qskinsettings.h"

WidgetScheduler::WidgetScheduler(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetScheduler)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.SchedulerToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetScheduler::~WidgetScheduler()
{
	delete ui;
}

void WidgetScheduler::changeEvent(QEvent *e)
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

void WidgetScheduler::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
	ui->treeWidgetSchedules->setStyleSheet(skinSettings.listViews);
}

void WidgetScheduler::saveWidget()
{
	quazaaSettings.WinMain.SchedulerToolbar = saveState();
}

void WidgetScheduler::on_actionAddScheduledTask_triggered()
{
	QSkinDialog *dlgSkinScheduler = new QSkinDialog(false, true, false, false, this);
	DialogScheduler *dlgScheduler = new DialogScheduler;

	dlgSkinScheduler->addChildWidget(dlgScheduler);

	connect(dlgScheduler, SIGNAL(closed()), dlgSkinScheduler, SLOT(close()));
	dlgSkinScheduler->show();
}

void WidgetScheduler::on_actionScheduleProperties_triggered()
{
	QSkinDialog *dlgSkinScheduler = new QSkinDialog(false, true, false, false, this);
	DialogScheduler *dlgScheduler = new DialogScheduler;

	dlgSkinScheduler->addChildWidget(dlgScheduler);

	connect(dlgScheduler, SIGNAL(closed()), dlgSkinScheduler, SLOT(close()));
	dlgSkinScheduler->show();
}
