/*
** wizardquickstart.cpp
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "wizardquickstart.h"
#include "ui_wizardquickstart.h"
#include "quazaasettings.h"
#include <QFileDialog>

WizardQuickStart::WizardQuickStart(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardQuickStart)
{
	ui->setupUi(this);
	this->setPixmap(QWizard::LogoPixmap, QPixmap(":/Resource/Quazaa48.png"));
	ui->comboBoxGender->setView(new QListView());
	ui->doubleSpinBoxInSpeed->setValue((quazaaSettings.Connection.InSpeed / 1024) * 8);
	ui->doubleSpinBoxOutSpeed->setValue((quazaaSettings.Connection.OutSpeed / 1024) * 8);
	ui->spinBoxPort->setValue(quazaaSettings.Connection.Port);
	ui->checkBoxRandomPort->setChecked(quazaaSettings.Connection.RandomPort);
	ui->checkBoxUPnP->setChecked(quazaaSettings.Security.EnableUPnP);
	ui->checkBoxAllowBrowseShares->setChecked(quazaaSettings.Security.AllowSharesBrowse);
	ui->listWidgetShares->addItems(quazaaSettings.Library.Shares);
	ui->checkBoxAllowBrowseProfile->setChecked(quazaaSettings.Security.AllowProfileBrowse);
	ui->lineEditIRCNick->setText(quazaaSettings.Profile.IrcNickname);
	ui->lineEditIRCAlternateNick->setText(quazaaSettings.Profile.IrcAlternateNickname);
	ui->lineEditGnutellaScreenName->setText(quazaaSettings.Profile.GnutellaScreenName);
	ui->comboBoxGender->setCurrentIndex(quazaaSettings.Profile.Gender);
	ui->spinBoxAge->setValue(quazaaSettings.Profile.Age);
	ui->lineEditCountry->setText(quazaaSettings.Profile.Country);
	ui->lineEditStateProvince->setText(quazaaSettings.Profile.StateProvince);
	ui->lineEditCity->setText(quazaaSettings.Profile.City);
	ui->checkBoxG2Connect->setChecked(quazaaSettings.Gnutella2.Enable);
	ui->checkBoxAresConnect->setChecked(quazaaSettings.Ares.Enable);
	ui->checkBoxEDonkeyConnect->setChecked(quazaaSettings.EDonkey.Enable);
	ui->checkBoxKADConnect->setChecked(quazaaSettings.EDonkey.EnableKad);
	ui->checkBoxBitTorrent->setChecked(quazaaSettings.Web.Torrent);
	ui->checkBoxBitTorrentKademliaConnect->setChecked(quazaaSettings.BitTorrent.UseKademlia);
	ui->checkBoxConnectNetworksAlways->setChecked(quazaaSettings.System.ConnectOnStartup);
	ui->checkBoxOSStart->setChecked(quazaaSettings.System.StartWithSystem);
	ui->checkBoxSimpleProgress->setChecked(quazaaSettings.Transfers.SimpleProgressBar);
	ui->checkBoxSlowHashing->setChecked(!quazaaSettings.Library.HighPriorityHashing);
}

WizardQuickStart::~WizardQuickStart()
{
    delete ui;
}

void WizardQuickStart::changeEvent(QEvent* e)
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

void WizardQuickStart::accept()
{
	quazaaSettings.Connection.InSpeed = (ui->doubleSpinBoxInSpeed->value() / 8) * 1024;
	quazaaSettings.Connection.OutSpeed = (ui->doubleSpinBoxOutSpeed->value() / 8) * 1024;
	quazaaSettings.Connection.Port = ui->spinBoxPort->value();
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
	quazaaSettings.Profile.IrcNickname = ui->lineEditIRCNick->text();
	quazaaSettings.Profile.IrcAlternateNickname = ui->lineEditIRCAlternateNick->text();
	quazaaSettings.Profile.GnutellaScreenName = ui->lineEditGnutellaScreenName->text();
	quazaaSettings.Profile.Gender = ui->comboBoxGender->currentIndex();
	quazaaSettings.Profile.Age = ui->spinBoxAge->value();
	quazaaSettings.Profile.Country = ui->lineEditCountry->text();
	quazaaSettings.Profile.StateProvince = ui->lineEditStateProvince->text();
	quazaaSettings.Profile.City = ui->lineEditCity->text();
	quazaaSettings.Gnutella2.Enable = ui->checkBoxG2Connect->isChecked();
	quazaaSettings.Ares.Enable = ui->checkBoxAresConnect->isChecked();
	quazaaSettings.EDonkey.Enable = ui->checkBoxEDonkeyConnect->isChecked();
	quazaaSettings.EDonkey.EnableKad = ui->checkBoxKADConnect->isChecked();
	quazaaSettings.Web.Torrent = ui->checkBoxBitTorrent->isChecked();
	quazaaSettings.BitTorrent.UseKademlia = ui->checkBoxBitTorrentKademliaConnect->isChecked();
	quazaaSettings.System.ConnectOnStartup = ui->checkBoxConnectNetworksAlways->isChecked();
	quazaaSettings.System.StartWithSystem = ui->checkBoxOSStart->isChecked();
	quazaaSettings.Transfers.SimpleProgressBar = ui->checkBoxSimpleProgress->isChecked();
	quazaaSettings.Library.HighPriorityHashing = !ui->checkBoxSlowHashing->isChecked();
	quazaaSettings.saveProfile();
	quazaaSettings.saveSettings();

	QDialog::accept();
}

void WizardQuickStart::on_toolButtonSharesAdd_clicked()
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

void WizardQuickStart::on_toolButtonSharesRemove_clicked()
{
    if(ui->listWidgetShares->currentRow() != -1)
    {
            ui->listWidgetShares->takeItem(ui->listWidgetShares->currentRow());
    }
}
