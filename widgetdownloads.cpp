#include "widgetdownloads.h"
#include "ui_widgetdownloads.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetDownloads::WidgetDownloads(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetDownloads)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.DownloadsToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetDownloads::~WidgetDownloads()
{
    delete ui;
}

void WidgetDownloads::changeEvent(QEvent *e)
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

void WidgetDownloads::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->toolBarFileTypes->setStyleSheet(skinSettings.toolbars);
	ui->toolBarFilter->setStyleSheet(skinSettings.toolbars);
}

void WidgetDownloads::saveWidget()
{
	quazaaSettings.WinMain.DownloadsToolbar = saveState();
}
