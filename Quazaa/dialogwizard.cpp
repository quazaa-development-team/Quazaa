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
#include "persistentsettings.h"
#include "quazaaglobals.h"
#include <QFileDialog>
#include <QSettings>

DialogWizard::DialogWizard(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DialogWizard)
{
    m_ui->setupUi(this);
	m_ui->doubleSpinBoxDownloadSpeed->setValue(Settings.Transfers.BandwidthDownloads);
	m_ui->doubleSpinBoxUploadSpeed->setValue(Settings.Transfers.BandwidthUploads);
	m_ui->checkBoxUPnP->setChecked(Settings.Security.EnableUPnP);
	m_ui->checkBoxAllowBrowseShares->setChecked(Settings.Security.AllowSharesBrowse);
	m_ui->listWidgetShares->addItems(Settings.Library.Shares);
	m_ui->checkBoxAllowBrowseProfile->setChecked(Settings.Security.AllowProfileBrowse);
	m_ui->lineEditIrcNick->setText(Settings.Profile.IrcNickname);
	m_ui->lineEditIrcAlternateNick->setText(Settings.Profile.IrcAlternateNickname);
	m_ui->lineEditGnutellaScreenName->setText(Settings.Profile.GnutellaScreenName);
	m_ui->comboBoxGender->setCurrentIndex(Settings.Profile.Gender);
	m_ui->spinBoxAge->setValue(Settings.Profile.Age);
	m_ui->lineEditCountry->setText(Settings.Profile.Country);
	m_ui->lineEditStateProvince->setText(Settings.Profile.StateProvince);
	m_ui->lineEditCity->setText(Settings.Profile.City);
	m_ui->checkBoxG2Connect->setChecked(Settings.Gnutella2.Enable);
	m_ui->checkBoxG1Connect->setChecked(Settings.Gnutella1.Enable);
	m_ui->checkBoxAresConnect->setChecked(Settings.Ares.Enable);
	m_ui->checkBoxED2kConnect->setChecked(Settings.eDonkey2k.Enable);
	m_ui->checkBoxKadConnect->setChecked(Settings.eDonkey2k.EnableKad);
	m_ui->checkBoxBittorrentLinks->setChecked(Settings.Web.Torrent);
	m_ui->checkBoxBittorrentKademliaConnect->setChecked(Settings.Bittorrent.UseKademlia);
	m_ui->checkBoxConnectNetworksAlways->setChecked(Settings.Basic.ConnectOnStartup);
	m_ui->checkBoxOSStart->setChecked(Settings.Basic.StartWithSystem);
	m_ui->checkBoxSimpleProgress->setChecked(Settings.Transfers.SimpleProgressBar);
	m_ui->checkBoxSlowHashing->setChecked(!Settings.Library.HighPriorityHashing);
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
	close();
}

void DialogWizard::on_pushButtonWelcomeNext_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(1);
}

void DialogWizard::on_pushButtonWelcomeExit_clicked()
{
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
	close();
}

void DialogWizard::on_pushButtonSystemBack_clicked()
{
	m_ui->pagesWizard->setCurrentIndex(4);
}

void DialogWizard::on_pushButtonSystemFinish_clicked()
{
	Settings.Transfers.BandwidthDownloads = m_ui->doubleSpinBoxDownloadSpeed->value();
	Settings.Transfers.BandwidthUploads = m_ui->doubleSpinBoxUploadSpeed->value();
	Settings.Security.EnableUPnP = m_ui->checkBoxUPnP->isChecked();
	Settings.Security.AllowSharesBrowse = m_ui->checkBoxAllowBrowseShares->isChecked();
	Settings.Library.Shares.clear();
	for (int m_iSharesRow = 0; m_iSharesRow < m_ui->listWidgetShares->count(); ++m_iSharesRow)
	{
		m_ui->listWidgetShares->setCurrentRow(m_iSharesRow);
		Settings.Library.Shares.append(m_ui->listWidgetShares->currentItem()->text());
	}
	Settings.Security.AllowProfileBrowse = m_ui->checkBoxAllowBrowseProfile->isChecked();
	Settings.Profile.IrcNickname = m_ui->lineEditIrcNick->text();
	Settings.Profile.IrcAlternateNickname = m_ui->lineEditIrcAlternateNick->text();
	Settings.Profile.GnutellaScreenName = m_ui->lineEditGnutellaScreenName->text();
	Settings.Profile.Gender = m_ui->comboBoxGender->currentIndex();
	Settings.Profile.Age = m_ui->spinBoxAge->value();
	Settings.Profile.Country = m_ui->lineEditCountry->text();
	Settings.Profile.StateProvince = m_ui->lineEditStateProvince->text();
	Settings.Profile.City = m_ui->lineEditCity->text();
	Settings.Gnutella2.Enable = m_ui->checkBoxG2Connect->isChecked();
	Settings.Gnutella1.Enable = m_ui->checkBoxG1Connect->isChecked();
	Settings.Ares.Enable = m_ui->checkBoxAresConnect->isChecked();
	Settings.eDonkey2k.Enable = m_ui->checkBoxED2kConnect->isChecked();
	Settings.eDonkey2k.EnableKad = m_ui->checkBoxKadConnect->isChecked();
	Settings.Web.Torrent = m_ui->checkBoxBittorrentLinks->isChecked();
	Settings.Bittorrent.UseKademlia = m_ui->checkBoxBittorrentKademliaConnect->isChecked();
	Settings.Basic.ConnectOnStartup = m_ui->checkBoxConnectNetworksAlways->isChecked();
	Settings.Basic.FirstRun = false;
	Settings.Basic.StartWithSystem = m_ui->checkBoxOSStart->isChecked();
	Settings.Transfers.SimpleProgressBar = m_ui->checkBoxSimpleProgress->isChecked();
	Settings.Library.HighPriorityHashing = !m_ui->checkBoxSlowHashing->isChecked();
	Settings.saveWizard();
	close();
}

void DialogWizard::on_pushButtonSystemExit_clicked()
{
	close();
}

void DialogWizard::on_pushButtonSharesAdd_clicked()
{
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QString directory = QFileDialog::getExistingDirectory(this,
							tr("Select A Folder To Share"),
							Settings.Downloads.CompletePath,
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
