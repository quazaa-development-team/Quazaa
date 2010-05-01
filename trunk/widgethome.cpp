#include "widgethome.h"
#include "ui_widgethome.h"

WidgetHome::WidgetHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetHome)
{
    ui->setupUi(this);
}

WidgetHome::~WidgetHome()
{
    delete ui;
}

void WidgetHome::changeEvent(QEvent *e)
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
