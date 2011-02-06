//
// dialogconnectto.h
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

#ifndef DIALOGCONNECTTO_H
#define DIALOGCONNECTTO_H

#include <QDialog>
#include "types.h"
#include "network.h"

namespace Ui
{
	class DialogConnectTo;
}

class DialogConnectTo : public QDialog
{
	Q_OBJECT
public:
	DialogConnectTo(QWidget* parent = 0);
	~DialogConnectTo();
	enum ConnectNetwork{G2 = 0, eDonkey, Ares};

signals:
	void closed();

protected:
	void changeEvent(QEvent* e);

private:
	Ui::DialogConnectTo* ui;
	QString addressAndPort;
	ConnectNetwork connectNetwork;

public slots:
	QString getAddressAndPort();
	ConnectNetwork getConnectNetwork();
	void setAddressAndPort(QString newAddressAndPort);
	void setConnectNetwork(ConnectNetwork network);

private slots:
	void on_comboBoxNetwork_currentIndexChanged(int index);
	void on_pushButtonConnect_clicked();
	void on_pushButtonCancel_clicked();
};

#endif // DIALOGCONNECTTO_H
