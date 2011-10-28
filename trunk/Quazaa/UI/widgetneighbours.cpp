/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
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

#include "widgetneighbours.h"
#include "ui_widgetneighbours.h"
#include "dialogsettings.h"
#include "dialogconnectto.h"

#include "quazaasettings.h"

#include "commonfunctions.h"
#include "network.h"
#include "neighbours.h"
#include "neighbour.h"
#include "datagrams.h"
#include "neighbourstablemodel.h"

#include "chatsessiong2.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetNeighbours::WidgetNeighbours(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetNeighbours)
{
	ui->setupUi(this);
	neighboursMenu = new QMenu(ui->tableViewNeighbours);
	neighboursMenu->addAction(ui->actionNeighbourDisconnect);
	neighboursMenu->addAction(ui->actionNetworkBan);
	neighboursMenu->addSeparator();
	neighboursMenu->addAction(ui->actionNetworkChatWith);
	neighboursMenu->addAction(ui->actionViewProfile);
	neighboursMenu->addAction(ui->actionNetworkBrowse);
	labelG2StatsIcon = new QLabel();
	QPixmap const pixmapG2Stats = QPixmap(":/Resource/Networks/Gnutella2.png");
	labelG2StatsIcon->setPixmap(pixmapG2Stats);
	labelG2StatsIcon->setScaledContents(true);
	labelG2StatsIcon->setMaximumSize(16, 16);
	labelG2Stats = new QLabel();
	ui->toolBarG2->addWidget(labelG2StatsIcon);
	ui->toolBarG2->addWidget(labelG2Stats);
	restoreState(quazaaSettings.WinMain.NeighboursToolbars);
	ui->tableViewNeighbours->horizontalHeader()->restoreState(quazaaSettings.WinMain.NeighboursHeader);

	neighboursList = new CNeighboursTableModel(this, treeView());
	setModel(neighboursList);
	neighboursList->sort(ui->tableViewNeighbours->horizontalHeader()->sortIndicatorSection(), ui->tableViewNeighbours->horizontalHeader()->sortIndicatorOrder());
}

WidgetNeighbours::~WidgetNeighbours()
{
	quazaaSettings.WinMain.NeighboursHeader = ui->tableViewNeighbours->horizontalHeader()->saveState();
	delete ui;
}

void WidgetNeighbours::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WidgetNeighbours::setModel(QAbstractItemModel* model)
{
	ui->tableViewNeighbours->setModel(model);
}

QWidget* WidgetNeighbours::treeView()
{
	return ui->tableViewNeighbours;
}

void WidgetNeighbours::saveWidget()
{
	quazaaSettings.WinMain.NeighboursToolbars = saveState();
	quazaaSettings.WinMain.NeighboursHeader = ui->tableViewNeighbours->horizontalHeader()->saveState();
}

void WidgetNeighbours::on_actionSettings_triggered()
{
	DialogSettings* dlgSettings = new DialogSettings(this, SettingsPage::Protocols);
	dlgSettings->show();
}

void WidgetNeighbours::updateG2()
{
	quint32 nHubsConnected = 0;
	quint32 nLeavesConnected = 0;
	quint32 nTCPInSpeed = 0;
	quint32 nTCPOutSpeed = 0;
	quint32 nUDPInSpeed = 0;
	quint32 nUDPOutSpeed = 0;

	if(Neighbours.m_pSection.tryLock(50))
	{
		nHubsConnected = Neighbours.m_nHubsConnectedG2;
		nLeavesConnected = Neighbours.m_nLeavesConnectedG2;

		nTCPInSpeed = Neighbours.DownloadSpeed();
		nTCPOutSpeed = Neighbours.UploadSpeed();

		if(Network.m_pSection.tryLock(50))
		{
			nUDPInSpeed = Datagrams.DownloadSpeed();
			nUDPOutSpeed = Datagrams.UploadSpeed();

			Network.m_pSection.unlock();
		}

		Neighbours.m_pSection.unlock();
	}

	labelG2Stats->setText(tr(" %1 Hubs, %2 Leaves, %3/s In:%4/s Out").arg(nHubsConnected).arg(nLeavesConnected).arg(Functions.FormatBytes(nTCPInSpeed + nUDPInSpeed)).arg(Functions.FormatBytes(nTCPOutSpeed + nUDPOutSpeed)));
}

void WidgetNeighbours::updateAres()
{

}

void WidgetNeighbours::updateEDonkey()
{

}

void WidgetNeighbours::onTimer()
{
	neighboursList->UpdateAll();
	updateG2();
}

void WidgetNeighbours::on_actionNeighbourConnectTo_triggered()
{
	DialogConnectTo* dlgConnectTo = new DialogConnectTo(this);
	bool accepted = dlgConnectTo->exec();

	if (accepted)
	{
		CEndPoint ip(dlgConnectTo->getAddressAndPort());

		switch (dlgConnectTo->getConnectNetwork())
		{
		case DialogConnectTo::G2:
			Neighbours.m_pSection.lock();
			Neighbours.ConnectTo(ip, dpGnutella2);
			Neighbours.m_pSection.unlock();
			break;
		case DialogConnectTo::eDonkey:
			break;
		case DialogConnectTo::Ares:
			break;
		default:
			break;
		}
	}
}

void WidgetNeighbours::on_actionNeighbourDisconnect_triggered()
{
	QModelIndex idx = ui->tableViewNeighbours->currentIndex();
	CNeighbour* pNode = neighboursList->NodeFromIndex(idx);

	if( pNode == 0 )
		return;

	Neighbours.m_pSection.lock();
	if( Neighbours.NeighbourExists(pNode) )
	{
		systemLog.postLog(LogCategory::Network, LogSeverity::Information, qPrintable(tr("Closing connection to neighbour %s")), qPrintable(pNode->m_oAddress.toStringWithPort()));
		pNode->Close();
	}
	Neighbours.m_pSection.unlock();
}

void WidgetNeighbours::on_tableViewNeighbours_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);
	neighboursMenu->exec(QCursor::pos());
}

void WidgetNeighbours::on_actionNetworkChatWith_triggered()
{
	QMutexLocker l(&Neighbours.m_pSection);

	if (ui->tableViewNeighbours->currentIndex().isValid())
	{
		QModelIndex idx = ui->tableViewNeighbours->currentIndex();
		CNeighbour* pNode = neighboursList->NodeFromIndex(idx);

		if( pNode == 0 )
			return;

		switch( pNode->m_nProtocol)
		{
		case dpGnutella2:
		{
			CChatSessionG2* pS = new CChatSessionG2(pNode->m_oAddress);
			pS->Connect();
			break;
		}
		default:
			break;

		}
	}
}

