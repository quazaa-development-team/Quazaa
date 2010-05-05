#include "widgetsearchmonitor.h"
#include "ui_widgetsearchmonitor.h"

#include "QSkinDialog/qskinsettings.h"

WidgetSearchMonitor::WidgetSearchMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSearchMonitor)
{
	ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetSearchMonitor::~WidgetSearchMonitor()
{
    delete ui;
}

void WidgetSearchMonitor::changeEvent(QEvent *e)
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

void WidgetSearchMonitor::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}
