#include "widgetpacketdump.h"
#include "ui_widgetpacketdump.h"

WidgetPacketDump::WidgetPacketDump(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetPacketDump)
{
    ui->setupUi(this);
}

WidgetPacketDump::~WidgetPacketDump()
{
    delete ui;
}

void WidgetPacketDump::changeEvent(QEvent *e)
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
