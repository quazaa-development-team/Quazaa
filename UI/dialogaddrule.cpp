/*
** dialogaddrule.cpp
**
** Copyright  Quazaa Development Team, 2009-2011.
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

#include "dialogaddrule.h"
#include "ui_dialogaddrule.h"
#include <QListView>

DialogAddRule::DialogAddRule(QWidget* parent, RuleType::Type ruleType) :
	QDialog(parent),
	m_ui(new Ui::DialogAddRule)
{
	m_ui->setupUi(this);
	m_ui->comboBoxAction->setView(new QListView());
	m_ui->comboBoxExpire->setView(new QListView());
	m_ui->comboBoxHashType->setView(new QListView());
	m_ui->comboBoxRuleType->setView(new QListView());
	switch (ruleType)
	{
	case RuleType::Range:
		m_ui->comboBoxRuleType->setCurrentIndex(1);
		m_ui->stackedWidgetType->setCurrentIndex(1);
		break;
	case RuleType::Country:
		m_ui->comboBoxRuleType->setCurrentIndex(2);
		m_ui->stackedWidgetType->setCurrentIndex(2);
		break;
	case RuleType::Hash:
		m_ui->comboBoxRuleType->setCurrentIndex(3);
		m_ui->stackedWidgetType->setCurrentIndex(3);
		break;
	case RuleType::Any:
		m_ui->comboBoxRuleType->setCurrentIndex(4);
		m_ui->stackedWidgetType->setCurrentIndex(4);
		break;
	case RuleType::RegularExpression:
		m_ui->comboBoxRuleType->setCurrentIndex(5);
		m_ui->stackedWidgetType->setCurrentIndex(5);
		break;
	case RuleType::UserAgent:
		m_ui->comboBoxRuleType->setCurrentIndex(6);
		m_ui->stackedWidgetType->setCurrentIndex(6);
		break;
	default:
		m_ui->comboBoxRuleType->setCurrentIndex(0);
		m_ui->stackedWidgetType->setCurrentIndex(0);
		break;
	}
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
