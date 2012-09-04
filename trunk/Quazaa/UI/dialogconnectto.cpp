/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "dialogconnectto.h"
#include "ui_dialogconnectto.h"
#include "skinsettings.h"

#include <QtWidgets/QListView>
#include <QtWidgets/QMessageBox>

#ifdef _DEBUG
#include "debug_new.h"
#endif

DialogConnectTo::DialogConnectTo(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogConnectTo)
{
	ui->setupUi(this);
	ui->pushButtonConnect->setDefault(true);
	ui->comboBoxAddress->setView(new QListView());
	ui->comboBoxNetwork->setView(new QListView());
	setConnectNetwork(DialogConnectTo::G2);
	ui->comboBoxAddress->setFocus();
	setSkin();
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
	CEndPoint tempAddress;
	if(ui->comboBoxAddress->currentText().count(":") > 1) // ipv6 address, check if includes port
	{
		if(ui->comboBoxAddress->currentText().contains("[") && ui->comboBoxAddress->currentText().contains("]")
			&& !ui->comboBoxAddress->currentText().endsWith("]")) // ipv6 address with port
		{
			tempAddress.setAddressWithPort(ui->comboBoxAddress->currentText());
		} else { // ipv6 address without port
			tempAddress.setAddress(ui->comboBoxAddress->currentText());
			tempAddress.setPort(ui->spinBoxPort->value());

		}
	}
	else if(ui->comboBoxAddress->currentText().contains(":") ) //ipv4 address with port
	{
		tempAddress.setAddressWithPort(ui->comboBoxAddress->currentText());
	}
	else //ipv4 address
	{
		tempAddress.setAddress(ui->comboBoxAddress->currentText());
		tempAddress.setPort(ui->spinBoxPort->value());
	}

	if ((QAbstractSocket::IPv4Protocol == tempAddress.protocol()) || (QAbstractSocket::IPv6Protocol == tempAddress.protocol()))
	{
		addressAndPort = tempAddress.toStringWithPort();
		accept();
	} else {
		QMessageBox msgBox;
		 msgBox.setText(tr("Address is invalid."));
		 msgBox.setInformativeText(tr("Please enter a valid IP Address."));
		 msgBox.setStandardButtons(QMessageBox::Ok);
		 msgBox.setDefaultButton(QMessageBox::Ok);
		 msgBox.exec();
	}
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
	CEndPoint address(newAddressAndPort);
	ui->comboBoxAddress->setEditText(address.toString());
	ui->spinBoxPort->setValue(address.port());
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

void DialogConnectTo::setSkin()
{

}
