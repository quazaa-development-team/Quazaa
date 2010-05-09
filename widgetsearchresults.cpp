#include "widgetsearchresults.h"
#include "ui_widgetsearchresults.h"
#include "widgetsearchtemplate.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSearchResults::WidgetSearchResults(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSearchResults)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.SearchToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	WidgetSearchTemplate *tabNewSearch = new WidgetSearchTemplate();
	ui->tabWidgetSearch->addTab(tabNewSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(0);
	ui->tabWidgetSearch->setTabsClosable(true);
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
