#include "widgetchatinput.h"
#include "ui_widgetchatinput.h"

WidgetChatInput::WidgetChatInput(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetChatInput)
{
    ui->setupUi(this);
	textEditInput = new WidgetReturnEmitTextEdit(this);
	ui->horizontalLayoutInput->addWidget(textEditInput);
	checkBoxSendOnEnter = new QCheckBox(tr("Send On Enter"), this);
	checkBoxSendOnEnter->setChecked(true);
	connect(checkBoxSendOnEnter, SIGNAL(toggled(bool)), textEditInput, SLOT(setEmitsReturn(bool)));
	connect(textEditInput, SIGNAL(returnPressed()), ui->toolButtonSend, SLOT(click()));
	toolButtonSmilies = new QToolButton();
	toolButtonSmilies->setToolTip("Smilies");
	toolButtonSmilies->setPopupMode(QToolButton::MenuButtonPopup);
	toolButtonSmilies->setIcon(QIcon(":/Resource/Smileys/colonClosingparentheses.png"));
	ui->toolBarTextTools->addSeparator();
	ui->toolBarTextTools->addWidget(toolButtonSmilies);
	ui->toolBarTextTools->addWidget(checkBoxSendOnEnter);
}

WidgetChatInput::~WidgetChatInput()
{
    delete ui;
}

void WidgetChatInput::changeEvent(QEvent *e)
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

void WidgetChatInput::on_toolButtonSend_clicked()
{
	if (!textEditInput->document()->isEmpty())
	{
		emit messageSent(textEditInput->document());
		emit messageSent(textEditInput->document()->toPlainText());
		textEditInput->document()->clear();
	}
}

void WidgetChatInput::setText(QString text)
{
	textEditInput->setHtml(text);
}
