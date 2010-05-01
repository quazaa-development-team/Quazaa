#include "widgetlibraryview.h"
#include "ui_widgetlibraryview.h"

widgetLibraryView::widgetLibraryView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::widgetLibraryView)
{
    ui->setupUi(this);
}

widgetLibraryView::~widgetLibraryView()
{
    delete ui;
}

void widgetLibraryView::changeEvent(QEvent *e)
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
