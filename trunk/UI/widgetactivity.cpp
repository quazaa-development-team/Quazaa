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

void WidgetActivity::saveWidget()
{
	quazaaSettings.WinMain.ActivitySplitter = ui->splitterActivity->saveState();
	panelNeighbors->saveWidget();
	panelSystemLog->saveWidget();
}

void WidgetActivity::on_splitterActivity_customContextMenuRequested(QPoint pos)
{
	if (ui->splitterActivity->handle(1)->underMouse())
	{
		if (ui->splitterActivity->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.ActivitySplitterRestoreTop = ui->splitterActivity->sizes()[0];
			quazaaSettings.WinMain.ActivitySplitterRestoreBottom = ui->splitterActivity->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterActivity->sizes()[0] + ui->splitterActivity->sizes()[1]);
			newSizes.append(0);
			ui->splitterActivity->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.ActivitySplitterRestoreBottom);
			ui->splitterActivity->setSizes(sizesList);
		}
	}
}
