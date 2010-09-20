//
// dialogsettings.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
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

#include "dialogsettings.h"
#include "ui_dialogsettings.h"

#include "QSkinDialog/qskinsettings.h"

DialogSettings::DialogSettings(QWidget *parent, SettingsPage::settingsPage page) :
	QDialog(parent),
	ui(new Ui::DialogSettings)
{
	ui->setupUi(this);
	switchSettingsPage(page);
	skinChangeEvent();
}

DialogSettings::~DialogSettings()
{
	delete ui;
}

void DialogSettings::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogSettings::switchSettingsPage(SettingsPage::settingsPage page)
{
	switch (page)
	{
	case SettingsPage::System:
		ui->stackedWidgetSettings->setCurrentIndex(0);
		ui->listWidgetGeneralTask->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Skins:
		ui->stackedWidgetSettings->setCurrentIndex(1);
		ui->listWidgetGeneralTask->setCurrentRow(1, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Library:
		ui->stackedWidgetSettings->setCurrentIndex(2);
		ui->listWidgetGeneralTask->setCurrentRow(2, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::FileTypes:
		ui->stackedWidgetSettings->setCurrentIndex(3);
		ui->listWidgetGeneralTask->setCurrentRow(3, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::MediaPlayer:
		ui->stackedWidgetSettings->setCurrentIndex(4);
		ui->listWidgetGeneralTask->setCurrentRow(4, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Search:
		ui->stackedWidgetSettings->setCurrentIndex(5);
		ui->listWidgetGeneralTask->setCurrentRow(5, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Integration:
		ui->stackedWidgetSettings->setCurrentIndex(6);
		ui->listWidgetGeneralTask->setCurrentRow(6, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Plugins:
		ui->stackedWidgetSettings->setCurrentIndex(7);
		ui->listWidgetGeneralTask->setCurrentRow(7, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::PrivateMessages:
		ui->stackedWidgetSettings->setCurrentIndex(8);
		ui->listWidgetCommunityTask->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(true);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Chat:
		ui->stackedWidgetSettings->setCurrentIndex(9);
		ui->listWidgetCommunityTask->setCurrentRow(1, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(true);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Security:
		ui->stackedWidgetSettings->setCurrentIndex(10);
		ui->listWidgetSecurityTask->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(true);
		break;
	case SettingsPage::Parental:
		ui->stackedWidgetSettings->setCurrentIndex(11);
		ui->listWidgetSecurityTask->setCurrentRow(1, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(true);
		break;
	case SettingsPage::Connection:
		ui->stackedWidgetSettings->setCurrentIndex(12);
		ui->listWidgetNetworkTask->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(true);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Transfers:
		ui->stackedWidgetSettings->setCurrentIndex(13);
		ui->listWidgetNetworkTask->setCurrentRow(1, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(true);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Downloads:
		ui->stackedWidgetSettings->setCurrentIndex(14);
		ui->listWidgetNetworkTask->setCurrentRow(2, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(true);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Uploads:
		ui->stackedWidgetSettings->setCurrentIndex(15);
		ui->listWidgetNetworkTask->setCurrentRow(3, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(true);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Gnutella2:
		ui->stackedWidgetSettings->setCurrentIndex(16);
		ui->listWidgetProtocolsTask->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(true);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Ares:
		ui->stackedWidgetSettings->setCurrentIndex(17);
		ui->listWidgetProtocolsTask->setCurrentRow(1, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(true);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::EDonkey:
		ui->stackedWidgetSettings->setCurrentIndex(18);
		ui->listWidgetProtocolsTask->setCurrentRow(2, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(true);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::BitTorrent:
		ui->stackedWidgetSettings->setCurrentIndex(19);
		ui->listWidgetProtocolsTask->setCurrentRow(3, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(true);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	case SettingsPage::Protocols:
		ui->stackedWidgetSettings->setCurrentIndex(20);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(false);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(true);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	default:
		ui->stackedWidgetSettings->setCurrentIndex(0);
		ui->listWidgetGeneralTask->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
		ui->toolButtonCommunityTask->setChecked(false);
		ui->toolButtonGeneralTask->setChecked(true);
		ui->toolButtonNetworkTask->setChecked(false);
		ui->toolButtonProtocolsTask->setChecked(false);
		ui->toolButtonSecurityTask->setChecked(false);
		break;
	}
}

void DialogSettings::on_listWidgetGeneralTask_clicked(QModelIndex index)
{
	ui->stackedWidgetSettings->setCurrentIndex(index.row());
}

void DialogSettings::on_listWidgetCommunityTask_clicked(QModelIndex index)
{
	ui->stackedWidgetSettings->setCurrentIndex(index.row() + 8);
}

void DialogSettings::on_listWidgetSecurityTask_clicked(QModelIndex index)
{
	ui->stackedWidgetSettings->setCurrentIndex(index.row() + 10);
}

void DialogSettings::on_listWidgetNetworkTask_clicked(QModelIndex index)
{
	ui->stackedWidgetSettings->setCurrentIndex(index.row() + 12);
}

void DialogSettings::on_listWidgetProtocolsTask_clicked(QModelIndex index)
{
	ui->stackedWidgetSettings->setCurrentIndex(index.row() + 16);
}

void DialogSettings::on_pushButtonOk_clicked()
{
	on_pushButtonApply_clicked();
	close();
}

void DialogSettings::on_pushButtonCancel_clicked()
{
	close();
}

void DialogSettings::on_pushButtonApply_clicked()
{
	ui->pushButtonApply->setEnabled(false);
}

void DialogSettings::skinChangeEvent()
{
	ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
	ui->frameSidebarContents->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonCommunityTask->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonGeneralTask->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonNetworkTask->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonProtocolsTask->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonSecurityTask->setStyleSheet(skinSettings.sidebarTaskHeader);
}
