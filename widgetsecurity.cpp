#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSecurity::WidgetSecurity(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSecurity)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetSecurity::~WidgetSecurity()
{
    delete ui;
}

void WidgetSecurity::changeEvent(QEvent *e)
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

void WidgetSecurity::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->toolBarSubscribe->setStyleSheet(skinSettings.toolbars);
}

void WidgetSecurity::saveState()
{

}
