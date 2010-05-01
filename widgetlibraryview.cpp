#include "widgetlibraryview.h"
#include "ui_widgetlibraryview.h"

WidgetLibraryView::WidgetLibraryView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetLibraryView)
{
    ui->setupUi(this);
}

WidgetLibraryView::~WidgetLibraryView()
{
    delete ui;
}

void WidgetLibraryView::changeEvent(QEvent *e)
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
