//
// widgetsearchresults.cpp
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

#include "widgetsearchresults.h"
#include "ui_widgetsearchresults.h"
#include "dialogfiltersearch.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

#include "NetworkCore/query.h"

WidgetSearchResults::WidgetSearchResults(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSearchResults)
{
	ui->setupUi(this);
	labelFilter = new QLabel();
	labelFilter->setText("Filter: ");
	lineEditFilter = new QLineEdit();
	lineEditFilter->setMaximumWidth(150);
	ui->toolBarFilter->insertWidget(ui->actionFilterMore, labelFilter);
	ui->toolBarFilter->insertWidget(ui->actionFilterMore, lineEditFilter);
	restoreState(quazaaSettings.WinMain.SearchToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	WidgetSearchTemplate *tabNewSearch = new WidgetSearchTemplate();
	ui->tabWidgetSearch->addTab(tabNewSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(0);
	connect(tabNewSearch, SIGNAL(statsUpdated(WidgetSearchTemplate*)), this, SLOT(onStatsUpdated(WidgetSearchTemplate*)));
	ui->splitterSearchDetails->restoreState(quazaaSettings.WinMain.SearchDetailsSplitter);
}

WidgetSearchResults::~WidgetSearchResults()
{
	delete ui;
}

void WidgetSearchResults::changeEvent(QEvent *e)
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

void WidgetSearchResults::skinChangeEvent()
{
	ui->frameSearches->setStyleSheet(skinSettings.tabSearches);
	ui->toolBarSearchResults->setStyleSheet(skinSettings.toolbars);
	ui->toolBarFilter->setStyleSheet(skinSettings.toolbars);
}

void WidgetSearchResults::saveWidget()
{
	quazaaSettings.WinMain.SearchToolbar = saveState();
	quazaaSettings.WinMain.SearchDetailsSplitter = ui->splitterSearchDetails->saveState();
}


void WidgetSearchResults::startSearch(QString searchString)
{
	if (searchString != "")
	{
		WidgetSearchTemplate* pWg = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
		if( pWg )
		{
			CQuery* pQuery = new CQuery();
			pQuery->SetDescriptiveName(searchString);
			pWg->StartSearch(pQuery);
			ui->tabWidgetSearch->setTabText(ui->tabWidgetSearch->currentIndex(), searchString);
		}
	}
}

void WidgetSearchResults::startNewSearch(QString *searchString)
{
	if (searchString != QString(""))
	{
		addSearchTab();
		WidgetSearchTemplate* pWg = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
		if( pWg )
		{
			CQuery* pQuery = new CQuery();
			pQuery->SetDescriptiveName(QString(*searchString));
			pWg->StartSearch(pQuery);
			ui->tabWidgetSearch->setTabText(ui->tabWidgetSearch->currentIndex(), QString(*searchString));
		}
	}
}

void WidgetSearchResults::addSearchTab()
{
	WidgetSearchTemplate *tabNewSearch = new WidgetSearchTemplate();
	ui->tabWidgetSearch->addTab(tabNewSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(ui->tabWidgetSearch->count());
	ui->tabWidgetSearch->setTabsClosable(true);
	connect(tabNewSearch, SIGNAL(statsUpdated(WidgetSearchTemplate*)), this, SLOT(onStatsUpdated(WidgetSearchTemplate*)));
}

void WidgetSearchResults::on_tabWidgetSearch_tabCloseRequested(int index)
{
	WidgetSearchTemplate* pWidget = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->widget(index));
	ui->tabWidgetSearch->removeTab(index);
		delete pWidget;
	if (ui->tabWidgetSearch->count() == 1)
	{
		ui->tabWidgetSearch->setTabsClosable(false);
	}
}

void WidgetSearchResults::stopSearch()
{
	WidgetSearchTemplate* pWg = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	if( pWg )
	{
		if ( pWg->searchState == SearchState::Searching )
			pWg->PauseSearch();
	}
}

bool WidgetSearchResults::clearSearch()
{
	WidgetSearchTemplate* pWg = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	if( pWg )
	{
		qDebug() << "Clear search captured in WidgetSearchResults.";

		pWg->StopSearch();
		pWg->ClearSearch();
		ui->tabWidgetSearch->setTabText(ui->tabWidgetSearch->currentIndex(), tr("Search"));
		return true;
	}
	return false;
}

void WidgetSearchResults::on_actionFilterMore_triggered()
{
	QSkinDialog *dlgSkinFilterSearch = new QSkinDialog(false, true, false, false, this);
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch;

	dlgSkinFilterSearch->addChildWidget(dlgFilterSearch);

	connect(dlgFilterSearch, SIGNAL(closed()), dlgSkinFilterSearch, SLOT(close()));
	dlgSkinFilterSearch->show();
}

void WidgetSearchResults::on_splitterSearchDetails_customContextMenuRequested(QPoint pos)
{
	if (ui->splitterSearchDetails->handle(1)->underMouse())
	{
		if (ui->splitterSearchDetails->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.SearchResultsSplitterRestoreTop = ui->splitterSearchDetails->sizes()[0];
			quazaaSettings.WinMain.SearchResultsSplitterRestoreBottom = ui->splitterSearchDetails->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterSearchDetails->sizes()[0] + ui->splitterSearchDetails->sizes()[1]);
			newSizes.append(0);
			ui->splitterSearchDetails->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.SearchResultsSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.SearchResultsSplitterRestoreBottom);
			ui->splitterSearchDetails->setSizes(sizesList);
		}
	}
}

void WidgetSearchResults::on_tabWidgetSearch_currentChanged(int index)
{
	WidgetSearchTemplate* pWg = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	emit searchTabChanged(pWg);
	emit statsUpdated( pWg );
}

void WidgetSearchResults::onStatsUpdated( WidgetSearchTemplate *searchWidget )
{
	ui->tabWidgetSearch->setTabText(ui->tabWidgetSearch->indexOf(searchWidget), (QString("%1 [%2,%3]").arg(searchWidget->sSearchString).arg(searchWidget->nFiles).arg(searchWidget->nHits)));
	if ( ( searchWidget = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget()) ) )
	{
		emit statsUpdated( searchWidget );
	}
}
