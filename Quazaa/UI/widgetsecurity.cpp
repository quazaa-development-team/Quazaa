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

#include "securitytablemodel.h"

#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#include "timedsignalqueue.h"

#include "debug_new.h"

#include <QMenu>
#include <QKeyEvent>

CWidgetSecurity::CWidgetSecurity(QWidget* parent) :
	QMainWindow( parent ),
	ui( new Ui::CWidgetSecurity )
{
	ui->setupUi( this );

	m_pSecurityMenu = new QMenu( this );
	m_pSecurityMenu->addAction(  ui->actionSecurityModifyRule  );
	m_pSecurityMenu->addAction(  ui->actionSecurityRemoveRule  );
	m_pSecurityMenu->addAction(  ui->actionSecurityExportRules );

	restoreState( quazaaSettings.WinMain.SecurityToolbars );

	tableViewSecurity = new CTableView();
	tableViewSecurity->verticalHeader()->setVisible( false );
	ui->verticalLayoutSecurityTable->addWidget( tableViewSecurity );

	connect(tableViewSecurity, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewSecurity_customContextMenuRequested(QPoint)));
	connect(tableViewSecurity, SIGNAL(clicked(QModelIndex)), this, SLOT(tableViewSecurity_clicked(QModelIndex)));
	connect(tableViewSecurity, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableViewSecurity_doubleClicked(QModelIndex)));

	m_pSecurityList = new CSecurityTableModel( this, tableView() );
	setModel( m_pSecurityList );
	m_pSecurityList->sort( tableViewSecurity->horizontalHeader()->sortIndicatorSection(),
						   tableViewSecurity->horizontalHeader()->sortIndicatorOrder()    );
	setSkin();
}

CWidgetSecurity::~CWidgetSecurity()
{
	delete ui; // Note: This does also take care of m_pSecurityMenu and m_pSecurityList.
}

void CWidgetSecurity::setModel(QAbstractItemModel* model)
{
	tableViewSecurity->setModel( model );
}

QWidget* CWidgetSecurity::tableView()
{
	return tableViewSecurity;
}

void CWidgetSecurity::saveWidget()
{
	quazaaSettings.WinMain.SecurityToolbars = saveState();
}

void CWidgetSecurity::changeEvent(QEvent* e)
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

void CWidgetSecurity::keyPressEvent(QKeyEvent *e)
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

void CWidgetSecurity::update()
{
	m_pSecurityList->updateAll();
}

void CWidgetSecurity::on_actionSecurityAddRule_triggered()
{
	CDialogAddRule* dlgAddRule = new CDialogAddRule( this );
	dlgAddRule->show();
}

void CWidgetSecurity::on_actionSecurityRemoveRule_triggered()
{
	QModelIndexList selection = tableViewSecurity->selectionModel()->selectedRows();

	// Lock security manager while fiddling with rules.
	QWriteLocker l( &securityManager.m_pRWLock );

	foreach( QModelIndex i, selection )
	{
		if ( i.isValid() )
		{
			CSecureRule* pRule = m_pSecurityList->nodeFromIndex( i );
			securityManager.remove( pRule, false );
		}
	}
}

void CWidgetSecurity::on_actionSecurityModifyRule_triggered()
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

		CSecureRule* pRule = m_pSecurityList->nodeFromIndex( index );
		CDialogAddRule* dlgAddRule = new CDialogAddRule( this, pRule );

		lock.unlock(); // Make the Security Manager available again.

		dlgAddRule->show();
	}
}

void CWidgetSecurity::on_actionSecurityImportRules_triggered()
{
	// TODO: Implement.
}

void CWidgetSecurity::on_actionSecurityExportRules_triggered()
{
	// TODO: Implement.
}

void CWidgetSecurity::on_actionSubscribeSecurityList_triggered()
{
	CDialogSecuritySubscriptions* dlgSecuritySubscriptions = new CDialogSecuritySubscriptions( this );
	dlgSecuritySubscriptions->show();
}

void CWidgetSecurity::tableViewSecurity_customContextMenuRequested(const QPoint& point)
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

void CWidgetSecurity::tableViewSecurity_doubleClicked(const QModelIndex& index)
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

void CWidgetSecurity::tableViewSecurity_clicked(const QModelIndex& index)
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

void CWidgetSecurity::setSkin()
{
	tableViewSecurity->setStyleSheet( skinSettings.listViews );
}
