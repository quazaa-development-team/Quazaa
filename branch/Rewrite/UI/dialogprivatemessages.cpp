#include "dialogprivatemessages.h"
#include "ui_dialogprivatemessages.h"

DialogPrivateMessages::DialogPrivateMessages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrivateMessages)
{
    ui->setupUi(this);
	widgetChatInput = new WidgetChatInput(this);
	ui->horizontalLayoutChatInput->addWidget(widgetChatInput);
}

DialogPrivateMessages::~DialogPrivateMessages()
{
	delete widgetChatInput;
    delete ui;
}

void DialogPrivateMessages::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DialogPrivateMessages::addChatSession(CChatSession *newChatSession)
{

}
