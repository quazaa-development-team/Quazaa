//
// dialogwizard.cpp
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

#include "dialogwizard.h"
#include "ui_dialogwizard.h"
#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"
#include <QFileDialog>

DialogWizard::DialogWizard(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogWizard)
{
	m_ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	m_ui->doubleSpinBoxDownloadSpeed->setValue(quazaaSettings.Transfers.BandwidthDownloads);
	m_ui->doubleSpinBoxUploadSpeed->setValue(quazaaSettings.Transfers.BandwidthUploads);
	m_ui->checkBoxUPnP->setChecked(quazaaSettings.Security.EnableUPnP);
	m_ui->checkBoxAllowBrowseShares->setChecked(quazaaSettings.Security.AllowSharesBrowse);
	m_ui->listWidgetShares->addItems(quazaaSettings.Library.Shares);
	m_ui->checkBoxAllowBrowseProfile->setChecked(quazaaSettings.Security.AllowProfileBrowse);
	m_ui->lineEditIrcNick->setText(quazaaSettings.Profile.IrcNickname);
	m_ui->lineEditIrcAlternateNick->setText(quazaaSettings.Profile.IrcAlternateNickname);
	m_ui->lineEditGnutellaScreenName->setText(quazaaSettings.Profile.GnutellaScreenName);
	m_ui->comboBoxGender->setCurrentIndex(quazaaSettings.Profile.Gender);
	m_ui->spinBoxAge->setValue(quazaaSettings.Profile.Age);
	m_ui->lineEditCountry->setText(quazaaSettings.Profile.Country);
	m_ui->lineEditStateProvince->setText(quazaaSettings.Profile.StateProvince);
	m_ui->lineEditCity->setText(quazaaSettings.Profile.City);
	m_ui->checkBoxG2Connect->setChecked(quazaaSettings.Gnutella2.Enable);
	m_ui->checkBoxAresConnect->setChecked(quazaaSettings.Ares.Enable);
	m_ui->checkBoxED2kConnect->setChecked(quazaaSettings.EDonkey.Enable);
	m_ui->checkBoxKadConnect->setChecked(quazaaSettings.EDonkey.EnableKad);
	m_ui->checkBoxBitTorrentLinks->setChecked(quazaaSettings.Web.Torrent);
	m_ui->checkBoxBitTorrentKademliaConnect->setChecked(quazaaSettings.BitTorrent.UseKademlia);
	m_ui->checkBoxConnectNetworksAlways->setChecked(quazaaSettings.Basic.ConnectOnStartup);
	m_ui->checkBoxOSStart->setChecked(quazaaSettings.Basic.StartWithSystem);
	m_ui->checkBoxSimpleProgress->setChecked(quazaaSettings.Transfers.SimpleProgressBar);
	m_ui->checkBoxSlowHashing->setChecked(!quazaaSettings.Library.HighPriorityHashing);
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
	quazaaSettings.Transfers.BandwidthDownloads = m_ui->doubleSpinBoxDownloadSpeed->value();
	quazaaSettings.Transfers.BandwidthUploads = m_ui->doubleSpinBoxUploadSpeed->value();
	quazaaSettings.Security.EnableUPnP = m_ui->checkBoxUPnP->isChecked();
	quazaaSettings.Security.AllowSharesBrowse = m_ui->checkBoxAllowBrowseShares->isChecked();
	quazaaSettings.Library.Shares.clear();
	for (int m_iSharesRow = 0; m_iSharesRow < m_ui->listWidgetShares->count(); ++m_iSharesRow)
	{
		m_ui->listWidgetShares->setCurrentRow(m_iSharesRow);
		quazaaSettings.Library.Shares.append(m_ui->listWidgetShares->currentItem()->text());
	}
	quazaaSettings.Security.AllowProfileBrowse = m_ui->checkBoxAllowBrowseProfile->isChecked();
	quazaaSettings.Profile.IrcNickname = m_ui->lineEditIrcNick->text();
	quazaaSettings.Profile.IrcAlternateNickname = m_ui->lineEditIrcAlternateNick->text();
	quazaaSettings.Profile.GnutellaScreenName = m_ui->lineEditGnutellaScreenName->text();
	quazaaSettings.Profile.Gender = m_ui->comboBoxGender->currentIndex();
	quazaaSettings.Profile.Age = m_ui->spinBoxAge->value();
	quazaaSettings.Profile.Country = m_ui->lineEditCountry->text();
	quazaaSettings.Profile.StateProvince = m_ui->lineEditStateProvince->text();
	quazaaSettings.Profile.City = m_ui->lineEditCity->text();
	quazaaSettings.Gnutella2.Enable = m_ui->checkBoxG2Connect->isChecked();
	quazaaSettings.Ares.Enable = m_ui->checkBoxAresConnect->isChecked();
	quazaaSettings.EDonkey.Enable = m_ui->checkBoxED2kConnect->isChecked();
	quazaaSettings.EDonkey.EnableKad = m_ui->checkBoxKadConnect->isChecked();
	quazaaSettings.Web.Torrent = m_ui->checkBoxBitTorrentLinks->isChecked();
	quazaaSettings.BitTorrent.UseKademlia = m_ui->checkBoxBitTorrentKademliaConnect->isChecked();
	quazaaSettings.Basic.ConnectOnStartup = m_ui->checkBoxConnectNetworksAlways->isChecked();
	quazaaSettings.Basic.StartWithSystem = m_ui->checkBoxOSStart->isChecked();
	quazaaSettings.Transfers.SimpleProgressBar = m_ui->checkBoxSimpleProgress->isChecked();
	quazaaSettings.Library.HighPriorityHashing = !m_ui->checkBoxSlowHashing->isChecked();
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
							quazaaSettings.Downloads.CompletePath,
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


void DialogWizard::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
