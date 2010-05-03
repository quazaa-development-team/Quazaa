#include "widgethostcache.h"
#include "ui_widgethostcache.h"

WidgetHostCache::WidgetHostCache(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetHostCache)
{
    ui->setupUi(this);
}

WidgetHostCache::~WidgetHostCache()
{
    delete ui;
}

void WidgetHostCache::changeEvent(QEvent *e)
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
