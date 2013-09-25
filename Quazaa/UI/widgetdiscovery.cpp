/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
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

#include "debug_new.h"

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

	connect( tableViewDiscovery, &CTableView::clicked,
			 this, &WidgetDiscovery::tableViewDiscovery_clicked );
	connect( tableViewDiscovery, &CTableView::doubleClicked,
			 this, &WidgetDiscovery::tableViewDiscovery_doubleClicked );
	connect( tableViewDiscovery, &CTableView::customContextMenuRequested,
			 this, &WidgetDiscovery::tableViewDiscovery_customContextMenuRequested );

#else	// Qt4 code

	connect( tableViewDiscovery, SIGNAL( clicked(QModelIndex) ),
			 this, SLOT( tableViewDiscovery_clicked(QModelIndex) ) );
	connect( tableViewDiscovery, SIGNAL( doubleClicked(QModelIndex) ),
			 this, SLOT( tableViewDiscovery_doubleClicked(QModelIndex) ) );
	connect( tableViewDiscovery, SIGNAL( customContextMenuRequested(QPoint) ),
			 this, SLOT( tableViewDiscovery_customContextMenuRequested(QPoint) ) );

#endif

	m_pDiscoveryList = new CDiscoveryTableModel( this, tableView() );
	setModel( m_pDiscoveryList );
	m_pDiscoveryList->sort( tableViewDiscovery->horizontalHeader()->sortIndicatorSection(),
							tableViewDiscovery->horizontalHeader()->sortIndicatorOrder()    );

	tableViewDiscovery->horizontalHeader()->setStretchLastSection( false );
	setSkin();

	enum Column
	{
		TYPE = 0,
		URL,
		ACCESSED,
		HOSTS,
		TOTAL_HOSTS,
		ALTERNATE_SERVICES,
		FAILURES,
#if ENABLE_DISCOVERY_DEBUGGING
		RATING,
		MULTIPLICATOR,
#endif
		PONG,
		_NO_OF_COLUMNS
	};

	// Set up header sizes
	if ( !tableViewDiscovery->horizontalHeader()->restoreState(
			 quazaaSettings.WinMain.DiscoveryHeader ) )
	{
		QFontMetrics fm     = tableViewDiscovery->fontMetrics();
		QHeaderView* header = tableViewDiscovery->horizontalHeader();

		header->resizeSection( CDiscoveryTableModel::TYPE,
							   fm.width( " -Banned GWC- " ) );
		header->resizeSection( CDiscoveryTableModel::URL,
							   fm.width( " -http://cache2.bazookanetworks.com/g2/bazooka.php- " ) );
		header->resizeSection( CDiscoveryTableModel::ACCESSED,
							   fm.width( " ---Mi 20.Mrz 2013 19:21:45--- " ) );
		header->resizeSection( CDiscoveryTableModel::HOSTS,
							   fm.width( " -Hosts- " ) );
		header->resizeSection( CDiscoveryTableModel::TOTAL_HOSTS,
							   fm.width( " -Total Hosts- " ) );
		header->resizeSection( CDiscoveryTableModel::ALTERNATE_SERVICES,
							   fm.width( " -Alt. Services- " ) );
		header->resizeSection( CDiscoveryTableModel::FAILURES,
							   fm.width( " -Failures- " ) );
#if ENABLE_DISCOVERY_DEBUGGING
		header->resizeSection( CDiscoveryTableModel::RATING,
							   fm.width( " -Rating- " ) );
		header->resizeSection( CDiscoveryTableModel::MULTIPLICATOR,
							   fm.width( " -Mult.- " ) );
#endif
		header->resizeSection( CDiscoveryTableModel::PONG,
							   fm.width( " Some space to fill in the pong reply " ) );
	}
}

WidgetDiscovery::~WidgetDiscovery()
{
	delete ui; // Note: This does also take care of m_pDiscoveryMenu and m_pDiscoveryList.
	delete tableViewDiscovery; // TODO: check whether this is necessary...
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
	quazaaSettings.WinMain.DiscoveryHeader  = tableViewDiscovery->horizontalHeader()->saveState();
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

void WidgetDiscovery::update()
{
	// TODO: improve

	m_pDiscoveryList->updateAll();
}

void WidgetDiscovery::tableViewDiscovery_customContextMenuRequested(const QPoint& point)
{
	QModelIndex index = tableViewDiscovery->indexAt( point );

	if ( index.isValid() )
	{
		ui->actionDiscoveryAddService->setEnabled( true );
		ui->actionDiscoveryRemoveService->setEnabled( true );

		if ( m_pDiscoveryList->isIndexBanned( index ) )
		{
			ui->actionDiscoveryQueryNow->setEnabled( false );
			ui->actionDiscoveryAdvertise->setEnabled( false );
		}
		else
		{
			ui->actionDiscoveryQueryNow->setEnabled( true );
			ui->actionDiscoveryAdvertise->setEnabled( true );
		}
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

		if ( m_pDiscoveryList->isIndexBanned( index ) )
		{
			ui->actionDiscoveryQueryNow->setEnabled( false );
			ui->actionDiscoveryAdvertise->setEnabled( false );
		}
		else
		{
			ui->actionDiscoveryQueryNow->setEnabled( true );
			ui->actionDiscoveryAdvertise->setEnabled( true );
		}
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
	
	//see https://qt-project.org/wiki/New_Signal_Slot_Syntax
	
// TODO: Pop up dialog to edit service
	}
}
