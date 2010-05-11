#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSecurity::WidgetSecurity(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSecurity)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.SecurityToolbars);
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

void WidgetSecurity::saveWidget()
{
	quazaaSettings.WinMain.SecurityToolbars = saveState();
}

void WidgetSecurity::on_actionSecurityAddRule_triggered()
{
	QSkinDialog *dlgSkinAddRule = new QSkinDialog(false, true, false, this);
	DialogAddRule *dlgAddRule = new DialogAddRule;

	dlgSkinAddRule->addChildWidget(dlgAddRule);

	connect(dlgAddRule, SIGNAL(closed()), dlgSkinAddRule, SLOT(close()));
	dlgSkinAddRule->show();
}

void WidgetSecurity::on_actionSubscribeSecurityList_triggered()
{
	QSkinDialog *dlgSkinSecuritySubscriptions = new QSkinDialog(false, true, false, this);
	DialogSecuritySubscriptions *dlgSecuritySubscriptions = new DialogSecuritySubscriptions;

	dlgSkinSecuritySubscriptions->addChildWidget(dlgSecuritySubscriptions);

	connect(dlgSecuritySubscriptions, SIGNAL(closed()), dlgSkinSecuritySubscriptions, SLOT(close()));
	dlgSkinSecuritySubscriptions->show();
}
