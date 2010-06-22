#include "dialogconnectto.h"
#include "ui_dialogconnectto.h"

#include "QSkinDialog/qskinsettings.h"

#include "network.h"

DialogConnectTo::DialogConnectTo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnectTo)
{
    ui->setupUi(this);
}

DialogConnectTo::~DialogConnectTo()
{
    delete ui;
}

void DialogConnectTo::changeEvent(QEvent *e)
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

void DialogConnectTo::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogConnectTo::on_pushButtonConnect_clicked()
{

	IPv4_ENDPOINT ip(ui->lineEditIPAddress->text() + ":" + ui->spinBoxPort->text());

	Network.m_pSection.lock();
	Network.ConnectTo(ip);
	Network.m_pSection.unlock();

	emit closed();
	close();
}

void DialogConnectTo::skinChangeEvent()
{
	ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
