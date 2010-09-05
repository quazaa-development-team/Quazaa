//
// widgetactivity.cpp
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

#include "widgetactivity.h"
#include "ui_widgetactivity.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetActivity::WidgetActivity(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetActivity)
{
	ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterActivity->restoreState(quazaaSettings.WinMain.ActivitySplitter);
	panelNeighbors = new WidgetNeighbors();
	ui->verticalLayoutNeighbors->addWidget(panelNeighbors);
	panelSystemLog = new WidgetSystemLog();
	ui->verticalLayoutSystemLog->addWidget(panelSystemLog);
}

WidgetActivity::~WidgetActivity()
{
	delete ui;
}

void WidgetActivity::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void WidgetActivity::skinChangeEvent()
{
	ui->toolButtonNeighborsHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonSystemLogHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
}

void WidgetActivity::saveWidget()
{
	quazaaSettings.WinMain.ActivitySplitter = ui->splitterActivity->saveState();
	panelNeighbors->saveWidget();
	panelSystemLog->saveWidget();
}

void WidgetActivity::on_splitterActivity_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if (ui->splitterActivity->handle(1)->underMouse())
	{
		if (ui->splitterActivity->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.ActivitySplitterRestoreTop = ui->splitterActivity->sizes()[0];
			quazaaSettings.WinMain.ActivitySplitterRestoreBottom = ui->splitterActivity->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterActivity->sizes()[0] + ui->splitterActivity->sizes()[1]);
			newSizes.append(0);
			ui->splitterActivity->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreBottom);
			ui->splitterActivity->setSizes(sizesList);
		}
	}
}

void WidgetActivity::on_toolButtonSystemLogHeader_clicked()
{
	if (ui->splitterActivity->sizes()[0] > 0)
	{
		quazaaSettings.WinMain.ActivitySplitterRestoreTop = ui->splitterActivity->sizes()[0];
		quazaaSettings.WinMain.ActivitySplitterRestoreBottom = ui->splitterActivity->sizes()[1];
		QList<int> newSizes;
		newSizes.append(0);
		newSizes.append(ui->splitterActivity->sizes()[0] + ui->splitterActivity->sizes()[1]);
		ui->splitterActivity->setSizes(newSizes);
	} else {
		QList<int> sizesList;
		sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreTop);
		sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreBottom);
		ui->splitterActivity->setSizes(sizesList);
	}
}

void WidgetActivity::on_toolButtonNeighborsHeader_clicked()
{
	if (ui->splitterActivity->sizes()[1] > 0)
	{
		quazaaSettings.WinMain.ActivitySplitterRestoreTop = ui->splitterActivity->sizes()[0];
		quazaaSettings.WinMain.ActivitySplitterRestoreBottom = ui->splitterActivity->sizes()[1];
		QList<int> newSizes;
		newSizes.append(ui->splitterActivity->sizes()[0] + ui->splitterActivity->sizes()[1]);
		newSizes.append(0);
		ui->splitterActivity->setSizes(newSizes);
	} else {
		QList<int> sizesList;
		sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreTop);
		sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreBottom);
		ui->splitterActivity->setSizes(sizesList);
	}
}
