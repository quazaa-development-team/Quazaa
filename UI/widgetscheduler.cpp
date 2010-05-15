#include "widgetscheduler.h"
#include "ui_widgetscheduler.h"
#include "dialogscheduler.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetScheduler::WidgetScheduler(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetScheduler)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.SchedulerToolbar);
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

void WidgetScheduler::saveWidget()
{
	quazaaSettings.WinMain.SchedulerToolbar = saveState();
}

void WidgetScheduler::on_actionAddScheduledTask_triggered()
{
	QSkinDialog *dlgSkinScheduler = new QSkinDialog(false, true, false, this);
	DialogScheduler *dlgScheduler = new DialogScheduler;

	dlgSkinScheduler->addChildWidget(dlgScheduler);

	connect(dlgScheduler, SIGNAL(closed()), dlgSkinScheduler, SLOT(close()));
	dlgSkinScheduler->show();
}

void WidgetScheduler::on_actionScheduleProperties_triggered()
{
	QSkinDialog *dlgSkinScheduler = new QSkinDialog(false, true, false, this);
	DialogScheduler *dlgScheduler = new DialogScheduler;

	dlgSkinScheduler->addChildWidget(dlgScheduler);

	connect(dlgScheduler, SIGNAL(closed()), dlgSkinScheduler, SLOT(close()));
	dlgSkinScheduler->show();
}
