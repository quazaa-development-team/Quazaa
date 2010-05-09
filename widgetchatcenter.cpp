#include "widgetchatcenter.h"
#include "ui_widgetchatcenter.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetChatCenter::WidgetChatCenter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetChatCenter)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.ChatToolbars);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetChatCenter::~WidgetChatCenter()
{
    delete ui;
}

void WidgetChatCenter::changeEvent(QEvent *e)
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

void WidgetChatCenter::skinChangeEvent()
{
	ui->toolBarChatControls->setStyleSheet(skinSettings.chatToolbar);
	ui->toolBarChatMessage->setStyleSheet(skinSettings.chatToolbar);
	ui->toolBarOperator->setStyleSheet(skinSettings.chatToolbar);
}

void WidgetChatCenter::saveWidget()
{
	quazaaSettings.WinMain.ChatToolbars = saveState();
}
