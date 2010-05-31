#include "widgetlibrary.h"
#include "ui_widgetlibrary.h"
#include "dialogeditshares.h"

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

void WidgetLibrary::on_toolButtonLibraryEditShares_clicked()
{
	QSkinDialog *dlgSkinEditShares = new QSkinDialog(false, true, false, this);
	DialogEditShares *dlgEditShares = new DialogEditShares;

	dlgSkinEditShares->addChildWidget(dlgEditShares);

	connect(dlgEditShares, SIGNAL(closed()), dlgSkinEditShares, SLOT(close()));
	dlgSkinEditShares->show();
}

void WidgetLibrary::saveWidget()
{
	quazaaSettings.WinMain.LibraryNavigatorTab = ui->tabWidgetLibraryNavigator->currentIndex();
	quazaaSettings.WinMain.LibrarySplitter = ui->splitterLibrary->saveState();
	panelLibraryView->saveWidget();
}

void WidgetLibrary::on_splitterLibrary_customContextMenuRequested(QPoint pos)
{
	if (ui->splitterLibrary->handle(1)->underMouse())
	{
		if (ui->splitterLibrary->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.LibrarySplitterRestoreLeft = ui->splitterLibrary->sizes()[0];
			quazaaSettings.WinMain.LibrarySplitterRestoreRight = ui->splitterLibrary->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterLibrary->sizes()[0] + ui->splitterLibrary->sizes()[1]);
			ui->splitterLibrary->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.LibrarySplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.LibrarySplitterRestoreRight);
			ui->splitterLibrary->setSizes(sizesList);
		}
	}
}
