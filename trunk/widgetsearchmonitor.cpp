#include "widgetsearchmonitor.h"
#include "ui_widgetsearchmonitor.h"

WidgetSearchMonitor::WidgetSearchMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSearchMonitor)
{
    ui->setupUi(this);
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
