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

#include "dialogsecuritysubscriptions.h"
#include "ui_dialogsecuritysubscriptions.h"
#include "dialogaddsecuritysubscription.h"
#include "quazaasettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

DialogSecuritySubscriptions::DialogSecuritySubscriptions(QWidget* parent) :
	QDialog(parent),
	m_ui(new Ui::DialogSecuritySubscriptions)
{
	m_ui->setupUi(this);
}

DialogSecuritySubscriptions::~DialogSecuritySubscriptions()
{
	delete m_ui;
}

void DialogSecuritySubscriptions::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogSecuritySubscriptions::on_pushButtonAddSubscription_clicked()
{
	DialogAddSecuritySubscription* dlgAddSecuritySubscription = new DialogAddSecuritySubscription(this);

	dlgAddSecuritySubscription->show();
}

void DialogSecuritySubscriptions::on_pushButtonOK_clicked()
{
	if(m_ui->pushButtonApply->isEnabled())
	{
		m_ui->pushButtonApply->click();
	}
	emit closed();
	close();
}

void DialogSecuritySubscriptions::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

