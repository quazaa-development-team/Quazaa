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

#include "securitytablemodel.h"

#include "widgetsecurity.h"
#include "ui_widgetsecurity.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"

#include "quazaasettings.h"

#include "timedsignalqueue.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetSecurity::WidgetSecurity(QWidget* parent) :
	QMainWindow( parent ),
	ui( new Ui::WidgetSecurity )
{
	ui->setupUi( this );
	restoreState( quazaaSettings.WinMain.SecurityToolbars );

	m_pSecurityList = new CSecurityTableModel( this, treeView() );
	setModel( m_pSecurityList );

	connect( this, SIGNAL( requestDataUpdate() ), this, SLOT( update() ) );
	signalQueue.push( this, "requestDataUpdate", 1000, true );
}

WidgetSecurity::~WidgetSecurity()
{
	delete ui;
}

void WidgetSecurity::setModel(QAbstractItemModel* model)
{
	ui->tableViewSecurity->setModel( model );
}

QWidget* WidgetSecurity::treeView()
{
	return ui->tableViewSecurity;
}

void WidgetSecurity::saveWidget()
{
	quazaaSettings.WinMain.SecurityToolbars = saveState();
}

void WidgetSecurity::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent( e );
	switch ( e->type() )
	{
		case QEvent::LanguageChange:
			ui->retranslateUi( this );
			break;
		default:
			break;
	}
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

void WidgetSecurity::on_actionSecurityModifyRule_triggered()
{
	QModelIndex index = ui->tableViewSecurity->currentIndex();

	if ( index.isValid() )
	{
		security::CSecureRule* pRule = m_pSecurityList->nodeFromIndex( index );
		DialogAddRule* dlgAddRule = new DialogAddRule( this, pRule );
		dlgAddRule->show();
	}
}

void WidgetSecurity::on_actionSubscribeSecurityList_triggered()
{
	DialogSecuritySubscriptions* dlgSecuritySubscriptions = new DialogSecuritySubscriptions( this );
	dlgSecuritySubscriptions->show();
}

