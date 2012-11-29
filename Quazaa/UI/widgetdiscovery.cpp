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

#include "widgetdiscovery.h"
#include "ui_widgetdiscovery.h"

#include "discoverytablemodel.h"
#include "quazaasettings.h"
#include "skinsettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

#include <QMenu>
#include <QKeyEvent>

WidgetDiscovery::WidgetDiscovery(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetDiscovery)
{
	ui->setupUi(this);

	m_pDiscoveryMenu = new QMenu( this );
	m_pDiscoveryMenu->addAction( ui->actionDiscoveryAddService );
	m_pDiscoveryMenu->addAction( ui->actionDiscoveryRemoveService );
	m_pDiscoveryMenu->addAction( ui->actionDiscoveryQueryNow );
	m_pDiscoveryMenu->addAction( ui->actionDiscoveryAdvertise );

	restoreState( quazaaSettings.WinMain.DiscoveryToolbar );

	tableViewDiscovery = new CTableView();
	tableViewDiscovery->verticalHeader()->setVisible( false );
	ui->verticalLayoutDiscoveryTable->addWidget( tableViewDiscovery );

#if QT_VERSION >= 0x050000
	{
		connect( tableViewDiscovery, &CTableView::clicked,                    this, &WidgetDiscovery::tableViewDiscovery_clicked );
		connect( tableViewDiscovery, &CTableView::doubleClicked,              this, &WidgetDiscovery::tableViewDiscovery_doubleClicked );
		connect( tableViewDiscovery, &CTableView::customContextMenuRequested, this, &WidgetDiscovery::tableViewDiscovery_customContextMenuRequested );

	}
#else	// Qt4 code
		connect( tableViewDiscovery, SIGNAL(clicked(QModelIndex)),               this, SLOT(tableViewDiscovery_clicked(QModelIndex)));
		connect( tableViewDiscovery, SIGNAL(doubleClicked(QModelIndex)),         this, SLOT(tableViewDiscovery_doubleClicked(QModelIndex)));
		connect( tableViewDiscovery, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewDiscovery_customContextMenuRequested(QPoint)));
#endif




	//m_pDiscoveryList = new CDiscoveryTableModel( this, tableView() );
	//setModel( m_pDiscoveryList );
	//m_pDiscoveryList->sort( tableViewDiscovery->horizontalHeader()->sortIndicatorSection(),
	//						tableViewDiscovery->horizontalHeader()->sortIndicatorOrder()    );

	setSkin();
}

WidgetDiscovery::~WidgetDiscovery()
{
	delete ui; // Note: This does also take care of m_pDiscoveryMenu and m_pDiscoveryList.
}

void WidgetDiscovery::setModel(QAbstractItemModel* model)
{
	tableViewDiscovery->setModel( model );
}

QWidget* WidgetDiscovery::tableView()
{
	return tableViewDiscovery;
}

void WidgetDiscovery::saveWidget()
{
	quazaaSettings.WinMain.DiscoveryToolbar = saveState();
}

void WidgetDiscovery::changeEvent(QEvent* e)
{
	switch ( e->type() )
	{
	case QEvent::LanguageChange:
	{
		ui->retranslateUi( this );
		break;
	}

	default:
		break;
	}

	QMainWindow::changeEvent( e );
}

void WidgetDiscovery::keyPressEvent(QKeyEvent *e)
{
	switch ( e->key() )
	{
	case Qt::Key_Delete:
	{

		break;
	}

	case Qt::Key_Return:
	{

		break;
	}

	case Qt::Key_F5:
	{
		m_pDiscoveryList->completeRefresh();
		break;
	}
	}

	QMainWindow::keyPressEvent( e );
}

void WidgetDiscovery::setVisibility(CNetworkType networks, bool bHidden)
{
	for ( quint32 i = m_pDiscoveryList->rowCount(); i > 0; )
	{
		--i;

		if ( m_pDiscoveryList->nodeFromRow( i )->networkType().isNetwork( networks ) )
		{
			tableViewDiscovery->setRowHidden( i, bHidden );
		}
	}
}

void WidgetDiscovery::update()
{
	m_pDiscoveryList->updateAll();
}

void WidgetDiscovery::tableViewDiscovery_customContextMenuRequested(const QPoint& point)
{
	QModelIndex index = tableViewDiscovery->indexAt( point );

	if ( index.isValid() )
	{
		ui->actionDiscoveryAddService->setEnabled( true );
		ui->actionDiscoveryRemoveService->setEnabled( true );
		ui->actionDiscoveryQueryNow->setEnabled( true );
		ui->actionDiscoveryAdvertise->setEnabled( true );
	}
	else
	{
		ui->actionDiscoveryAddService->setEnabled( true );
		ui->actionDiscoveryRemoveService->setEnabled( false );
		ui->actionDiscoveryQueryNow->setEnabled( false );
		ui->actionDiscoveryAdvertise->setEnabled( false );
	}

	m_pDiscoveryMenu->popup( QCursor::pos() );
}

void WidgetDiscovery::tableViewDiscovery_doubleClicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		on_actionDiscoveryProperties_triggered();
	}
	else
	{
		ui->actionDiscoveryAddService->setEnabled( true );
		ui->actionDiscoveryRemoveService->setEnabled( false );
		ui->actionDiscoveryQueryNow->setEnabled( false );
		ui->actionDiscoveryAdvertise->setEnabled( false );
	}
}

void WidgetDiscovery::tableViewDiscovery_clicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		ui->actionDiscoveryAddService->setEnabled( true );
		ui->actionDiscoveryRemoveService->setEnabled( true );
		ui->actionDiscoveryQueryNow->setEnabled( true );
		ui->actionDiscoveryAdvertise->setEnabled( true );
	}
	else
	{
		ui->actionDiscoveryAddService->setEnabled( true );
		ui->actionDiscoveryRemoveService->setEnabled( false );
		ui->actionDiscoveryQueryNow->setEnabled( false );
		ui->actionDiscoveryAdvertise->setEnabled( false );
	}
}

void WidgetDiscovery::setSkin()
{
	tableViewDiscovery->setStyleSheet( skinSettings.listViews );
}

void WidgetDiscovery::on_actionDiscoveryGnutellaBootstrap_triggered(bool checked)
{
	setVisibility( CNetworkType( dpGnutella ), !checked );
}

void WidgetDiscovery::on_actionAresDiscoveryType_triggered(bool checked)
{
	setVisibility( CNetworkType( dpAres ), !checked );
}

void WidgetDiscovery::on_actionEDonkeyServerMet_triggered(bool checked)
{
	setVisibility( CNetworkType( dpeDonkey2000 ), !checked );
}

void WidgetDiscovery::on_actionGWebCache_triggered(bool checked)
{
	setVisibility( CNetworkType( dpG2 ), !checked );
}

void WidgetDiscovery::on_actionDiscoveryAddService_triggered()
{
	// TODO: Pop up dialog to add service
}

void WidgetDiscovery::on_actionDiscoveryBrowseStatistics_triggered()
{
	// TODO: Find out what this is ispposed to do.
}

void WidgetDiscovery::on_actionDiscoveryRemoveService_triggered()
{
	QModelIndexList selection = tableViewDiscovery->selectionModel()->selectedRows();

	foreach( QModelIndex i, selection )
	{
		if ( i.isValid() )
		{
			Q_ASSERT( discoveryManager.check( m_pDiscoveryList->nodeFromIndex( i ) ) );

			discoveryManager.remove( m_pDiscoveryList->nodeFromIndex( i )->id() );
		}
	}
}

void WidgetDiscovery::on_actionDiscoveryQueryNow_triggered()
{
	QModelIndex index = tableViewDiscovery->currentIndex();

	if ( index.isValid() )
	{
		Q_ASSERT( discoveryManager.check( m_pDiscoveryList->nodeFromIndex( index ) ) );

		discoveryManager.queryService( m_pDiscoveryList->nodeFromIndex( index )->id() );
	}
}

void WidgetDiscovery::on_actionDiscoveryAdvertise_triggered()
{
	QModelIndex index = tableViewDiscovery->currentIndex();

	if ( index.isValid() )
	{
		Q_ASSERT( discoveryManager.check( m_pDiscoveryList->nodeFromIndex( index ) ) );

		discoveryManager.updateService( m_pDiscoveryList->nodeFromIndex( index )->id() );
	}
}

void WidgetDiscovery::on_actionDiscoveryProperties_triggered()
{
	QModelIndex index = tableViewDiscovery->currentIndex();

	if ( index.isValid() )
	{
// TODO: Pop up dialog to edit service
	}
}
