#include "widgethome.h"
#include "ui_widgethome.h"

widgetHome::widgetHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetHome)
{
    ui->setupUi(this);
}

widgetHome::~widgetHome()
{
    delete ui;
}

void widgetHome::changeEvent(QEvent *e)
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
