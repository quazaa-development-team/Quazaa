//
// widgetlibrary.cpp
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

#include "widgetlibrary.h"
#include "ui_widgetlibrary.h"
#include "dialogeditshares.h"
#include <QtGui>

#include "quazaasettings.h"
 

WidgetLibrary::WidgetLibrary(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::WidgetLibrary)
{
	ui->setupUi(this);
	ui->comboBoxLibraryNavigatorSharesFilter->setView(new QListView());
	ui->tabWidgetLibraryNavigator->setCurrentIndex(quazaaSettings.WinMain.LibraryNavigatorTab);
	ui->splitterLibrary->restoreState(quazaaSettings.WinMain.LibrarySplitter);
	panelLibraryView = new WidgetLibraryView();
	ui->verticalLayoutLibraryView->addWidget(panelLibraryView);
}

WidgetLibrary::~WidgetLibrary()
{
	delete ui;
}

void WidgetLibrary::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WidgetLibrary::on_toolButtonLibraryEditShares_clicked()
{
	DialogEditShares* dlgEditShares = new DialogEditShares(this);
	dlgEditShares->show();
}

void WidgetLibrary::saveWidget()
{
	quazaaSettings.WinMain.LibraryNavigatorTab = ui->tabWidgetLibraryNavigator->currentIndex();
	quazaaSettings.WinMain.LibrarySplitter = ui->splitterLibrary->saveState();
	panelLibraryView->saveWidget();
}

void WidgetLibrary::on_splitterLibrary_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterLibrary->handle(1)->underMouse())
	{
		if(ui->splitterLibrary->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.LibrarySplitterRestoreLeft = ui->splitterLibrary->sizes()[0];
			quazaaSettings.WinMain.LibrarySplitterRestoreRight = ui->splitterLibrary->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterLibrary->sizes()[0] + ui->splitterLibrary->sizes()[1]);
			ui->splitterLibrary->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.LibrarySplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.LibrarySplitterRestoreRight);
			ui->splitterLibrary->setSizes(sizesList);
		}
	}
}

void WidgetLibrary::initializeLibrary()
{

}
