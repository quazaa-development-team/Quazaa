#include "widgetscheduler.h"
#include "ui_widgetscheduler.h"
#include "QSkinDialog/qskinsettings.h"

WidgetScheduler::WidgetScheduler(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetScheduler)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetScheduler::~WidgetScheduler()
{
    delete ui;
}

void WidgetScheduler::changeEvent(QEvent *e)
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

void WidgetScheduler::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}
