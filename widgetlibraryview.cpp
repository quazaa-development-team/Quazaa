#include "widgetlibraryview.h"
#include "ui_widgetlibraryview.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetLibraryView::WidgetLibraryView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetLibraryView)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterLibraryView->restoreState(quazaaSettings.WinMain.LibraryDetailsSplitter);
	ui->toolButtonLibraryDetailsToggle->setChecked(quazaaSettings.WinMain.LibraryDetailsVisible);
}

WidgetLibraryView::~WidgetLibraryView()
{
    delete ui;
	quazaaSettings.WinMain.LibraryDetailsSplitter = ui->splitterLibraryView->saveState();
	quazaaSettings.WinMain.LibraryDetailsVisible = ui->toolButtonLibraryDetailsToggle->isChecked();
}

void WidgetLibraryView::changeEvent(QEvent *e)
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

void WidgetLibraryView::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameLibraryStatus->setStyleSheet(skinSettings.toolbars);
}
