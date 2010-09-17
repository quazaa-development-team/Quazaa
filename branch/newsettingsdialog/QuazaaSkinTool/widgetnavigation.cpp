#include "widgetnavigation.h"
#include "ui_widgetnavigation.h"

#include "../QSkinDialog/qskinsettings.h"

WidgetNavigation::WidgetNavigation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetNavigation)
{
    ui->setupUi(this);
	actionGroupMainNavigation = new QActionGroup(this);
	actionGroupMainNavigation->addAction(ui->actionHome);
	actionGroupMainNavigation->addAction(ui->actionLibrary);
	actionGroupMainNavigation->addAction(ui->actionMedia);
	actionGroupMainNavigation->addAction(ui->actionSearch);
	actionGroupMainNavigation->addAction(ui->actionTransfers);
	actionGroupMainNavigation->addAction(ui->actionSecurity);
	actionGroupMainNavigation->addAction(ui->actionActivity);
	actionGroupMainNavigation->addAction(ui->actionChat);
	actionGroupMainNavigation->addAction(ui->actionGeneric);
}

WidgetNavigation::~WidgetNavigation()
{
    delete ui;
}

void WidgetNavigation::changeEvent(QEvent *e)
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

void WidgetNavigation::skinChangeEvent()
{
	ui->toolBarNavigation->setStyleSheet(skinSettings.navigationToolbar);
	ui->frameHomeHeader->setStyleSheet(skinSettings.homeHeader);
	ui->frameLibraryHeader->setStyleSheet(skinSettings.libraryHeader);
	ui->frameMediaHeader->setStyleSheet(skinSettings.mediaHeader);
	ui->frameSearchHeader->setStyleSheet(skinSettings.searchHeader);
	ui->frameTransfersHeader->setStyleSheet(skinSettings.transfersHeader);
	ui->frameSecurityHeader->setStyleSheet(skinSettings.securityHeader);
	ui->frameActivityHeader->setStyleSheet(skinSettings.activityHeader);
	ui->frameChatHeader->setStyleSheet(skinSettings.chatHeader);
	ui->frameGenericHeader->setStyleSheet(skinSettings.genericHeader);
	ui->frameDialogHeader->setStyleSheet(skinSettings.dialogHeader);
}
