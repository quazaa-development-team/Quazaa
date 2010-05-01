#include "widgetlibrary.h"
#include "ui_widgetlibrary.h"

widgetLibrary::widgetLibrary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetLibrary)
{
    ui->setupUi(this);
}

widgetLibrary::~widgetLibrary()
{
    delete ui;
}

void widgetLibrary::changeEvent(QEvent *e)
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
