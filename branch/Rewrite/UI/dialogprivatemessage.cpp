#include "dialogprivatemessage.h"
#include "ui_dialogprivatemessage.h"

DialogPrivateMessage::DialogPrivateMessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrivateMessage)
{
	ui->setupUi(this);
}

DialogPrivateMessage::~DialogPrivateMessage()
{
    delete ui;
}

void DialogPrivateMessage::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return)
	{
		if (ui->checkBoxSendOnEnter->isChecked())
		{
			on_toolButtonSend_clicked();
		} else {
			QWidget::keyPressEvent(event);
		}
	} else {
		QWidget::keyPressEvent(event);
	}
}

void DialogPrivateMessage::on_toolButtonSend_clicked()
{
	if (!ui->textEditMessageInput->document()->isEmpty())
		emit sendMessage(ui->textEditMessageInput->document());
}

void DialogPrivateMessage::onMessage(QString)
{

}

void DialogPrivateMessage::onSystemMessage(QString)
{

}
