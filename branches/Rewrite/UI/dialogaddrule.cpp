/*
** dialogaddrule.cpp
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
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

#include "dialogaddrule.h"
#include "ui_dialogaddrule.h"
#include <QListView>

DialogAddRule::DialogAddRule(QWidget* parent) :
	QDialog(parent),
	m_ui(new Ui::DialogAddRule)
{
	m_ui->setupUi(this);
	m_ui->comboBoxAction->setView(new QListView());
	m_ui->comboBoxExpire->setView(new QListView());
	m_ui->comboBoxHashType->setView(new QListView());
	m_ui->comboBoxRuleType->setView(new QListView());
}

DialogAddRule::~DialogAddRule()
{
	delete m_ui;
}

void DialogAddRule::changeEvent(QEvent* e)
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

void DialogAddRule::on_pushButtonOK_clicked()
{
	emit closed();
	close();
}

void DialogAddRule::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}
