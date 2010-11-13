//
// widgetdiscovery.cpp
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

#include "widgetdiscovery.h"
#include "ui_widgetdiscovery.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetDiscovery::WidgetDiscovery(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetDiscovery)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.DiscoveryToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetDiscovery::~WidgetDiscovery()
{
	delete ui;
}

void WidgetDiscovery::changeEvent(QEvent* e)
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

void WidgetDiscovery::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->toolBarProperties->setStyleSheet(skinSettings.toolbars);
	ui->toolBarServices->setStyleSheet(skinSettings.toolbars);
	ui->treeViewDiscovery->setStyleSheet(skinSettings.listViews);
}

void WidgetDiscovery::saveWidget()
{
	quazaaSettings.WinMain.DiscoveryToolbar = saveState();
}
