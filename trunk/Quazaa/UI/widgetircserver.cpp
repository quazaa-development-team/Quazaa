#include "widgetircserver.h"
#include "ui_widgetircserver.h"

WidgetIRCServer::WidgetIRCServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetIRCServer)
{
    ui->setupUi(this);
}

WidgetIRCServer::~WidgetIRCServer()
{
    delete ui;
}
