#include "widgetchatcenter.h"
#include "ui_widgetchatcenter.h"
#include "dialogsettings.h"
#include "quazaairc.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetChatCenter::WidgetChatCenter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetChatCenter)
{
    ui->setupUi(this);
	lineEditTextInput = new QLineEdit();
	lineEditTextInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	toolButtonSmilies = new QToolButton();
	toolButtonSmilies->setToolTip("Smilies");
	toolButtonSmilies->setPopupMode(QToolButton::MenuButtonPopup);
	toolButtonSmilies->setIcon(QIcon(":/Resource/Smileys/Happy.png"));
	toolButtonOp = new QToolButton();
	toolButtonOp->setToolTip("Operator Commands");
	toolButtonOp->setPopupMode(QToolButton::MenuButtonPopup);
	toolButtonOp->setIcon(QIcon(":/Resource/Generic/QuazaaForums.png"));
	ui->toolBarChatMessage->insertWidget(ui->actionSend, lineEditTextInput);
	ui->toolBarOperator->addWidget(toolButtonSmilies);
	ui->toolBarOperator->addWidget(toolButtonOp);
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

void WidgetChatCenter::on_actionConnect_triggered()
{
    QuazaaIRC::QuazaaIRC();
	qDebug() << "Trying to connect to IRC";
}

void WidgetChatCenter::on_actionChatSettings_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSettings->switchSettingsPage(7);
	dlgSkinSettings->show();
}
