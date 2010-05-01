#include "widgetmedia.h"
#include "ui_widgetmedia.h"

WidgetMedia::WidgetMedia(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetMedia)
{
    ui->setupUi(this);
}

WidgetMedia::~WidgetMedia()
{
    delete ui;
}

void WidgetMedia::changeEvent(QEvent *e)
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
