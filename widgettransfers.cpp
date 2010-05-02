#include "widgettransfers.h"
#include "ui_widgettransfers.h"

WidgetTransfers::WidgetTransfers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetTransfers)
{
    ui->setupUi(this);
}

WidgetTransfers::~WidgetTransfers()
{
    delete ui;
}

void WidgetTransfers::changeEvent(QEvent *e)
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
