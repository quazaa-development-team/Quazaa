//
// dialogdownloadmonitor.cpp
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

#include "dialogdownloadmonitor.h"
#include "ui_dialogdownloadmonitor.h"

DialogDownloadMonitor::DialogDownloadMonitor(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DialogDownloadMonitor)
{
    m_ui->setupUi(this);
}

DialogDownloadMonitor::~DialogDownloadMonitor()
{
    delete m_ui;
}

void DialogDownloadMonitor::changeEvent(QEvent *e)
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

void DialogDownloadMonitor::on_pushButtonHide_clicked()
{
	close();
}

void DialogDownloadMonitor::on_pushButton_clicked()
{
	close();
}

void DialogDownloadMonitor::updateProgress(int percent, QString transferSpeed, QString timeRemaining,
										   QString volumeDownloaded, QString numberSources, QPixmap icon,
										   QString status, QString file)
{
	m_ui->progressBarStatus->setValue(percent);
	m_ui->labelEstimatedTimeRemaining->setText(timeRemaining);
	m_ui->labelTransferSpeed->setText(transferSpeed);
	m_ui->labelVolumeDownloaded->setText(volumeDownloaded);
	m_ui->labelNumberSources->setText(numberSources);
	m_ui->labelFileIcon->setPixmap(icon);
	m_ui->labelStatus->setText(status);
	m_ui->labelFileName->setText(file);
}
