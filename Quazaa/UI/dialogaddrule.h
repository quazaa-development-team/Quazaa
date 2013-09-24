/*
** dialogaddrule.h
**
** Copyright  Quazaa Development Team, 2009-2012.
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

#ifndef DIALOGADDRULE_H
#define DIALOGADDRULE_H

#include <QDialog>

#include "Security/securitymanager.h"
#include "Security/securerule.h"
#include "widgetsecurity.h"

namespace Ui
{
	class DialogAddRule;
}

class DialogAddRule : public QDialog
{
	Q_OBJECT

private:
	typedef Security::CSecureRule	 CSecureRule;
	typedef Security::CIPRule		 CIPRule;
	typedef Security::CIPRangeRule	 CIPRangeRule;
#if SECURITY_ENABLE_GEOIP
	typedef Security::CCountryRule	 CCountryRule;
#endif // SECURITY_ENABLE_GEOIP
	typedef Security::CHashRule		 CHashRule;
	typedef Security::CRegExpRule	 CRegExpRule;
	typedef Security::CUserAgentRule CUserAgentRule;
	typedef Security::CContentRule	 CContentRule;

	typedef Security::CSecurity		 CSecurity;

	Ui::DialogAddRule* ui;
	CSecureRule* m_pRule;
	WidgetSecurity* m_pParent;

public:
	// Creates a dialog window using a copy of the provided rule as basis.
	DialogAddRule(WidgetSecurity* parent, CSecureRule* pRule = NULL);
	~DialogAddRule();

protected:
	void changeEvent(QEvent* e);

private:

signals:
	void closed();

private slots:
	void on_pushButtonCancel_clicked();
	void on_pushButtonOK_clicked();
	void on_comboBoxExpire_currentIndexChanged(int index);
	void on_lineEditMinutes_lostFocus();
	void on_lineEditHours_lostFocus();
	void on_lineEditDays_lostFocus();
	void setSkin();
};

#endif // DIALOGADDRULE_H
