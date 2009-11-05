//
// dialogscheduler.cpp
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

#include "dialogscheduler.h"
#include "ui_dialogscheduler.h"

DialogScheduler::DialogScheduler(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DialogScheduler)
{
    m_ui->setupUi(this);
}

DialogScheduler::~DialogScheduler()
{
    delete m_ui;
}

void DialogScheduler::changeEvent(QEvent *e)
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

void DialogScheduler::on_comboBoxAction_currentIndexChanged(int index)
{
	switch (index)
	{
		case 0:
			m_ui->spinBoxBandwidth->setEnabled(true);
			m_ui->labelNetworks->setEnabled(true);
			m_ui->checkBoxAres->setEnabled(true);
			m_ui->checkBoxBittorrent->setEnabled(true);
			m_ui->checkBoxED2k->setEnabled(true);
			m_ui->checkBoxG1->setEnabled(true);
			m_ui->checkBoxG2->setEnabled(true);
			m_ui->labelStop->setEnabled(true);
			m_ui->dateTimeEditStop->setEnabled(true);
		break;
		case 1:
			m_ui->spinBoxBandwidth->setEnabled(false);
			m_ui->labelNetworks->setEnabled(false);
			m_ui->checkBoxAres->setEnabled(false);
			m_ui->checkBoxBittorrent->setEnabled(false);
			m_ui->checkBoxED2k->setEnabled(false);
			m_ui->checkBoxG1->setEnabled(false);
			m_ui->checkBoxG2->setEnabled(false);
			m_ui->labelStop->setEnabled(true);
			m_ui->dateTimeEditStop->setEnabled(true);
		break;
		case 2:
			m_ui->spinBoxBandwidth->setEnabled(false);
			m_ui->labelNetworks->setEnabled(false);
			m_ui->checkBoxAres->setEnabled(false);
			m_ui->checkBoxBittorrent->setEnabled(false);
			m_ui->checkBoxED2k->setEnabled(false);
			m_ui->checkBoxG1->setEnabled(false);
			m_ui->checkBoxG2->setEnabled(false);
			m_ui->labelStop->setEnabled(false);
			m_ui->dateTimeEditStop->setEnabled(false);
		break;
		default:
		break;
	}
}

void DialogScheduler::on_pushButtonOK_clicked()
{
	if (m_ui->pushButtonApply->isEnabled())
	{
		m_ui->pushButtonApply->click();
	}
	this->close();
}

void DialogScheduler::on_pushButtonCancel_clicked()
{
	this->close();
}
