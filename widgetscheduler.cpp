#include "widgetscheduler.h"
#include "ui_widgetscheduler.h"

WidgetScheduler::WidgetScheduler(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetScheduler)
{
    ui->setupUi(this);
}

WidgetScheduler::~WidgetScheduler()
{
    delete ui;
}

void WidgetScheduler::changeEvent(QEvent *e)
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
