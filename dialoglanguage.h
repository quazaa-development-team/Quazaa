//
// dialoglanguage.h
//
// Copyright © Quazaa Development Team, 2009.
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

#ifndef DIALOGLANGUAGE_H
#define DIALOGLANGUAGE_H

#include <QtGui/QDialog>
#include <QListWidgetItem>

namespace Ui {
	class DialogLanguage;
}

class DialogLanguage : public QDialog {
	Q_OBJECT
public:
	DialogLanguage(QWidget *parent = 0);
	~DialogLanguage();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogLanguage *m_ui;

private slots:
	void on_listWidgetLanguages_itemClicked(QListWidgetItem* item);
	void on_pushButtonOK_clicked();
	void on_pushButtonCancel_clicked();
};

#endif // DIALOGLANGUAGE_H
