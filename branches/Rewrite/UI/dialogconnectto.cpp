//
// dialogconnectto.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "dialogconnectto.h"
#include "ui_dialogconnectto.h"

#include <QListView>

DialogConnectTo::DialogConnectTo(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogConnectTo)
{
	ui->setupUi(this);
	ui->pushButtonConnect->setDefault(true);
	ui->comboBoxAddress->setView(new QListView());
	ui->comboBoxNetwork->setView(new QListView());
}

DialogConnectTo::~DialogConnectTo()
{
	delete ui;
}

void DialogConnectTo::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogConnectTo::on_pushButtonCancel_clicked()
{
	reject();
}

void DialogConnectTo::on_pushButtonConnect_clicked()
{
	if(ui->comboBoxAddress->currentText().contains(":"))
	{
		addressAndPort = ui->comboBoxAddress->currentText();
	}
	else
	{
		addressAndPort = ui->comboBoxAddress->currentText() + ":" + ui->spinBoxPort->text();
	}

	accept();
}


QString DialogConnectTo::getAddressAndPort()
{
	return addressAndPort;
}


DialogConnectTo::ConnectNetwork DialogConnectTo::getConnectNetwork()
{
	return connectNetwork;
}

void DialogConnectTo::setAddressAndPort(QString newAddressAndPort)
{
	addressAndPort = newAddressAndPort;
	QStringList addressAndPortList = newAddressAndPort.split(":");
	ui->comboBoxAddress->setEditText(addressAndPortList.value(0));
	QString port = addressAndPortList.value(1);
	ui->spinBoxPort->setValue(port.toInt());
}

void DialogConnectTo::setConnectNetwork(ConnectNetwork network)
{
	connectNetwork = network;
	ui->comboBoxNetwork->setCurrentIndex(network);
}

void DialogConnectTo::on_comboBoxNetwork_currentIndexChanged(int index)
{
	switch (index)
	{
	case 0:
		connectNetwork = DialogConnectTo::G2;
		break;
	case 1:
		connectNetwork = DialogConnectTo::eDonkey;
		break;
	case 2:
		connectNetwork = DialogConnectTo::Ares;
		break;
	}
}
