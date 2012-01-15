//
// dialoghashprogress.cpp
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

#include "dialoghashprogress.h"
#include "ui_dialoghashprogress.h"
#include "QSkinDialog/qskinsettings.h"

DialogHashProgress::DialogHashProgress(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogHashProgress)
{
	ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogHashProgress::~DialogHashProgress()
{
	delete ui;
}

void DialogHashProgress::changeEvent(QEvent* e)
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

void DialogHashProgress::updateProgress(int percent, QString status, QString file)
{
	ui->progressBarStatus->setValue(percent);
	ui->labelStatus->setText(status);
	ui->labelFileName->setText(file);
}

void DialogHashProgress::skinChangeEvent()
{
	setStyleSheet(skinSettings.standardItems);
}
