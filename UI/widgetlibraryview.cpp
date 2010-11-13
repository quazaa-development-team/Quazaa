//
// widgetlibraryview.cpp
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

#include "widgetlibraryview.h"
#include "ui_widgetlibraryview.h"
#include "dialoglibrarysearch.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetLibraryView::WidgetLibraryView(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetLibraryView)
{
	ui->setupUi(this);
	lineEditFind = new QLineEdit();
	lineEditFind->setMaximumWidth(150);
	ui->toolBarSearch->insertWidget(ui->actionFind, lineEditFind);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	restoreState(quazaaSettings.WinMain.LibraryToolbar);
	ui->splitterLibraryView->restoreState(quazaaSettings.WinMain.LibraryDetailsSplitter);
}

WidgetLibraryView::~WidgetLibraryView()
{
	delete ui;
}

void WidgetLibraryView::changeEvent(QEvent* e)
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

void WidgetLibraryView::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
	ui->toolBarSearch->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameLibraryStatus->setStyleSheet(skinSettings.libraryViewHeader);
	ui->treeViewLibraryFiles->setStyleSheet(skinSettings.listViews);
}

void WidgetLibraryView::saveWidget()
{
	quazaaSettings.WinMain.LibraryToolbar = saveState();
	quazaaSettings.WinMain.LibraryDetailsSplitter = ui->splitterLibraryView->saveState();
}

void WidgetLibraryView::on_actionFind_triggered()
{
	QSkinDialog* dlgSkinLibrarySearch = new QSkinDialog(false, true, false, false, this);
	DialogLibrarySearch* dlgLibrarySearch = new DialogLibrarySearch;

	dlgSkinLibrarySearch->addChildWidget(dlgLibrarySearch);

	connect(dlgLibrarySearch, SIGNAL(closed()), dlgSkinLibrarySearch, SLOT(close()));
	dlgSkinLibrarySearch->show();
}

void WidgetLibraryView::on_splitterLibraryView_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterLibraryView->handle(1)->underMouse())
	{
		if(ui->splitterLibraryView->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.LibraryDetailsSplitterRestoreTop = ui->splitterLibraryView->sizes()[0];
			quazaaSettings.WinMain.LibraryDetailsSplitterRestoreBottom = ui->splitterLibraryView->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterLibraryView->sizes()[0] + ui->splitterLibraryView->sizes()[1]);
			newSizes.append(0);
			ui->splitterLibraryView->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.LibraryDetailsSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.LibraryDetailsSplitterRestoreBottom);
			ui->splitterLibraryView->setSizes(sizesList);
		}
	}
}
