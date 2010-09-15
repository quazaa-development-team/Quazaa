//
// widgetneighbors.cpp
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

#include "widgetneighbors.h"
#include "ui_widgetneighbors.h"
#include "dialogsettings.h"
#include "dialogconnectto.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

#include "commonfunctions.h"
#include "network.h"
#include "datagrams.h"

WidgetNeighbors::WidgetNeighbors(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetNeighbors)
{
	ui->setupUi(this);
	labelG2StatsIcon = new QLabel();
	QPixmap const pixmapG2Stats = QPixmap(":/Resource/Networks/Gnutella2.png");
	labelG2StatsIcon->setPixmap(pixmapG2Stats);
	labelG2StatsIcon->setScaledContents(true);
	labelG2StatsIcon->setMaximumSize(16, 16);
	labelG2Stats = new QLabel();
	ui->toolBarG2->addWidget(labelG2StatsIcon);
	ui->toolBarG2->addWidget(labelG2Stats);
	restoreState(quazaaSettings.WinMain.NeighborsToolbars);
	ui->tableViewNeighbors->horizontalHeader()->restoreState(quazaaSettings.WinMain.NeighborsHeader);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetNeighbors::~WidgetNeighbors()
{
	delete ui;
}

void WidgetNeighbors::changeEvent(QEvent *e)
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

void WidgetNeighbors::skinChangeEvent()
{
	ui->toolBarControls->setStyleSheet(skinSettings.toolbars);
	ui->toolBarNetworks->setStyleSheet(skinSettings.toolbars);
	ui->toolBarAres->setStyleSheet(skinSettings.toolbars);
	ui->toolBarG2->setStyleSheet(skinSettings.toolbars);
	ui->toolBarEDonkey->setStyleSheet(skinSettings.toolbars);
}

void WidgetNeighbors::setModel(QAbstractItemModel *model)
{
	ui->tableViewNeighbors->setModel(model);
}

QWidget *WidgetNeighbors::treeView()
{
	return ui->tableViewNeighbors;
}

void WidgetNeighbors::saveWidget()
{
	quazaaSettings.WinMain.NeighborsToolbars = saveState();
	quazaaSettings.WinMain.NeighborsHeader = ui->tableViewNeighbors->horizontalHeader()->saveState();
}

void WidgetNeighbors::on_actionSettings_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, false, this);
	DialogSettings *dlgSettings = new DialogSettings(0);
	dlgSettings->switchSettingsPage(20);

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSkinSettings->show();
}

void WidgetNeighbors::updateG2()
{
	bool bEmit = (sender() != 0);
	quint16 nHubsConnected = 0;
	quint16 nLeavesConnected = 0;
	quint16 nTCPInSpeed = 0;
	quint16 nTCPOutSpeed = 0;
	quint16 nUDPInSpeed = 0;
	quint16 nUDPOutSpeed = 0;

	if( Network.m_pSection.tryLock(50) && bEmit  )
	{
		nHubsConnected = Network.m_nHubsConnected;
		nLeavesConnected = Network.m_nLeavesConnected;
		nTCPInSpeed = Network.DownloadSpeed();
		nTCPOutSpeed = Network.UploadSpeed();
		Network.m_pSection.unlock();
	}
	if( Datagrams.m_pSection.tryLock(50) && bEmit )
	{
		nUDPInSpeed = Datagrams.DownloadSpeed();
		nUDPOutSpeed = Datagrams.UploadSpeed();
		Datagrams.m_pSection.unlock();
	}
	labelG2Stats->setText(tr(" %1 Hubs, %2 Leaves, %3/s In:%4/s Out").arg(nHubsConnected).arg(nLeavesConnected).arg(Functions.FormatBytes(nTCPInSpeed + nUDPInSpeed)).arg(Functions.FormatBytes(nTCPOutSpeed + nUDPOutSpeed)));
}

void WidgetNeighbors::updateAres()
{

}

void WidgetNeighbors::updateEDonkey()
{

}

void WidgetNeighbors::on_actionNeighborConnectTo_triggered()
{
	QSkinDialog *dlgSkinConnectTo = new QSkinDialog(false, true, false, false, this);
	DialogConnectTo *dlgConnectTo = new DialogConnectTo;

	dlgSkinConnectTo->addChildWidget(dlgConnectTo);

	connect(dlgConnectTo, SIGNAL(closed()), dlgSkinConnectTo, SLOT(close()));
	dlgSkinConnectTo->show();
}

void WidgetNeighbors::on_actionNeighborDisconnect_triggered()
{

}
