//
// dialogtransferprogresstooltip.cpp
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

#include "dialogtransferprogresstooltip.h"
#include "ui_dialogtransferprogresstooltip.h"
#include "QSkinDialog/qskinsettings.h"

DialogTransferProgressTooltip::DialogTransferProgressTooltip(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogTransferProgressTooltip)
{
	m_ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogTransferProgressTooltip::~DialogTransferProgressTooltip()
{
	delete m_ui;
}

void DialogTransferProgressTooltip::changeEvent(QEvent *e)
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

void DialogTransferProgressTooltip::updateProgress(int percent, QString transferSpeed, QString timeRemaining,
												   QString volumeDownloaded, QString numberSources, QPixmap icon,
												   QString file, QString size, QString type, QStringList hashes)
{
	m_ui->progressBarProgress->setValue(percent);
	m_ui->labelTransferSpeed->setText(transferSpeed);
	m_ui->labelTimeRemaining->setText(timeRemaining);
	m_ui->labelVolumeDownloaded->setText(volumeDownloaded);
	m_ui->labelNumberSources->setText(numberSources);
	m_ui->labelFileIcon->setPixmap(icon);
	m_ui->labelFileName->setText(file);
	m_ui->labelSize->setText(size);
	m_ui->labelType->setText(type);
	m_ui->labelSHA1Hash->setText(hashes.at(0));
	m_ui->labelTigerTreeHash->setText(hashes.at(1));
	m_ui->labelED2KHash->setText(hashes.at(2));
	m_ui->labelMD5Hash->setText(hashes.at(3));
}

void DialogTransferProgressTooltip::skinChangeEvent()
{
}
