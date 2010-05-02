#include "widgetuploads.h"
#include "ui_widgetuploads.h"

WidgetUploads::WidgetUploads(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetUploads)
{
    ui->setupUi(this);
}

WidgetUploads::~WidgetUploads()
{
    delete ui;
}

void WidgetUploads::changeEvent(QEvent *e)
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
