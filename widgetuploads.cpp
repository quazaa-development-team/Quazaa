#include "widgetuploads.h"
#include "ui_widgetuploads.h"
#include "QSkinDialog/qskinsettings.h"

WidgetUploads::WidgetUploads(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetUploads)
{
    ui->setupUi(this);
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
