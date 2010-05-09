#include "widgetdiscovery.h"
#include "ui_widgetdiscovery.h"
#include "QSkinDialog/qskinsettings.h"

WidgetDiscovery::WidgetDiscovery(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetDiscovery)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetDiscovery::~WidgetDiscovery()
{
    delete ui;
}

void WidgetDiscovery::changeEvent(QEvent *e)
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

void WidgetDiscovery::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->toolBarProperties->setStyleSheet(skinSettings.toolbars);
	ui->toolBarServices->setStyleSheet(skinSettings.toolbars);
}

void WidgetDiscovery::saveState()
{

}
