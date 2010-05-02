#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"

WidgetSecurity::WidgetSecurity(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSecurity)
{
    ui->setupUi(this);
}

WidgetSecurity::~WidgetSecurity()
{
    delete ui;
}

void WidgetSecurity::changeEvent(QEvent *e)
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
