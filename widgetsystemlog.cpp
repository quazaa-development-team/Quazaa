#include "widgetsystemlog.h"
#include "ui_widgetsystemlog.h"

WidgetSystemLog::WidgetSystemLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSystemLog)
{
    ui->setupUi(this);
}

WidgetSystemLog::~WidgetSystemLog()
{
    delete ui;
}

void WidgetSystemLog::changeEvent(QEvent *e)
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
