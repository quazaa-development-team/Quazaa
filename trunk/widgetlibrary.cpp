#include "widgetlibrary.h"
#include "ui_widgetlibrary.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetLibrary::WidgetLibrary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetLibrary)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->tabWidgetLibraryNavigator->setCurrentIndex(quazaaSettings.WinMain.LibraryNavigatorTab);
	ui->splitterLibrary->restoreState(quazaaSettings.WinMain.LibrarySplitter);
    panelLibraryView = new WidgetLibraryView();
    ui->verticalLayoutLibraryView->addWidget(panelLibraryView);
}

WidgetLibrary::~WidgetLibrary()
{
	quazaaSettings.WinMain.LibraryNavigatorTab = ui->tabWidgetLibraryNavigator->currentIndex();
	quazaaSettings.WinMain.LibrarySplitter = ui->splitterLibrary->saveState();
	delete ui;
}

void WidgetLibrary::changeEvent(QEvent *e)
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

void WidgetLibrary::skinChangeEvent()
{
	ui->frameLibraryNavigator->setStyleSheet(skinSettings.sidebarBackground);
	ui->tabWidgetLibraryNavigator->setStyleSheet(skinSettings.libraryNavigator);
}
