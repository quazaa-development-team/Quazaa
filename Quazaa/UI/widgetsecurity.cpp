﻿/*
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

#include <QMenu>
#include <QKeyEvent>

#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"

#include "securitytablemodel.h"
#include "dialogmodifyrule.h"
#include "dialogsecuritysubscriptions.h"
#include "dialogimportsecurity.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#include "debug_new.h"

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

	m_pTableViewSecurity = new CTableView();
	ui->verticalLayoutManual->addWidget(m_pTableViewSecurity);

	connect(m_pTableViewSecurity, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewSecurity_customContextMenuRequested(QPoint)));
	connect(m_pTableViewSecurity, SIGNAL(clicked(QModelIndex)), this, SLOT(tableViewSecurity_clicked(QModelIndex)));
	connect(m_pTableViewSecurity, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableViewSecurity_doubleClicked(QModelIndex)));

	m_pTableViewSecurityAuto = new CTableView();
	ui->verticalLayoutAuto->addWidget(m_pTableViewSecurityAuto);

	connect(m_pTableViewSecurityAuto, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewSecurityAuto_customContextMenuRequested(QPoint)));
	connect(m_pTableViewSecurityAuto, SIGNAL(clicked(QModelIndex)), this, SLOT(tableViewSecurity_clicked(QModelIndex)));
	connect(m_pTableViewSecurityAuto, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableViewSecurity_doubleClicked(QModelIndex)));

	m_pTableViewSecurity->horizontalHeader()->restoreState(quazaaSettings.WinMain.SecurityManualHeader);
	m_pTableViewSecurityAuto->horizontalHeader()->restoreState(quazaaSettings.WinMain.SecurityAutomaticHeader);

	m_lSecurity = new SecurityTableModel( this );

	m_lManual = new SecurityFilterModel(m_lSecurity, false);
	m_lAutomatic = new SecurityFilterModel(m_lSecurity, true);

	m_pTableViewSecurity->setModel( m_lManual );
	m_pTableViewSecurityAuto->setModel( m_lAutomatic );
	m_lManual->sort( m_pTableViewSecurity->horizontalHeader()->sortIndicatorSection(),
						   m_pTableViewSecurity->horizontalHeader()->sortIndicatorOrder()    );
	m_lAutomatic->sort( m_pTableViewSecurityAuto->horizontalHeader()->sortIndicatorSection(),
							m_pTableViewSecurityAuto->horizontalHeader()->sortIndicatorOrder()    );
	setSkin();
}

CWidgetSecurity::~CWidgetSecurity()
{
	delete ui; // Note: This does also take care of m_pSecurityMenu and m_pSecurityList.
}

void CWidgetSecurity::saveWidget()
{
	quazaaSettings.WinMain.SecurityToolbars = saveState();
	quazaaSettings.WinMain.SecurityManualHeader = m_pTableViewSecurity->horizontalHeader()->saveState();
	quazaaSettings.WinMain.SecurityAutomaticHeader = m_pTableViewSecurityAuto->horizontalHeader()->saveState();
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
		m_lSecurity->completeRefresh();
		break;
	}
	}

	QMainWindow::keyPressEvent( e );
}

void CWidgetSecurity::update()
{
	// TODO: Test whether this is needed.
	//m_lSecurity->updateAll();
}

void CWidgetSecurity::on_actionSecurityAddRule_triggered()
{
	CDialogModifyRule* dlgAddRule = new CDialogModifyRule( this );
	// connect(dlgAddRule, SIGNAL(accepted()), SLOT(update())); // not required
	dlgAddRule->show();
}

void CWidgetSecurity::on_actionSecurityRemoveRule_triggered()
{
	if ( ui->tabWidgetSecurity->currentIndex() == 1 )
	{
		QModelIndexList lSelection = m_pTableViewSecurityAuto->selectionModel()->selectedRows();

		foreach( QModelIndex i, lSelection )
		{
			if ( i.isValid() )
			{
				// map filter model index to table model index and request rule removal
				m_lSecurity->triggerRuleRemoval( m_lAutomatic->mapToSource( i ).row() );
			}
		}
	}
	else
	{
		QModelIndexList selection = m_pTableViewSecurity->selectionModel()->selectedRows();

		foreach( QModelIndex i, selection )
		{
			if ( i.isValid() )
			{
				// map filter model index to table model index and request rule removal
				m_lSecurity->triggerRuleRemoval( m_lManual->mapToSource( i ).row() );
			}
		}
	}
}

void CWidgetSecurity::on_actionSecurityModifyRule_triggered()
{
	CTableView* pTableView;
	SecurityFilterModel* pFilterModel;

	if ( ui->tabWidgetSecurity->currentIndex() == 1 )
	{
		pTableView = m_pTableViewSecurityAuto;
		pFilterModel = m_lAutomatic;
	}
	else
	{
		pTableView = m_pTableViewSecurity;
		pFilterModel = m_lManual;
	}

	QModelIndexList lSelection = pTableView->selectionModel()->selectedRows();
	QModelIndex index = QModelIndex();

	// Get the highest selected row.
	foreach( QModelIndex i, lSelection )
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
		QModelIndex i = pFilterModel->mapToSource( index );

		Q_ASSERT( i.isValid()  &&
				  i.row() >= 0 &&
				  i.row() < m_lSecurity->rowCount() );

		RuleDataPtr pData = m_lSecurity->dataFromRow( i.row() );

		Q_ASSERT( !pData.isNull() );

		CDialogModifyRule* dlgModifyRule = new CDialogModifyRule( this, pData );
		//connect(dlgAddRule, SIGNAL(accepted()), SLOT(update())); //not required/handled by manager
		dlgModifyRule->show();
	}
}

void CWidgetSecurity::on_actionSecurityImportRules_triggered()
{
	CDialogImportSecurity* dlgImportSecurity = new CDialogImportSecurity( this );
	dlgImportSecurity->exec();
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
	m_pTableViewSecurity->setStyleSheet( skinSettings.listViews );
	m_pTableViewSecurityAuto->setStyleSheet( skinSettings.listViews );
}

void CWidgetSecurity::tableViewSecurity_customContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = m_pTableViewSecurity->indexAt( pos );

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

void CWidgetSecurity::tableViewSecurityAuto_customContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = m_pTableViewSecurityAuto->indexAt( pos );

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
