#include "widgetlibrary.h"
#include "ui_widgetlibrary.h"

WidgetLibrary::WidgetLibrary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetLibrary)
{
    ui->setupUi(this);
    panelLibraryView = new WidgetLibraryView();
    ui->verticalLayoutLibraryView->addWidget(panelLibraryView);
}

WidgetLibrary::~WidgetLibrary()
{
    delete ui;
}

void WidgetLibrary::changeEvent(QEvent *e)
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
