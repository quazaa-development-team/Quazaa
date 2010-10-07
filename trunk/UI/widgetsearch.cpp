//
// widgetsearch.cpp
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

#include "widgetsearch.h"
#include "ui_widgetsearch.h"

#include "quazaasettings.h"
#include "systemlog.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSearch::WidgetSearch(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetSearch)
{
	ui->setupUi(this);
	ui->comboBoxSearchFileType->setView(new QListView());
	ui->comboBoxSearchSizeMax->setView(new QListView());
	ui->comboBoxSearchSizeMin->setView(new QListView());
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterSearch->restoreState(quazaaSettings.WinMain.SearchSplitter);
	ui->toolButtonSearchFiletypeTaskHeader->setChecked(quazaaSettings.WinMain.SearchFileTypeTaskVisible);
	ui->toolButtonSearchNetworksTaskHeader->setChecked(quazaaSettings.WinMain.SearchNetworksTaskVisible);
	ui->toolButtonSearchResultsTaskHeader->setChecked(quazaaSettings.WinMain.SearchResultsTaskVisible);
	ui->toolButtonSearchTaskHeader->setChecked(quazaaSettings.WinMain.SearchTaskVisible);
	panelSearchResults = new WidgetSearchResults();
	ui->verticalLayoutSearchResults->addWidget(panelSearchResults);
	connect(panelSearchResults, SIGNAL(searchTabChanged(WidgetSearchTemplate*)), this, SLOT(onSearchTabChanged(WidgetSearchTemplate*)));
	connect(panelSearchResults, SIGNAL(statsUpdated(WidgetSearchTemplate*)), this, SLOT(updateStats(WidgetSearchTemplate*)));
}

WidgetSearch::~WidgetSearch()
{
	delete ui;
}

void WidgetSearch::changeEvent(QEvent *e)
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

void WidgetSearch::skinChangeEvent()
{
	ui->frameSearchTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameSearchNetworksTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameSearchResultsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameSearchFiletypeTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->scrollAreaSearchSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonSearchFiletypeTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonSearchNetworksTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonSearchResultsTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonSearchTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonNewSearch->setStyleSheet(skinSettings.addSearchButton);
	ui->listViewSearchFiletype->setStyleSheet(skinSettings.listViews);
}

void WidgetSearch::saveWidget()
{
	quazaaSettings.WinMain.SearchSplitter = ui->splitterSearch->saveState();
	quazaaSettings.WinMain.SearchFileTypeTaskVisible = ui->toolButtonSearchFiletypeTaskHeader->isChecked();
	quazaaSettings.WinMain.SearchNetworksTaskVisible = ui->toolButtonSearchNetworksTaskHeader->isChecked();
	quazaaSettings.WinMain.SearchResultsTaskVisible = ui->toolButtonSearchResultsTaskHeader->isChecked();
	quazaaSettings.WinMain.SearchTaskVisible = ui->toolButtonSearchTaskHeader->isChecked();
	panelSearchResults->saveWidget();
}

void WidgetSearch::on_toolButtonSearch_clicked()
{
	panelSearchResults->startSearch(ui->lineEditSearch->text());
	ui->toolButtonSearch->setText(tr("Searching"));
	ui->toolButtonSearch->setEnabled(false);
	ui->toolButtonSearchClear->setText("Stop");
	ui->toolButtonSearchClear->setEnabled(true);
}

void WidgetSearch::on_toolButtonSearchClear_clicked()
{
	if (ui->toolButtonSearchClear->text() == "Stop")
	{
		ui->toolButtonSearch->setText(tr("More"));
		ui->toolButtonSearch->setEnabled(true);
		ui->toolButtonSearchClear->setText("Clear");
		panelSearchResults->stopSearch();
	} else
	if (ui->toolButtonSearchClear->text() == "Clear")
	{
		qDebug() << "Clear search triggered.";
		bool cleared = panelSearchResults->clearSearch();
		if (cleared)
		{
			ui->toolButtonSearch->setText(tr("Search"));
			ui->toolButtonSearch->setEnabled(true);
			ui->toolButtonSearchClear->setText("Stop");
			ui->toolButtonSearchClear->setEnabled(false);
			ui->labelSearchResultsSearching->setText(tr("Not Currently Searching"));
			ui->labelSearchResultsFound->setText(tr("No Files Found"));
		}
	}
}

void WidgetSearch::startNewSearch(QString *searchString)
{
	panelSearchResults->startNewSearch(searchString);
}

void WidgetSearch::on_toolButtonNewSearch_clicked()
{
	panelSearchResults->addSearchTab();
}

void WidgetSearch::on_splitterSearch_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if (ui->splitterSearch->handle(1)->underMouse())
	{
		if (ui->splitterSearch->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.SearchSplitterRestoreLeft = ui->splitterSearch->sizes()[0];
			quazaaSettings.WinMain.SearchSplitterRestoreRight = ui->splitterSearch->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterSearch->sizes()[0] + ui->splitterSearch->sizes()[1]);
			ui->splitterSearch->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.SearchSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.SearchSplitterRestoreRight);
			ui->splitterSearch->setSizes(sizesList);
		}
	}
}

void WidgetSearch::onSearchTabChanged(WidgetSearchTemplate *searchPage)
{
	ui->lineEditSearch->setText(searchPage->sSearchString);

	switch (searchPage->searchState)
	{
	case SearchState::Searching:
		ui->toolButtonSearch->setText(tr("Searching"));
		ui->toolButtonSearch->setEnabled(false);
		ui->toolButtonSearchClear->setText("Stop");
		ui->toolButtonSearchClear->setEnabled(true);
		break;
	case SearchState::Paused:
		ui->toolButtonSearch->setText(tr("More"));
		ui->toolButtonSearch->setEnabled(true);
		ui->toolButtonSearchClear->setText("Clear");
		panelSearchResults->stopSearch();
		break;
	case SearchState::Stopped:
		ui->toolButtonSearch->setText(tr("Search"));
		ui->toolButtonSearch->setEnabled(true);
		ui->toolButtonSearchClear->setText("Stop");
		ui->toolButtonSearchClear->setEnabled(false);
		ui->labelSearchResultsSearching->setText(tr("Not Currently Searching"));
		ui->labelSearchResultsFound->setText(tr("No Files Found"));
		break;
	default:
		break;
	}
}

void WidgetSearch::updateStats(WidgetSearchTemplate *searchWidget)
{
	ui->labelSearchResultsSearching->setText(tr("%1 hubs,%2 leaves.").arg(searchWidget->nHubs).arg(searchWidget->nLeaves));
	ui->labelSearchResultsFound->setText(tr("%1 files in %2 hits.").arg(searchWidget->nFiles).arg(searchWidget->nHits));
	if (searchWidget->nHubs == 0 && searchWidget->nLeaves == 0 && searchWidget->searchState != SearchState::Searching)
	{
		ui->labelSearchResultsSearching->setText(tr("Not Currently Searching"));
	}
	if (searchWidget->nFiles == 0 && searchWidget->nHits == 0)
	{
		ui->labelSearchResultsFound->setText(tr("No Files Found"));
	}
}
