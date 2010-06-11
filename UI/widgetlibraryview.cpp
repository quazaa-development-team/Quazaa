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
	ui->toolBarSearch->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameLibraryStatus->setStyleSheet(skinSettings.libraryViewHeader);
}

void WidgetLibraryView::saveWidget()
{
	quazaaSettings.WinMain.LibraryToolbar = saveState();
	quazaaSettings.WinMain.LibraryDetailsSplitter = ui->splitterLibraryView->saveState();
}

void WidgetLibraryView::on_actionFind_triggered()
{
	QSkinDialog *dlgSkinLibrarySearch = new QSkinDialog(false, true, false, this);
	DialogLibrarySearch *dlgLibrarySearch = new DialogLibrarySearch;

	dlgSkinLibrarySearch->addChildWidget(dlgLibrarySearch);

	connect(dlgLibrarySearch, SIGNAL(closed()), dlgSkinLibrarySearch, SLOT(close()));
	dlgSkinLibrarySearch->show();
}

void WidgetLibraryView::on_splitterLibraryView_customContextMenuRequested(QPoint pos)
{
	if (ui->splitterLibraryView->handle(1)->underMouse())
	{
		if (ui->splitterLibraryView->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.LibraryDetailsSplitterRestoreTop = ui->splitterLibraryView->sizes()[0];
			quazaaSettings.WinMain.LibraryDetailsSplitterRestoreBottom = ui->splitterLibraryView->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterLibraryView->sizes()[0] + ui->splitterLibraryView->sizes()[1]);
			newSizes.append(0);
			ui->splitterLibraryView->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.LibraryDetailsSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.LibraryDetailsSplitterRestoreBottom);
			ui->splitterLibraryView->setSizes(sizesList);
		}
	}
}
