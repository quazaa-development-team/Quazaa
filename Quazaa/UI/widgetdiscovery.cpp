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

#include "discoverytablemodel.h"

#include "widgetdiscovery.h"
#include "ui_widgetdiscovery.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetDiscovery::WidgetDiscovery(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetDiscovery)
{
	ui->setupUi(this);

	m_pDiscoveryMenu = new QMenu( this );

	restoreState( quazaaSettings.WinMain.DiscoveryToolbar );

	tableViewDiscovery = new CTableView();
	tableViewDiscovery->verticalHeader()->setVisible( false );
	ui->verticalLayoutDiscoveryTable->addWidget( tableViewDiscovery );

	connect(tableViewDiscovery, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_tableViewSecurity_customContextMenuRequested(QPoint)));
	connect(tableViewDiscovery, SIGNAL(clicked(QModelIndex)), this, SLOT(on_tableViewSecurity_clicked(QModelIndex)));
	connect(tableViewDiscovery, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_tableViewSecurity_doubleClicked(QModelIndex)));

	m_pDiscoveryList = new CDiscoveryTableModel( this, tableView() );
	setModel( m_pDiscoveryList );
	m_pDiscoveryList->sort( tableViewDiscovery->horizontalHeader()->sortIndicatorSection(),
							tableViewDiscovery->horizontalHeader()->sortIndicatorOrder()    );

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

void WidgetDiscovery::update()
{
	m_pDiscoveryList->updateAll();
}

void WidgetDiscovery::on_tableViewDiscovery_customContextMenuRequested(const QPoint& point)
{
	QModelIndex index = tableViewDiscovery->indexAt( point );

	if ( index.isValid() )
	{
		// Enable / disable menu items
	}
	else
	{
		// Enable / disable menu items
	}

	m_pDiscoveryMenu->popup( QCursor::pos() );
}

void WidgetDiscovery::on_tableViewDiscovery_doubleClicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		// Edit service
	}
	else
	{
		// Enable / disable menu items
	}
}

void WidgetDiscovery::on_tableViewDiscovery_clicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		// Enable / disable menu items
	}
	else
	{
		// Enable / disable menu items
	}
}

void WidgetDiscovery::setSkin()
{
	tableViewDiscovery->setStyleSheet( skinSettings.listViews );
}
