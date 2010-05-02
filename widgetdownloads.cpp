#include "widgetdownloads.h"
#include "ui_widgetdownloads.h"

WidgetDownloads::WidgetDownloads(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetDownloads)
{
    ui->setupUi(this);
}

WidgetDownloads::~WidgetDownloads()
{
    delete ui;
}

void WidgetDownloads::changeEvent(QEvent *e)
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
