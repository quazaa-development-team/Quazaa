#include "widgetsystemlog.h"
#include "ui_widgetsystemlog.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSystemLog::WidgetSystemLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSystemLog)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetSystemLog::~WidgetSystemLog()
{
	delete ui;
}

void WidgetSystemLog::changeEvent(QEvent *e)
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

void WidgetSystemLog::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}
