#include "widgetchatcenter.h"
#include "ui_widgetchatcenter.h"

WidgetChatCenter::WidgetChatCenter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetChatCenter)
{
    ui->setupUi(this);
}

WidgetChatCenter::~WidgetChatCenter()
{
    delete ui;
}

void WidgetChatCenter::changeEvent(QEvent *e)
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
