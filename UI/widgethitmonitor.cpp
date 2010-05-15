#include "widgethitmonitor.h"
#include "ui_widgethitmonitor.h"
#include "dialogfiltersearch.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetHitMonitor::WidgetHitMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetHitMonitor)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.HitMonitorToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetHitMonitor::~WidgetHitMonitor()
{
    delete ui;
}

void WidgetHitMonitor::changeEvent(QEvent *e)
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

void WidgetHitMonitor::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}

void WidgetHitMonitor::saveWidget()
{
	quazaaSettings.WinMain.HitMonitorToolbar = saveState();
}

void WidgetHitMonitor::on_actionMore_triggered()
{
	QSkinDialog *dlgSkinFilterSearch = new QSkinDialog(false, true, false, this);
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch;

	dlgSkinFilterSearch->addChildWidget(dlgFilterSearch);

	connect(dlgFilterSearch, SIGNAL(closed()), dlgSkinFilterSearch, SLOT(close()));
	dlgSkinFilterSearch->show();
}
