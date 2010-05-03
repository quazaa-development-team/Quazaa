#include "widgethitmonitor.h"
#include "ui_widgethitmonitor.h"

WidgetHitMonitor::WidgetHitMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetHitMonitor)
{
    ui->setupUi(this);
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
