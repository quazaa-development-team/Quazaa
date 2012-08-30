//
// widgetgraph.cpp
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

#include "widgetgraph.h"
#include "ui_widgetgraph.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetGraph::WidgetGraph(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetGraph)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.GraphToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterGraph->restoreState(quazaaSettings.WinMain.GraphSplitter);
}

WidgetGraph::~WidgetGraph()
{
	delete ui;
}

void WidgetGraph::changeEvent(QEvent* e)
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

void WidgetGraph::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->treeWidgetGraphSelector->setStyleSheet(skinSettings.listViews);
}

void WidgetGraph::saveWidget()
{
	quazaaSettings.WinMain.GraphToolbar = saveState();
	quazaaSettings.WinMain.GraphSplitter = ui->splitterGraph->saveState();
}

void WidgetGraph::on_splitterGraph_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterGraph->handle(1)->underMouse())
	{
		if(ui->splitterGraph->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.GraphSplitterRestoreLeft = ui->splitterGraph->sizes()[0];
			quazaaSettings.WinMain.GraphSplitterRestoreRight = ui->splitterGraph->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterGraph->sizes()[0] + ui->splitterGraph->sizes()[1]);
			ui->splitterGraph->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.GraphSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.GraphSplitterRestoreRight);
			ui->splitterGraph->setSizes(sizesList);
		}
	}
}
