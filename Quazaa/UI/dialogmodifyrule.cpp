/*
** dialogmodifyrule.cpp
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

#include "dialogmodifyrule.h"
#include "ui_dialogmodifyrule.h"
#include "skinsettings.h"

#include <QListView>
#include <QMessageBox>

#include "debug_new.h"

CDialogModifyRule::CDialogModifyRule(CWidgetSecurity* parent, CSecureRule* pRule) :
	QDialog( parent ),
	ui( new Ui::CDialogModifyRule ),
	m_pParent( parent )
{
	ui->setupUi( this );
	ui->comboBoxAction->setView( new QListView() );
	ui->comboBoxExpire->setView( new QListView() );
	ui->comboBoxRuleType->setView( new QListView() );


	m_pRule = pRule;

	if(m_pRule) { // We are modifying a rule.
		ui->comboBoxRuleType->setEnabled(false);
		ui->lineEditComment->setText( m_pRule->m_sComment );
		m_pRule->setLockForModify(true);

		switch ( m_pRule->type() )
		{
		case RuleType::IPAddressRange:
		{
			ui->comboBoxRuleType->setCurrentIndex( RuleIndex::IPAddressRange );
			ui->stackedWidgetType->setCurrentIndex( RuleIndex::IPAddressRange );
			QStringList lAddressRange = m_pRule->getContentString().split("-");
			ui->lineEditStartIP->setText( lAddressRange.at(0) );
			ui->lineEditEndIP->setText( lAddressRange.at(1) );
			break;
		}
		case RuleType::Hash:
			ui->comboBoxRuleType->setCurrentIndex( RuleIndex::Hash );
			ui->stackedWidgetType->setCurrentIndex( RuleIndex::Hash );
			break;
		case RuleType::Content:
			ui->comboBoxRuleType->setCurrentIndex( RuleIndex::Content );
			ui->stackedWidgetType->setCurrentIndex( RuleIndex::Content );
			ui->lineEditContent->setText( m_pRule->getContentString() );
			break;
		case RuleType::RegularExpression:
			ui->comboBoxRuleType->setCurrentIndex( RuleIndex::RegularExpression );
			ui->stackedWidgetType->setCurrentIndex( RuleIndex::RegularExpression );
			ui->lineEditRegularExpression->setText( m_pRule->getContentString() );
			break;
		case RuleType::UserAgent:
			ui->comboBoxRuleType->setCurrentIndex( RuleIndex::UserAgent );
			ui->stackedWidgetType->setCurrentIndex( RuleIndex::UserAgent );
			ui->checkBoxUserAgentRegularExpression->setChecked( ((CUserAgentRule*)m_pRule)->getRegExp() );
			ui->lineEditUserAgent->setText( m_pRule->getContentString() );
			break;
		default:
			ui->comboBoxRuleType->setCurrentIndex( RuleIndex::IPAddress );
			ui->stackedWidgetType->setCurrentIndex( RuleIndex::IPAddress );
			ui->lineEditIP->setText( ((CIPRule*)m_pRule)->getContentString() );
			break;
		}

		switch (m_pRule->m_nAction) {
			case RuleAction::Accept:
				ui->comboBoxAction->setCurrentIndex(1);
				break;
			case RuleAction::Deny:
				ui->comboBoxAction->setCurrentIndex(2);
				break;
			default:
				ui->comboBoxAction->setCurrentIndex(0);
				break;
		}

		quint32 tExpire = m_pRule->getExpiryTime();

		switch ( tExpire )
		{
		case RuleTime::Special:
			if(m_pRule->isForever()) {
				ui->comboBoxExpire->setCurrentIndex( 0 );
				ui->lineEditMinutes->setEnabled( false );
				ui->lineEditHours->setEnabled( false );
				ui->lineEditDays->setEnabled( false );

			} else {
				ui->comboBoxExpire->setCurrentIndex( 1 );
				ui->lineEditMinutes->setEnabled( false );
				ui->lineEditHours->setEnabled( false );
				ui->lineEditDays->setEnabled( false );
			}
			break;
		default:
			ui->comboBoxExpire->setCurrentIndex( 2 );
			ui->lineEditMinutes->setEnabled( true );
			ui->lineEditHours->setEnabled( true );
			ui->lineEditDays->setEnabled( true );

			tExpire -= common::getTNowUTC();

			tExpire /= 60; // minutes now
			ui->lineEditMinutes->setText( QString::number( tExpire % 60 ) );

			tExpire /= 60; // hours now
			ui->lineEditHours->setText(   QString::number( tExpire % 24 ) );
			ui->lineEditDays->setText(    QString::number( tExpire / 24 ) );
			break;
		}
	} else { // We are adding a rule.
		setWindowTitle(tr("Add Security Rule"));
		setWindowIcon(QIcon(":/Resource/Security/AddRule.png"));

		ui->comboBoxRuleType->setCurrentIndex( 0 );
		ui->stackedWidgetType->setCurrentIndex( 0 );
		ui->comboBoxAction->setCurrentIndex(0);
		ui->comboBoxExpire->setCurrentIndex( 0 );
	}

	setSkin();
}

CDialogModifyRule::~CDialogModifyRule()
{
	if(m_pRule)
		m_pRule->setLockForModify(false);
	delete ui;
}

void CDialogModifyRule::changeEvent(QEvent* e)
{
	QDialog::changeEvent( e );
	switch ( e->type() )
	{
		case QEvent::LanguageChange:
			ui->retranslateUi( this );
			break;
		default:
			break;
	}
}

// TODO: change user interface for IP ranges and hashes.
void CDialogModifyRule::on_pushButtonOK_clicked()
{
	bool bIsNewRule = false;

	if(!m_pRule) {
		bIsNewRule = true;
		switch ( ui->comboBoxRuleType->currentIndex() )
		{
			case RuleIndex::IPAddress:
				m_pRule = new CIPRule();
				break;
			case RuleIndex::IPAddressRange:
				m_pRule = new CIPRangeRule();
				break;
			case RuleIndex::Hash:
				m_pRule = new CHashRule();
				break;
			case RuleIndex::Content:
				m_pRule = new CContentRule();
				break;
			case RuleIndex::RegularExpression:
				m_pRule = new CRegularExpressionRule();
				break;
			case RuleIndex::UserAgent:
				m_pRule = new CUserAgentRule();
				break;
			default:
				Q_ASSERT( false );
		}
	}

	QString sTmp;

	switch ( m_pRule->type() )
	{
		case RuleType::IPAddress:
			sTmp = ui->lineEditIP->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("IP Address Rule Invalid"),
											   tr("The ip address rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::IPAddressRange:
			sTmp = QString("%1-%2").arg(ui->lineEditStartIP->text()).arg(ui->lineEditEndIP->text());
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("IP Address Range Rule Invalid"),
											   tr("The ip address range rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::Hash:
			sTmp = ui->lineEditURI->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("Hash Rule Invalid"),
											   tr("The hash rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::Content:
			sTmp = ui->lineEditContent->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("Content Rule Invalid"),
											   tr("The content rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			((CContentRule*)m_pRule)->setAll( ui->radioButtonMatchAll->isChecked() );
			break;
		case RuleType::RegularExpression:
			sTmp = ui->lineEditRegularExpression->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("Regular Expression Rule Invalid"),
											   tr("The regular expression rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			break;
		case RuleType::UserAgent:
			sTmp = ui->lineEditUserAgent->text();
			if ( !m_pRule->parseContent( sTmp ) )
			{
				QMessageBox::warning(this, tr("User Agent Rule Invalid"),
											   tr("The user agent rule is invalid. Please see the example for correct usage."),
											   QMessageBox::Ok,
											   QMessageBox::Ok);
				return;
			}
			((CUserAgentRule*)m_pRule)->setRegExp( ui->checkBoxUserAgentRegularExpression->isChecked() );
			break;
		default:
			Q_ASSERT( false );
	}

	switch (ui->comboBoxAction->currentIndex()) {
		case 0: // None
			m_pRule->m_nAction = RuleAction::None;
			break;
		case 1: // Accept
			m_pRule->m_nAction = RuleAction::Accept;
			break;
		case 2: // Deny
			m_pRule->m_nAction = RuleAction::Deny;
			break;
	}

	switch (ui->comboBoxExpire->currentIndex()) {
		case 0: // Forever
			m_pRule->setForever(true);
			m_pRule->setExpiryTime(RuleTime::Special);
			break;
		case 1: // Session
			m_pRule->setForever(false);
			m_pRule->setExpiryTime(RuleTime::Special);
			break;
		case 2: // Set Time
		{
			quint32 tExpire = 0;
			tExpire += ui->lineEditMinutes->text().toUShort() * 60;
			tExpire += ui->lineEditHours->text().toUShort() * 3600;
			tExpire += ui->lineEditDays->text().toUShort() * 216000;
			tExpire += common::getTNowUTC();

			m_pRule->setExpiryTime(tExpire);
			break;
		}
	}

	m_pRule->m_sComment = ui->lineEditComment->text();
	m_pRule->m_bAutomatic = false;

	if(bIsNewRule)
		securityManager.add(m_pRule);

	accept();
}

void CDialogModifyRule::on_pushButtonCancel_clicked()
{
	reject();
}

void CDialogModifyRule::on_comboBoxExpire_currentIndexChanged(int index)
{
	if ( index == 2 )
	{
		ui->lineEditDays->setEnabled( true );
		ui->lineEditHours->setEnabled( true );
		ui->lineEditMinutes->setEnabled( true );
	}
	else
	{
		ui->lineEditDays->setEnabled( false );
		ui->lineEditHours->setEnabled( false );
		ui->lineEditMinutes->setEnabled( false );
	}
}

void CDialogModifyRule::setSkin()
{

}

void CDialogModifyRule::on_lineEditDays_editingFinished()
{
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditDays->setText( QString::number( nDays ) );
}

void CDialogModifyRule::on_lineEditHours_editingFinished()
{
	quint64 nHours = ui->lineEditHours->text().toULong();
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	ui->lineEditDays->setText( QString::number( nDays ) );
}

void CDialogModifyRule::on_lineEditMinutes_editingFinished()
{
	quint64 nMinutes = ui->lineEditMinutes->text().toULong();
	quint64 nHours = ui->lineEditHours->text().toULong();
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditMinutes->setText( QString::number( nMinutes % 60 ) );
	nHours += nMinutes / 60;
	ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	ui->lineEditDays->setText( QString::number( nDays ) );
}
