//
// dialogfiltersearch.cpp
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

#include "dialogfiltersearch.h"
#include "ui_dialogfiltersearch.h"
#include "QSkinDialog/qskinsettings.h"

DialogFilterSearch::DialogFilterSearch(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogFilterSearch)
{
	m_ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogFilterSearch::~DialogFilterSearch()
{
	delete m_ui;
}

void DialogFilterSearch::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogFilterSearch::on_pushButtonFilter_clicked()
{
	emit closed();
	close();
}

void DialogFilterSearch::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogFilterSearch::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
