#include "widgetneighbors.h"
#include "ui_widgetneighbors.h"
#include "dialogsettings.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetNeighbors::WidgetNeighbors(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetNeighbors)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.NeighborsToolbars);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetNeighbors::~WidgetNeighbors()
{
    delete ui;
}

void WidgetNeighbors::changeEvent(QEvent *e)
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

void WidgetNeighbors::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->toolBarNetworks->setStyleSheet(skinSettings.toolbars);
}

void WidgetNeighbors::setModel(QAbstractItemModel *model)
{
	ui->tableViewNeighbors->setModel(model);
}

void WidgetNeighbors::saveWidget()
{
	quazaaSettings.WinMain.NeighborsToolbars = saveState();
}

void WidgetNeighbors::on_actionSettings_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSettings->switchSettingsPage(20);
	dlgSkinSettings->show();
}
