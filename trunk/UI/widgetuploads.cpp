#include "widgetuploads.h"
#include "ui_widgetuploads.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetUploads::WidgetUploads(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetUploads)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.UploadsToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetUploads::~WidgetUploads()
{
	delete ui;
}

void WidgetUploads::changeEvent(QEvent *e)
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

void WidgetUploads::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->toolBarFilter->setStyleSheet(skinSettings.toolbars);
}

void WidgetUploads::saveWidget()
{
	quazaaSettings.WinMain.UploadsToolbar = saveState();
}
