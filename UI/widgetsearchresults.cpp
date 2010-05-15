#include "widgetsearchresults.h"
#include "ui_widgetsearchresults.h"
#include "widgetsearchtemplate.h"
#include "dialogfiltersearch.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

#include "NetworkCore/Query.h"

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
	ui->splitterSearchDetails->restoreState(quazaaSettings.WinMain.SearchDetailsSplitter);
	ui->actionSeachDetailsToggle->setChecked(quazaaSettings.WinMain.SearchDetailsVisible);
	ui->actionSearchToggle->setChecked(quazaaSettings.WinMain.SearchSidebarVisible);
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
}

void WidgetSearchResults::saveWidget()
{
	quazaaSettings.WinMain.SearchToolbar = saveState();
	quazaaSettings.WinMain.SearchDetailsSplitter = ui->splitterSearchDetails->saveState();
	quazaaSettings.WinMain.SearchDetailsVisible = ui->actionSeachDetailsToggle->isChecked();
	quazaaSettings.WinMain.SearchSidebarVisible = ui->actionSearchToggle->isChecked();
}


void WidgetSearchResults::startSearch(QString searchString)
{
	WidgetSearchTemplate* pWg = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	if( pWg )
	{
		CQuery* pQuery = new CQuery();
		pQuery->SetDescriptiveName(searchString);
		pWg->StartSearch(pQuery);
	}
}

void WidgetSearchResults::startNewSearch(QString searchString)
{
	WidgetSearchTemplate *tabNewSearch = new WidgetSearchTemplate();
	ui->tabWidgetSearch->addTab(tabNewSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(ui->tabWidgetSearch->count());
	ui->tabWidgetSearch->setTabsClosable(true);
	WidgetSearchTemplate* pWg = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	if( pWg )
	{
		CQuery* pQuery = new CQuery();
		pQuery->SetDescriptiveName(searchString);
		pWg->StartSearch(pQuery);
	}
}

void WidgetSearchResults::addSearchTab()
{
	WidgetSearchTemplate *tabNewSearch = new WidgetSearchTemplate();
	ui->tabWidgetSearch->addTab(tabNewSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(ui->tabWidgetSearch->count());
	ui->tabWidgetSearch->setTabsClosable(true);
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

void WidgetSearchResults::on_actionFilterMore_triggered()
{
	QSkinDialog *dlgSkinFilterSearch = new QSkinDialog(false, true, false, this);
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch;

	dlgSkinFilterSearch->addChildWidget(dlgFilterSearch);

	connect(dlgFilterSearch, SIGNAL(closed()), dlgSkinFilterSearch, SLOT(close()));
	dlgSkinFilterSearch->show();
}
