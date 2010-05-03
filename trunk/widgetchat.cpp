#include "widgetchat.h"
#include "ui_widgetchat.h"

WidgetChat::WidgetChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetChat)
{
    ui->setupUi(this);
	panelChatCenter = new WidgetChatCenter();
	ui->verticalLayoutChatCenter->addWidget(panelChatCenter);
}

WidgetChat::~WidgetChat()
{
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
