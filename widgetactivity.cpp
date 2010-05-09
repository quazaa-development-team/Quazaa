#include "widgetactivity.h"
#include "ui_widgetactivity.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetActivity::WidgetActivity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetActivity)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterActivity->restoreState(quazaaSettings.WinMain.ActivitySplitter);
	panelNeighbors = new WidgetNeighbors();
	ui->verticalLayoutNeighbors->addWidget(panelNeighbors);
	panelSystemLog = new WidgetSystemLog();
	ui->verticalLayoutSystemLog->addWidget(panelSystemLog);
}

WidgetActivity::~WidgetActivity()
{
	delete ui;
}

void WidgetActivity::changeEvent(QEvent *e)
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

void WidgetActivity::skinChangeEvent()
{
	ui->toolButtonNeighborsHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonSystemLogHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
}

void WidgetActivity::saveState()
{
	quazaaSettings.WinMain.ActivitySplitter = ui->splitterActivity->saveState();
	panelNeighbors->saveState();
	panelSystemLog->saveState();
}
