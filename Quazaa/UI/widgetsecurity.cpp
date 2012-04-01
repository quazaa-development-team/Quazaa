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

#include "securitytablemodel.h"

#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#include "timedsignalqueue.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetSecurity::WidgetSecurity(QWidget* parent) :
	QMainWindow( parent ),
	ui( new Ui::WidgetSecurity )
{
	ui->setupUi( this );

	m_pSecurityMenu = new QMenu( this );
	m_pSecurityMenu->addAction(  ui->actionSecurityModifyRule  );
	m_pSecurityMenu->addAction(  ui->actionSecurityRemoveRule  );
	m_pSecurityMenu->addAction(  ui->actionSecurityExportRules );

	restoreState( quazaaSettings.WinMain.SecurityToolbars );

	tableViewSecurity = new CTableView();
	tableViewSecurity->verticalHeader()->setVisible(false);
	ui->verticalLayoutSecurityTable->addWidget(tableViewSecurity);
	connect(tableViewSecurity, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_tableViewSecurity_customContextMenuRequested(QPoint)));
	connect(tableViewSecurity, SIGNAL(clicked(QModelIndex)), this, SLOT(on_tableViewSecurity_clicked(QModelIndex)));
	connect(tableViewSecurity, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_tableViewSecurity_doubleClicked(QModelIndex)));
	m_pSecurityList = new CSecurityTableModel( this, tableView() );
	setModel( m_pSecurityList );
	m_pSecurityList->sort( tableViewSecurity->horizontalHeader()->sortIndicatorSection(),
						   tableViewSecurity->horizontalHeader()->sortIndicatorOrder()    );
	setSkin();
}

WidgetSecurity::~WidgetSecurity()
{
	delete ui; // Note: This does also take care of m_pSecurityMenu and m_pSecurityList.
}

void WidgetSecurity::setModel(QAbstractItemModel* model)
{
	tableViewSecurity->setModel( model );
}

QWidget* WidgetSecurity::tableView()
{
	return tableViewSecurity;
}

void WidgetSecurity::saveWidget()
{
	quazaaSettings.WinMain.SecurityToolbars = saveState();
}

void WidgetSecurity::changeEvent(QEvent* e)
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

void WidgetSecurity::keyPressEvent(QKeyEvent *e)
{
	switch ( e->key() )
	{
	case Qt::Key_Delete:
	{
		on_actionSecurityRemoveRule_triggered();
		break;
	}

	case Qt::Key_Return:
	{
		on_actionSecurityModifyRule_triggered();
		break;
	}

	case Qt::Key_F5:
	{
		m_pSecurityList->completeRefresh();
		break;
	}
	}

	QMainWindow::keyPressEvent( e );
}

void WidgetSecurity::update()
{
	m_pSecurityList->updateAll();
}

void WidgetSecurity::on_actionSecurityAddRule_triggered()
{
	DialogAddRule* dlgAddRule = new DialogAddRule( this );
	connect( dlgAddRule, SIGNAL( dataUpdated() ), SLOT( update() ), Qt::QueuedConnection );
	dlgAddRule->show();
}

void WidgetSecurity::on_actionSecurityRemoveRule_triggered()
{
	QModelIndexList selection = tableViewSecurity->selectionModel()->selectedRows();

	// Lock security manager while fiddling with rules.
	QWriteLocker l( &securityManager.m_pRWLock );

	foreach( QModelIndex i, selection )
	{
		if ( i.isValid() )
		{
			security::CSecureRule* pRule = m_pSecurityList->nodeFromIndex( i );
			securityManager.remove( pRule, false );
		}
	}
}

void WidgetSecurity::on_actionSecurityModifyRule_triggered()
{
	QModelIndexList selection = tableViewSecurity->selectionModel()->selectedRows();
	QModelIndex index = QModelIndex();

	// Get the highest selected row.
	foreach( QModelIndex i, selection )
	{
		if ( index.isValid() )
		{
			if ( index.row() > i.row() )
				index = i;
		}
		else
		{
			index = i;
		}
	}

	if ( index.isValid() )
	{
		// Lock security manager while fiddling with rule.
		QReadLocker lock( &securityManager.m_pRWLock );

		security::CSecureRule* pRule = m_pSecurityList->nodeFromIndex( index );
		DialogAddRule* dlgAddRule = new DialogAddRule( this, pRule );

		lock.unlock(); // Make the Security Manager available again.

		connect( dlgAddRule, SIGNAL( dataUpdated() ), SLOT( update() ), Qt::QueuedConnection );
		dlgAddRule->show();
	}
}

void WidgetSecurity::on_actionSecurityImportRules_triggered()
{
	// TODO: Implement.
}

void WidgetSecurity::on_actionSecurityExportRules_triggered()
{
	// TODO: Implement.
}

void WidgetSecurity::on_actionSubscribeSecurityList_triggered()
{
	DialogSecuritySubscriptions* dlgSecuritySubscriptions = new DialogSecuritySubscriptions( this );
	dlgSecuritySubscriptions->show();
}

void WidgetSecurity::on_tableViewSecurity_customContextMenuRequested(const QPoint& point)
{
	QModelIndex index = tableViewSecurity->indexAt( point );

	if ( index.isValid() )
	{
		ui->actionSecurityExportRules->setEnabled( true );
		ui->actionSecurityModifyRule->setEnabled( true );
		ui->actionSecurityRemoveRule->setEnabled( true );
	}
	else
	{
		ui->actionSecurityExportRules->setEnabled( false );
		ui->actionSecurityModifyRule->setEnabled( false );
		ui->actionSecurityRemoveRule->setEnabled( false );
	}

	m_pSecurityMenu->popup( QCursor::pos() );
}

void WidgetSecurity::on_tableViewSecurity_doubleClicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		on_actionSecurityModifyRule_triggered();
	}
	else
	{
		ui->actionSecurityExportRules->setEnabled( false );
		ui->actionSecurityModifyRule->setEnabled( false );
		ui->actionSecurityRemoveRule->setEnabled( false );
	}
}

void WidgetSecurity::on_tableViewSecurity_clicked(const QModelIndex& index)
{
	if ( index.isValid() )
	{
		ui->actionSecurityExportRules->setEnabled( true );
		ui->actionSecurityModifyRule->setEnabled( true );
		ui->actionSecurityRemoveRule->setEnabled( true );
	}
	else
	{
		ui->actionSecurityExportRules->setEnabled( false );
		ui->actionSecurityModifyRule->setEnabled( false );
		ui->actionSecurityRemoveRule->setEnabled( false );
	}
}

void WidgetSecurity::setSkin()
{
	tableViewSecurity->setStyleSheet(skinSettings.listViews);
}
