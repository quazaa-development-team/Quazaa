#include "widgetsearch.h"
#include "ui_widgetsearch.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSearch::WidgetSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSearch)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterSearch->restoreState(quazaaSettings.WinMain.SearchSplitter);
	ui->toolButtonSearchFiletypeTaskHeader->setChecked(quazaaSettings.WinMain.SearchFileTypeTaskVisible);
	ui->toolButtonSearchNetworksTaskHeader->setChecked(quazaaSettings.WinMain.SearchNetworksTaskVisible);
	ui->toolButtonSearchResultsTaskHeader->setChecked(quazaaSettings.WinMain.SearchResultsTaskVisible);
	ui->toolButtonSearchTaskHeader->setChecked(quazaaSettings.WinMain.SearchTaskVisible);
	panelSearchResults = new WidgetSearchResults();
	ui->verticalLayoutSearchResults->addWidget(panelSearchResults);
	connect(panelSearchResults, SIGNAL(searchSidebarToggled(bool)), ui->frameSearchSidebar, SLOT(setVisible(bool)));
	ui->frameSearchSidebar->setVisible(quazaaSettings.WinMain.SearchSidebarVisible);
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
}

void WidgetSearch::on_toolButtonSearchClear_clicked()
{

}

void WidgetSearch::startNewSearch(QString *searchString)
{
	panelSearchResults->startNewSearch(searchString);
}

void WidgetSearch::on_toolButtonNewSearch_clicked()
{
	panelSearchResults->addSearchTab();
}
