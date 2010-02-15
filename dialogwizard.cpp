//
// dialogwizard.cpp
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

#include "dialogwizard.h"
#include "ui_dialogwizard.h"
#include "quazaaSettings.h"
#include <QFileDialog>

DialogWizard::DialogWizard(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogWizard)
{
	m_ui->setupUi(this);
	m_ui->doubleSpinBoxDownloadSpeed->setValue(quazaaSettings.TransfersBandwidthDownloads);
	m_ui->doubleSpinBoxUploadSpeed->setValue(quazaaSettings.TransfersBandwidthUploads);
	m_ui->checkBoxUPnP->setChecked(quazaaSettings.SecurityEnableUPnP);
	m_ui->checkBoxAllowBrowseShares->setChecked(quazaaSettings.SecurityAllowSharesBrowse);
	m_ui->listWidgetShares->addItems(quazaaSettings.LibraryShares);
	m_ui->checkBoxAllowBrowseProfile->setChecked(quazaaSettings.SecurityAllowProfileBrowse);
	m_ui->lineEditIrcNick->setText(quazaaSettings.ProfileIrcNickname);
	m_ui->lineEditIrcAlternateNick->setText(quazaaSettings.ProfileIrcAlternateNickname);
	m_ui->lineEditGnutellaScreenName->setText(quazaaSettings.ProfileGnutellaScreenName);
	m_ui->comboBoxGender->setCurrentIndex(quazaaSettings.ProfileGender);
	m_ui->spinBoxAge->setValue(quazaaSettings.ProfileAge);
	m_ui->lineEditCountry->setText(quazaaSettings.ProfileCountry);
	m_ui->lineEditStateProvince->setText(quazaaSettings.ProfileStateProvince);
	m_ui->lineEditCity->setText(quazaaSettings.ProfileCity);
	m_ui->checkBoxG2Connect->setChecked(quazaaSettings.Gnutella2Enable);
	m_ui->checkBoxG1Connect->setChecked(quazaaSettings.Gnutella1Enable);
	m_ui->checkBoxAresConnect->setChecked(quazaaSettings.AresEnable);
	m_ui->checkBoxED2kConnect->setChecked(quazaaSettings.EDonkeyEnable);
	m_ui->checkBoxKadConnect->setChecked(quazaaSettings.EDonkeyEnableKad);
	m_ui->checkBoxBitTorrentLinks->setChecked(quazaaSettings.WebTorrent);
	m_ui->checkBoxBitTorrentKademliaConnect->setChecked(quazaaSettings.BitTorrentUseKademlia);
	m_ui->checkBoxConnectNetworksAlways->setChecked(quazaaSettings.BasicConnectOnStartup);
	m_ui->checkBoxOSStart->setChecked(quazaaSettings.BasicStartWithSystem);
	m_ui->checkBoxSimpleProgress->setChecked(quazaaSettings.TransfersSimpleProgressBar);
	m_ui->checkBoxSlowHashing->setChecked(!quazaaSettings.LibraryHighPriorityHashing);
	m_ui->pagesWizard->setCurrentIndex(0);
}

DialogWizard::~DialogWizard()
{
	delete m_ui;
}

void DialogWizard::changeEvent(QEvent *e)
{
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogWizard::on_pushButtonConnectionBack_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(0);
}

void DialogWizard::on_pushButtonConnectionNext_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(2);
}

void DialogWizard::on_pushButtonConnectionExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonWelcomeNext_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(1);
}

void DialogWizard::on_pushButtonWelcomeExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonSharesBack_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(1);
}

void DialogWizard::on_pushButtonSharesNext_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(3);
}

void DialogWizard::on_pushButtonSharesExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonChatBack_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(2);
}

void DialogWizard::on_pushButtonChatNext_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(4);
}

void DialogWizard::on_pushButtonChatExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonNetworksBack_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(3);
}

void DialogWizard::on_pushButtonNetworksNext_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(5);
}

void DialogWizard::on_pushButtonNetworksExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonSystemBack_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(4);
}

void DialogWizard::on_pushButtonSystemFinish_clicked()
{
	quazaaSettings.TransfersBandwidthDownloads = m_ui->doubleSpinBoxDownloadSpeed->value();
	quazaaSettings.TransfersBandwidthUploads = m_ui->doubleSpinBoxUploadSpeed->value();
	quazaaSettings.SecurityEnableUPnP = m_ui->checkBoxUPnP->isChecked();
	quazaaSettings.SecurityAllowSharesBrowse = m_ui->checkBoxAllowBrowseShares->isChecked();
	quazaaSettings.LibraryShares.clear();
	for (int m_iSharesRow = 0; m_iSharesRow < m_ui->listWidgetShares->count(); ++m_iSharesRow)
	{
		m_ui->listWidgetShares->setCurrentRow(m_iSharesRow);
		quazaaSettings.LibraryShares.append(m_ui->listWidgetShares->currentItem()->text());
	}
	quazaaSettings.SecurityAllowProfileBrowse = m_ui->checkBoxAllowBrowseProfile->isChecked();
	quazaaSettings.ProfileIrcNickname = m_ui->lineEditIrcNick->text();
	quazaaSettings.ProfileIrcAlternateNickname = m_ui->lineEditIrcAlternateNick->text();
	quazaaSettings.ProfileGnutellaScreenName = m_ui->lineEditGnutellaScreenName->text();
	quazaaSettings.ProfileGender = m_ui->comboBoxGender->currentIndex();
	quazaaSettings.ProfileAge = m_ui->spinBoxAge->value();
	quazaaSettings.ProfileCountry = m_ui->lineEditCountry->text();
	quazaaSettings.ProfileStateProvince = m_ui->lineEditStateProvince->text();
	quazaaSettings.ProfileCity = m_ui->lineEditCity->text();
	quazaaSettings.Gnutella2Enable = m_ui->checkBoxG2Connect->isChecked();
	quazaaSettings.Gnutella1Enable = m_ui->checkBoxG1Connect->isChecked();
	quazaaSettings.AresEnable = m_ui->checkBoxAresConnect->isChecked();
	quazaaSettings.EDonkeyEnable = m_ui->checkBoxED2kConnect->isChecked();
	quazaaSettings.EDonkeyEnableKad = m_ui->checkBoxKadConnect->isChecked();
	quazaaSettings.WebTorrent = m_ui->checkBoxBitTorrentLinks->isChecked();
	quazaaSettings.BitTorrentUseKademlia = m_ui->checkBoxBitTorrentKademliaConnect->isChecked();
	quazaaSettings.BasicConnectOnStartup = m_ui->checkBoxConnectNetworksAlways->isChecked();
	quazaaSettings.BasicStartWithSystem = m_ui->checkBoxOSStart->isChecked();
	quazaaSettings.TransfersSimpleProgressBar = m_ui->checkBoxSimpleProgress->isChecked();
	quazaaSettings.LibraryHighPriorityHashing = !m_ui->checkBoxSlowHashing->isChecked();
	quazaaSettings.saveProfile();
	quazaaSettings.saveSettings();
	emit closed();
	close();
}

void DialogWizard::on_pushButtonSystemExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonSharesAdd_clicked()
{
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QString directory = QFileDialog::getExistingDirectory(this,
							tr("Select A Folder To Share"),
							quazaaSettings.DownloadsCompletePath,
							options);
	if (!directory.isEmpty())
	{
		m_ui->listWidgetShares->addItem(directory);
	}
}

void DialogWizard::on_pushButtonSharesRemove_clicked()
{
	if(m_ui->listWidgetShares->currentRow() != -1)
	{
		m_ui->listWidgetShares->takeItem(m_ui->listWidgetShares->currentRow());
	}
}
