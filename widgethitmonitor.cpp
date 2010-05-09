#include "widgethitmonitor.h"
#include "ui_widgethitmonitor.h"
#include "QSkinDialog/qskinsettings.h"

WidgetHitMonitor::WidgetHitMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetHitMonitor)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetHitMonitor::~WidgetHitMonitor()
{
    delete ui;
}

void WidgetHitMonitor::changeEvent(QEvent *e)
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

void WidgetHitMonitor::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}

void WidgetHitMonitor::saveState()
{

}
