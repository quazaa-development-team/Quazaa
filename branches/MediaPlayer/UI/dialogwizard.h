//
// dialogwizard.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef DIALOGWIZARD_H
#define DIALOGWIZARD_H

#include <QtGui/QDialog>

namespace Ui
{
	class DialogWizard;
}

class DialogWizard : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY(DialogWizard)
public:
	explicit DialogWizard(QWidget* parent = 0);
	virtual ~DialogWizard();

protected:
	virtual void changeEvent(QEvent* e);

private:
	Ui::DialogWizard* m_ui;

signals:
	void closed();

private slots:
	void on_pushButtonSharesRemove_clicked();
	void on_pushButtonSharesAdd_clicked();
	void on_pushButtonSystemExit_clicked();
	void on_pushButtonSystemFinish_clicked();
	void on_pushButtonSystemBack_clicked();
	void on_pushButtonNetworksExit_clicked();
	void on_pushButtonNetworksNext_clicked();
	void on_pushButtonNetworksBack_clicked();
	void on_pushButtonChatExit_clicked();
	void on_pushButtonChatNext_clicked();
	void on_pushButtonChatBack_clicked();
	void on_pushButtonSharesExit_clicked();
	void on_pushButtonSharesNext_clicked();
	void on_pushButtonSharesBack_clicked();
	void on_pushButtonWelcomeExit_clicked();
	void on_pushButtonWelcomeNext_clicked();
	void on_pushButtonConnectionExit_clicked();
	void on_pushButtonConnectionNext_clicked();
	void on_pushButtonConnectionBack_clicked();
};

#endif // DIALOGWIZARD_H
