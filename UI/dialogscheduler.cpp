//
// dialogscheduler.cpp
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

#include "dialogscheduler.h"
#include "ui_dialogscheduler.h"
#include "QSkinDialog/qskinsettings.h"
#include <QListView>


DialogScheduler::DialogScheduler(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogScheduler)
{
	ui->setupUi(this);
	ui->comboBoxAction->setView(new QListView());
	ui->comboBoxOnceWeekly->setView(new QListView());
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogScheduler::~DialogScheduler()
{
	delete ui;
}

void DialogScheduler::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogScheduler::on_comboBoxAction_currentIndexChanged(int index)
{
	switch(index)
	{
		case 0:
			ui->spinBoxBandwidth->setEnabled(true);
			ui->labelNetworks->setEnabled(true);
			ui->checkBoxAres->setEnabled(true);
			ui->checkBoxBitTorrent->setEnabled(true);
			ui->checkBoxEDonkey->setEnabled(true);
			ui->checkBoxG2->setEnabled(true);
			ui->labelStop->setEnabled(true);
			ui->dateTimeEditStop->setEnabled(true);
			break;
		case 1:
			ui->spinBoxBandwidth->setEnabled(false);
			ui->labelNetworks->setEnabled(false);
			ui->checkBoxAres->setEnabled(false);
			ui->checkBoxBitTorrent->setEnabled(false);
			ui->checkBoxEDonkey->setEnabled(false);
			ui->checkBoxG2->setEnabled(false);
			ui->labelStop->setEnabled(true);
			ui->dateTimeEditStop->setEnabled(true);
			break;
		case 2:
			ui->spinBoxBandwidth->setEnabled(false);
			ui->labelNetworks->setEnabled(false);
			ui->checkBoxAres->setEnabled(false);
			ui->checkBoxBitTorrent->setEnabled(false);
			ui->checkBoxEDonkey->setEnabled(false);
			ui->checkBoxG2->setEnabled(false);
			ui->labelStop->setEnabled(false);
			ui->dateTimeEditStop->setEnabled(false);
			break;
		default:
			ui->spinBoxBandwidth->setEnabled(true);
			ui->labelNetworks->setEnabled(true);
			ui->checkBoxAres->setEnabled(true);
			ui->checkBoxBitTorrent->setEnabled(true);
			ui->checkBoxEDonkey->setEnabled(true);
			ui->checkBoxG2->setEnabled(true);
			ui->labelStop->setEnabled(true);
			ui->dateTimeEditStop->setEnabled(true);
			break;
	}
}

void DialogScheduler::on_pushButtonOK_clicked()
{
	if(ui->pushButtonApply->isEnabled())
	{
		ui->pushButtonApply->click();
	}
	emit closed();
	close();
}

void DialogScheduler::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogScheduler::skinChangeEvent()
{
	ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
