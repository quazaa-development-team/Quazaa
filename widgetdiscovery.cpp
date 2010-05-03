#include "widgetdiscovery.h"
#include "ui_widgetdiscovery.h"

WidgetDiscovery::WidgetDiscovery(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetDiscovery)
{
    ui->setupUi(this);
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
