#include "widgetchat.h"
#include "ui_widgetchat.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetChat::WidgetChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetChat)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	panelChatCenter = new WidgetChatCenter();
	ui->verticalLayoutChatCenter->addWidget(panelChatCenter);
	ui->toolButtonChatFriendsHeader->setChecked(quazaaSettings.WinMain.ChatFriendsTaskVisible);
	ui->toolButtonChatRoomsHeader->setChecked(quazaaSettings.WinMain.ChatRoomsTaskVisible);
}

WidgetChat::~WidgetChat()
{
	quazaaSettings.WinMain.ChatFriendsTaskVisible = ui->toolButtonChatFriendsHeader->isChecked();
	quazaaSettings.WinMain.ChatRoomsTaskVisible = ui->toolButtonChatRoomsHeader->isChecked();
	delete ui;
}

void WidgetChat::changeEvent(QEvent *e)
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

void WidgetChat::skinChangeEvent()
{
	ui->frameChatLeftSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->frameChatRightSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonChatFriendsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonChatRoomsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonChatUsersHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->frameChatUsersTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameChatFriendsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameChatRoomsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameChatWelcome->setStyleSheet(skinSettings.chatWelcome);
}
