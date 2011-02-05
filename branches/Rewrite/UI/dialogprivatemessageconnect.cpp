#include "dialogprivatemessageconnect.h"
#include "ui_dialogprivatemessageconnect.h"

DialogPrivateMessageConnect::DialogPrivateMessageConnect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrivateMessageConnect)
{
    ui->setupUi(this);
}

DialogPrivateMessageConnect::~DialogPrivateMessageConnect()
{
    delete ui;
}

void DialogPrivateMessageConnect::changeEvent(QEvent *e)
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

void DialogPrivateMessageConnect::on_pushButtonConnect_clicked()
{
	switch (ui->comboBoxNetwork->currentIndex())
	{
	case PMNetwork::G2:
		if(ui->comboBoxG2Address->currentText().contains(":"))
		{
			CEndPoint ip(ui->comboBoxG2Address->currentText());
			emit startG2PrivateMessage(ip);
		}
		else
		{
			CEndPoint ip(ui->comboBoxG2Address->currentText() + ":" + ui->spinBoxG2Port->text());
			emit startG2PrivateMessage(ip);
		}
		break;
	case PMNetwork::eDonkey:
		//emit startEDonkeyPrivateMessage();
		break;
	case PMNetwork::Ares:
		//emit startAresPrivateMessage();
		break;
	case PMNetwork::IRC:
		//emit startIRCPrivateMessage();
		break;
	}
	close();
}

void DialogPrivateMessageConnect::on_pushButtonCancel_clicked()
{
	close();
}
