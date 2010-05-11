#include "widgetlibraryview.h"
#include "ui_widgetlibraryview.h"
#include "dialoglibrarysearch.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetLibraryView::WidgetLibraryView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetLibraryView)
{
    ui->setupUi(this);
	lineEditFind = new QLineEdit();
	lineEditFind->setMaximumWidth(150);
	ui->toolBarSearch->insertWidget(ui->actionFind, lineEditFind);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	restoreState(quazaaSettings.WinMain.LibraryToolbar);
	ui->splitterLibraryView->restoreState(quazaaSettings.WinMain.LibraryDetailsSplitter);
	ui->toolButtonLibraryDetailsToggle->setChecked(quazaaSettings.WinMain.LibraryDetailsVisible);
}

WidgetLibraryView::~WidgetLibraryView()
{
	delete ui;
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

void WidgetLibraryView::saveWidget()
{
	quazaaSettings.WinMain.LibraryToolbar = saveState();
	quazaaSettings.WinMain.LibraryDetailsSplitter = ui->splitterLibraryView->saveState();
	quazaaSettings.WinMain.LibraryDetailsVisible = ui->toolButtonLibraryDetailsToggle->isChecked();
}

void WidgetLibraryView::on_actionFind_triggered()
{
	QSkinDialog *dlgSkinLibrarySearch = new QSkinDialog(false, true, false, this);
	DialogLibrarySearch *dlgLibrarySearch = new DialogLibrarySearch;

	dlgSkinLibrarySearch->addChildWidget(dlgLibrarySearch);

	connect(dlgLibrarySearch, SIGNAL(closed()), dlgSkinLibrarySearch, SLOT(close()));
	dlgSkinLibrarySearch->show();
}
