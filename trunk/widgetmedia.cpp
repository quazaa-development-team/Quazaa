#include "widgetmedia.h"
#include "ui_widgetmedia.h"

widgetMedia::widgetMedia(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetMedia)
{
    ui->setupUi(this);
}

widgetMedia::~widgetMedia()
{
    delete ui;
}

void widgetMedia::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
