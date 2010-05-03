#include "widgetgraph.h"
#include "ui_widgetgraph.h"

WidgetGraph::WidgetGraph(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetGraph)
{
    ui->setupUi(this);
}

WidgetGraph::~WidgetGraph()
{
    delete ui;
}

void WidgetGraph::changeEvent(QEvent *e)
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
