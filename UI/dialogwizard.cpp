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

DialogWizard::DialogWizard(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogWizard)
{
	ui->setupUi(this);
	ui->comboBoxGender->setView(new QListView());
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->doubleSpinBoxInSpeed->setValue((quazaaSettings.Connection.InSpeed / 1024) * 8);
	ui->doubleSpinBoxOutSpeed->setValue((quazaaSettings.Connection.OutSpeed / 1024) * 8);
	ui->spinBoxQuazaaPort->setValue(quazaaSettings.Connection.Port);
	ui->checkBoxRandomPort->setChecked(quazaaSettings.Connection.RandomPort);
	ui->checkBoxUPnP->setChecked(quazaaSettings.Security.EnableUPnP);
	ui->checkBoxAllowBrowseShares->setChecked(quazaaSettings.Security.AllowSharesBrowse);
	ui->listWidgetShares->addItems(quazaaSettings.Library.Shares);
	ui->checkBoxAllowBrowseProfile->setChecked(quazaaSettings.Security.AllowProfileBrowse);
	ui->lineEditIrcNick->setText(quazaaSettings.Profile.IrcNickname);
	ui->lineEditIrcAlternateNick->setText(quazaaSettings.Profile.IrcAlternateNickname);
	ui->lineEditGnutellaScreenName->setText(quazaaSettings.Profile.GnutellaScreenName);
	ui->comboBoxGender->setCurrentIndex(quazaaSettings.Profile.Gender);
	ui->spinBoxAge->setValue(quazaaSettings.Profile.Age);
	ui->lineEditCountry->setText(quazaaSettings.Profile.Country);
	ui->lineEditStateProvince->setText(quazaaSettings.Profile.StateProvince);
	ui->lineEditCity->setText(quazaaSettings.Profile.City);
	ui->checkBoxG2Connect->setChecked(quazaaSettings.Gnutella2.Enable);
	ui->checkBoxAresConnect->setChecked(quazaaSettings.Ares.Enable);
	ui->checkBoxED2kConnect->setChecked(quazaaSettings.EDonkey.Enable);
	ui->checkBoxKadConnect->setChecked(quazaaSettings.EDonkey.EnableKad);
	ui->checkBoxBitTorrentLinks->setChecked(quazaaSettings.Web.Torrent);
	ui->checkBoxBitTorrentKademliaConnect->setChecked(quazaaSettings.BitTorrent.UseKademlia);
	ui->checkBoxConnectNetworksAlways->setChecked(quazaaSettings.System.ConnectOnStartup);
	ui->checkBoxOSStart->setChecked(quazaaSettings.System.StartWithSystem);
	ui->checkBoxSimpleProgress->setChecked(quazaaSettings.Transfers.SimpleProgressBar);
	ui->checkBoxSlowHashing->setChecked(!quazaaSettings.Library.HighPriorityHashing);
	ui->pagesWizard->setCurrentIndex(0);
}

DialogWizard::~DialogWizard()
{
	delete ui;
}

void DialogWizard::changeEvent(QEvent* e)
{
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogWizard::on_pushButtonConnectionBack_clicked()
{
	ui->pagesWizard->setCurrentIndex(0);
}

void DialogWizard::on_pushButtonConnectionNext_clicked()
{
	ui->pagesWizard->setCurrentIndex(2);
}

void DialogWizard::on_pushButtonConnectionExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonWelcomeNext_clicked()
{
	ui->pagesWizard->setCurrentIndex(1);
}

void DialogWizard::on_pushButtonWelcomeExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonSharesBack_clicked()
{
	ui->pagesWizard->setCurrentIndex(1);
}

void DialogWizard::on_pushButtonSharesNext_clicked()
{
	ui->pagesWizard->setCurrentIndex(3);
}

void DialogWizard::on_pushButtonSharesExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonChatBack_clicked()
{
	ui->pagesWizard->setCurrentIndex(2);
}

void DialogWizard::on_pushButtonChatNext_clicked()
{
	ui->pagesWizard->setCurrentIndex(4);
}

void DialogWizard::on_pushButtonChatExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonNetworksBack_clicked()
{
	ui->pagesWizard->setCurrentIndex(3);
}

void DialogWizard::on_pushButtonNetworksNext_clicked()
{
	ui->pagesWizard->setCurrentIndex(5);
}

void DialogWizard::on_pushButtonNetworksExit_clicked()
{
	emit closed();
	close();
}

void DialogWizard::on_pushButtonSystemBack_clicked()
{
	ui->pagesWizard->setCurrentIndex(4);
}

void DialogWizard::on_pushButtonSystemFinish_clicked()
{
	quazaaSettings.Connection.InSpeed = (ui->doubleSpinBoxInSpeed->value() / 8) * 1024;
	quazaaSettings.Connection.OutSpeed = (ui->doubleSpinBoxOutSpeed->value() / 8) * 1024;
	quazaaSettings.Connection.Port = ui->spinBoxQuazaaPort->value();
	quazaaSettings.Connection.RandomPort = ui->checkBoxRandomPort->isChecked();
	quazaaSettings.Security.EnableUPnP = ui->checkBoxUPnP->isChecked();
	quazaaSettings.Security.AllowSharesBrowse = ui->checkBoxAllowBrowseShares->isChecked();
	quazaaSettings.Library.Shares.clear();
	for(int m_iSharesRow = 0; m_iSharesRow < ui->listWidgetShares->count(); ++m_iSharesRow)
	{
		ui->listWidgetShares->setCurrentRow(m_iSharesRow);
		quazaaSettings.Library.Shares.append(ui->listWidgetShares->currentItem()->text());
	}
	quazaaSettings.Security.AllowProfileBrowse = ui->checkBoxAllowBrowseProfile->isChecked();
	quazaaSettings.Profile.IrcNickname = ui->lineEditIrcNick->text();
	quazaaSettings.Profile.IrcAlternateNickname = ui->lineEditIrcAlternateNick->text();
	quazaaSettings.Profile.GnutellaScreenName = ui->lineEditGnutellaScreenName->text();
	quazaaSettings.Profile.Gender = ui->comboBoxGender->currentIndex();
	quazaaSettings.Profile.Age = ui->spinBoxAge->value();
	quazaaSettings.Profile.Country = ui->lineEditCountry->text();
	quazaaSettings.Profile.StateProvince = ui->lineEditStateProvince->text();
	quazaaSettings.Profile.City = ui->lineEditCity->text();
	quazaaSettings.Gnutella2.Enable = ui->checkBoxG2Connect->isChecked();
	quazaaSettings.Ares.Enable = ui->checkBoxAresConnect->isChecked();
	quazaaSettings.EDonkey.Enable = ui->checkBoxED2kConnect->isChecked();
	quazaaSettings.EDonkey.EnableKad = ui->checkBoxKadConnect->isChecked();
	quazaaSettings.Web.Torrent = ui->checkBoxBitTorrentLinks->isChecked();
	quazaaSettings.BitTorrent.UseKademlia = ui->checkBoxBitTorrentKademliaConnect->isChecked();
	quazaaSettings.System.ConnectOnStartup = ui->checkBoxConnectNetworksAlways->isChecked();
	quazaaSettings.System.StartWithSystem = ui->checkBoxOSStart->isChecked();
	quazaaSettings.Transfers.SimpleProgressBar = ui->checkBoxSimpleProgress->isChecked();
	quazaaSettings.Library.HighPriorityHashing = !ui->checkBoxSlowHashing->isChecked();
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
	if(!directory.isEmpty())
	{
		ui->listWidgetShares->addItem(directory);
	}
}

void DialogWizard::on_pushButtonSharesRemove_clicked()
{
	if(ui->listWidgetShares->currentRow() != -1)
	{
		ui->listWidgetShares->takeItem(ui->listWidgetShares->currentRow());
	}
}


void DialogWizard::skinChangeEvent()
{
	ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
	ui->listWidgetShares->setStyleSheet(skinSettings.listViews);
}
