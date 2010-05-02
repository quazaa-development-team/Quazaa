#include "widgetactivity.h"
#include "ui_widgetactivity.h"

WidgetActivity::WidgetActivity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetActivity)
{
    ui->setupUi(this);
}

WidgetActivity::~WidgetActivity()
{
    delete ui;
}

void WidgetActivity::changeEvent(QEvent *e)
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
