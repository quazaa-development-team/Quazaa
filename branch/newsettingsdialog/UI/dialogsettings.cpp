//
// dialogsettings.cpp
//
// Copyright  Quazaa Development Team, 2009-2010.
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

#include "dialogsettings.h"
#include "ui_dialogsettings.h"
#include "dialogprofile.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include "QSkinDialog/dialogskinpreview.h"
#include "QSkinDialog/qskinsettings.h"
#include "QSkinDialog/qskindialog.h"

DialogSettings::DialogSettings(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogSettings)
{
	m_ui->setupUi(this);

	newSkinSelected = false;
	// Load Basic Settings
	m_ui->checkBoxSystemStart->setChecked(quazaaSettings.Basic.StartWithSystem);
	m_ui->checkBoxAutoConnect->setChecked(quazaaSettings.Basic.ConnectOnStartup);
	m_ui->comboBoxCloseButton->setCurrentIndex(quazaaSettings.Basic.CloseMode);
	m_ui->checkBoxMinimiseToTray->setChecked(quazaaSettings.Basic.MinimizeToTray);
	m_ui->checkBoxSearchTooltips->setChecked(quazaaSettings.Basic.SearchTips);
	m_ui->checkBoxLibraryTooltips->setChecked(quazaaSettings.Basic.LibraryTips);
	m_ui->checkBoxDownloadTooltips->setChecked(quazaaSettings.Basic.DownloadsTips);
	m_ui->checkBoxUploadTooltips->setChecked(quazaaSettings.Basic.UploadsTips);
	m_ui->checkBoxNeighbourTooltips->setChecked(quazaaSettings.Basic.NeighboursTips);
	m_ui->checkBoxMediaPlayerTooltips->setChecked(quazaaSettings.Basic.MediaTips);
	m_ui->checkBoxChatTooltips->setChecked(quazaaSettings.Basic.ChatTips);
	m_ui->spinBoxToolTipDelay->setValue(quazaaSettings.Basic.TipDelay);
	m_ui->horizontalSliderTransparency->setValue(quazaaSettings.Basic.TipTransparency);
	m_ui->spinBoxDiskSpaceWarning->setValue(quazaaSettings.Basic.DiskSpaceWarning);
	m_ui->spinBoxDiskSpaceStop->setValue(quazaaSettings.Basic.DiskSpaceStop);

	// Load Parental Settings
	m_ui->checkBoxCensor->setChecked(quazaaSettings.Parental.ChatAdultCensor);
	m_ui->checkBoxFilterAdult->setChecked(quazaaSettings.Parental.FilterAdultSearchResults);
	m_ui->listWidgetAdultFilter->addItems(quazaaSettings.Parental.AdultFilter);

	// Load Library Settings
	m_ui->checkBoxRememberViews->setChecked(quazaaSettings.Library.RememberViews);
	m_ui->checkBoxWatchFolders->setChecked(quazaaSettings.Library.WatchFolders);
	m_ui->checkBoxQuickHashing->setChecked(quazaaSettings.Library.HighPriorityHashing);
	m_ui->checkBoxDisplayHashingProgress->setChecked(quazaaSettings.Library.HashWindow);
	m_ui->checkBoxGhostFiles->setChecked(quazaaSettings.Library.GhostFiles);
	m_ui->checkBoxSeriesDetection->setChecked(quazaaSettings.Library.SmartSeriesDetection);
	m_ui->spinBoxRememberFiles->setValue(quazaaSettings.Library.HistoryTotal);
	m_ui->spinBoxUpToDays->setValue(quazaaSettings.Library.HistoryDays);
	m_ui->listWidgetSafeOpen->addItems(quazaaSettings.Library.SafeExecuteTypes);
	m_ui->listWidgetNeverShare->addItems(quazaaSettings.Library.NeverShareTypes);

	// Load Media Player Settings
	m_ui->comboBoxChoosePlayer->setCurrentIndex(quazaaSettings.Media.Handler);
	m_ui->lineEditCustomPlayerPath->setText(quazaaSettings.Media.CustomPlayerPath);
	m_ui->listWidgetPlayerFilesList->addItems(quazaaSettings.Media.FileTypes);

	// Load Search Settings
	m_ui->checkBoxExpandMultiSorce->setChecked(quazaaSettings.Search.ExpandSearchMatches);
	m_ui->checkBoxSwitchOnDownload->setChecked(quazaaSettings.Search.SwitchOnDownload);
	m_ui->checkBoxHighlightNewSearchMatches->setChecked(quazaaSettings.Search.HighlightNew);

	// Load Chat Settings
	m_ui->checkBoxGnutellaChatEnable->setChecked(quazaaSettings.Chat.GnutellaChatEnable);
	m_ui->checkBoxEnableChatOther->setChecked(quazaaSettings.Chat.EnableChatAllNetworks);
	m_ui->checkBoxConnectIRCOnStart->setChecked(quazaaSettings.Chat.ConnectOnStartup);
	m_ui->checkBoxEnableIRCFileTransfers->setChecked(quazaaSettings.Chat.EnableFileTransfers);
	m_ui->checkBoxIRCShowTimestamp->setChecked(quazaaSettings.Chat.ShowTimestamp);
	m_ui->checkBoxIRCSSL->setChecked(quazaaSettings.Chat.IrcUseSSL);
	m_ui->spinBoxChatIdleMessage->setValue(quazaaSettings.Chat.AwayMessageIdleTime);
	m_ui->lineEditIRCServer->setText(quazaaSettings.Chat.IrcServerName);
	m_ui->spinBoxIRCPort->setValue(quazaaSettings.Chat.IrcServerPort);
	m_ui->labelChatFontPreview->setText(quazaaSettings.Chat.ScreenFont.rawName());
	m_ui->labelChatFontPreview->setFont(quazaaSettings.Chat.ScreenFont);
	fontTempChat = quazaaSettings.Chat.ScreenFont;
	m_ui->toolButtonColorChatBackground->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.Chat.ColorChatBackground.name() + ";}");
	m_ui->frameIRCColors->setStyleSheet("background-color: " + quazaaSettings.Chat.ColorChatBackground.name() + ";");
	colorTempChatBackground.setNamedColor(quazaaSettings.Chat.ColorChatBackground.name());
	m_ui->toolButtonColorNormalText->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.Chat.ColorNormalText.name() + ";}");
	m_ui->labelNormalText->setFont(quazaaSettings.Chat.ScreenFont);
	m_ui->labelNormalText->setStyleSheet("color: " + quazaaSettings.Chat.ColorNormalText.name() + ";");
	colorTempNormalText.setNamedColor(quazaaSettings.Chat.ColorNormalText.name());
	m_ui->toolButtonColorServerMessages->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.Chat.ColorServerMessagesText.name() + ";}");
	m_ui->labelServerMessages->setFont(quazaaSettings.Chat.ScreenFont);
	m_ui->labelServerMessages->setStyleSheet("color: " + quazaaSettings.Chat.ColorServerMessagesText.name() + ";");
	colorTempServerMessages.setNamedColor(quazaaSettings.Chat.ColorServerMessagesText.name());
	m_ui->toolButtonColorTopics->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.Chat.ColorTopicsText.name() + ";}");
	m_ui->labelTopics->setFont(quazaaSettings.Chat.ScreenFont);
	m_ui->labelTopics->setStyleSheet("color: " + quazaaSettings.Chat.ColorTopicsText.name() + ";");
	colorTempTopics.setNamedColor(quazaaSettings.Chat.ColorTopicsText.name());
	m_ui->toolButtonColorRoomActions->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.Chat.ColorRoomActionsText.name() + ";}");
	m_ui->labelRoomActions->setFont(quazaaSettings.Chat.ScreenFont);
	m_ui->labelRoomActions->setStyleSheet("color: " + quazaaSettings.Chat.ColorRoomActionsText.name() + ";");
	colorTempRoomActions.setNamedColor(quazaaSettings.Chat.ColorRoomActionsText.name());
	m_ui->toolButtonColorNotices->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.Chat.ColorNoticesText.name() + ";}");
	m_ui->labelNotices->setFont(quazaaSettings.Chat.ScreenFont);
	m_ui->labelNotices->setStyleSheet("color: " + quazaaSettings.Chat.ColorNoticesText.name() + ";");
	colorTempNotices.setNamedColor(quazaaSettings.Chat.ColorNoticesText.name());

	// Load Connection Settings
	m_ui->doubleSpinBoxInSpeed->setValue((quazaaSettings.Connection.InSpeed/1024)*8);
	m_ui->doubleSpinBoxOutSpeed->setValue((quazaaSettings.Connection.OutSpeed/1024)*8);
	m_ui->spinBoxQuazaaPort->setValue(quazaaSettings.Connection.Port);
	m_ui->checkBoxRandomPort->setChecked(quazaaSettings.Connection.RandomPort);
	m_ui->spinBoxConnectionTimeout->setValue(quazaaSettings.Connection.TimeoutConnect);

	// Load Web Settings
	m_ui->checkBoxMagnetLinks->setChecked(quazaaSettings.Web.Magnet);
	m_ui->checkBoxGnutellaLinks->setChecked(quazaaSettings.Web.Gnutella);
	m_ui->checkBoxAresLinks->setChecked(quazaaSettings.Web.Ares);
	m_ui->checkBoxBitTorrentLinks->setChecked(quazaaSettings.Web.Torrent);
	m_ui->checkBoxPioletLinks->setChecked(quazaaSettings.Web.Piolet);
	m_ui->checkBoxEDonkeyLinks->setChecked(quazaaSettings.Web.ED2K);
	m_ui->checkBoxManageWebDownloads->setChecked(quazaaSettings.Web.BrowserIntegration);
	m_ui->listWidgetManageDownloadTypes->addItems(quazaaSettings.Web.ManageDownloadTypes);

	// Load Transfer Settings
	m_ui->checkBoxOnlyDownloadConnectedNetworks->setChecked(quazaaSettings.Transfers.RequireConnectedNetwork);
	m_ui->checkBoxSimpleProgress->setChecked(quazaaSettings.Transfers.SimpleProgressBar);
	m_ui->comboBoxRates->setCurrentIndex(quazaaSettings.Transfers.RatesUnit);

	// Load Download Settings
	m_ui->checkBoxExpandDownloads->setChecked(quazaaSettings.Downloads.ExpandDownloads);
	m_ui->lineEditSaveFolder->setText(quazaaSettings.Downloads.CompletePath);
	m_ui->lineEditTempFolder->setText(quazaaSettings.Downloads.IncompletePath);
	m_ui->comboBoxQueLength->setCurrentIndex(quazaaSettings.Downloads.QueueLimit);
	m_ui->spinBoxMaxFiles->setValue(quazaaSettings.Downloads.MaxFiles);
	m_ui->spinBoxMaxTransfers->setValue(quazaaSettings.Downloads.MaxTransfers);
	m_ui->spinBoxTransfersPerFile->setValue(quazaaSettings.Downloads.MaxTransfersPerFile);

	// Load Upload Settings
	m_ui->checkBoxSharePartials->setChecked(quazaaSettings.Uploads.SharePartials);
	m_ui->checkBoxSharingLimitHub->setChecked(quazaaSettings.Uploads.HubShareLimiting);
	m_ui->checkBoxSharePreviews->setChecked(quazaaSettings.Uploads.SharePreviews);
	m_ui->spinBoxUniqueHostLimit->setValue(quazaaSettings.Uploads.MaxPerHost);

	// Load Security Settings
	m_ui->checkBoxFilterChatSpam->setChecked(quazaaSettings.Security.ChatFilter);
	m_ui->checkBoxFilterEDonkeyChatSpam->setChecked(quazaaSettings.Security.ED2kChatFilter);
	m_ui->checkBoxAllowBrowseProfile->setChecked(quazaaSettings.Security.AllowProfileBrowse);
	m_ui->checkBoxChatFloodProtection->setChecked(quazaaSettings.Security.IrcFloodProtection);
	m_ui->spinBoxChatFloodLimit->setValue(quazaaSettings.Security.IrcFloodLimit);
	m_ui->checkBoxEnableRemote->setChecked(quazaaSettings.Security.RemoteEnable);
	m_ui->lineEditRemoteUserName->setText(quazaaSettings.Security.RemoteUsername);
	m_ui->lineEditRemotePassword->setText(quazaaSettings.Security.RemotePassword);
	m_ui->checkBoxIgnoreLocalIP->setChecked(quazaaSettings.Security.SearchIgnoreLocalIP);
	m_ui->checkBoxEnableUPnP->setChecked(quazaaSettings.Security.EnableUPnP);
	m_ui->checkBoxAllowBrowseShares->setChecked(quazaaSettings.Security.AllowSharesBrowse);
	m_ui->listWidgetUserAgents->addItems(quazaaSettings.Security.BlockedAgentUploadFilter);

	// Load Gnutella 2 Settings
	m_ui->checkBoxConnectG2->setChecked(quazaaSettings.Gnutella2.Enable);
	m_ui->comboBoxG2Mode->setCurrentIndex(quazaaSettings.Gnutella2.ClientMode);
	m_ui->spinBoxG2LeafToHub->setValue(quazaaSettings.Gnutella2.NumHubs);
	m_ui->spinBoxG2HubToLeaf->setValue(quazaaSettings.Gnutella2.NumLeafs);
	m_ui->spinBoxG2HubToHub->setValue(quazaaSettings.Gnutella2.NumPeers);

	// Load Ares Settings
	m_ui->checkBoxConnectAres->setChecked(quazaaSettings.Ares.Enable);
	m_ui->checkBoxConnectAres->setChecked(quazaaSettings.Ares.Enable);

	// Load eDonkey 2k Settings
	m_ui->checkBoxConnectEDonkey->setChecked(quazaaSettings.EDonkey.Enable);
	m_ui->checkBoxConnectKAD->setChecked(quazaaSettings.EDonkey.EnableKad);
	m_ui->checkBoxED2kSearchCahedServers->setChecked(quazaaSettings.EDonkey.SearchCachedServers);
	m_ui->spinBoxED2kMaxResults->setValue(quazaaSettings.EDonkey.MaxResults);
	m_ui->checkBoxED2kUpdateServerList->setChecked(quazaaSettings.EDonkey.LearnNewServers);
	m_ui->spinBoxED2kMaxClients->setValue(quazaaSettings.EDonkey.MaxClients);
	m_ui->checkBoxAutoQueryServerList->setChecked(quazaaSettings.EDonkey.MetAutoQuery);
	m_ui->lineEditEDonkeyServerListUrl->setText(quazaaSettings.EDonkey.ServerListURL);

	// Load BitTorrent Settings
	m_ui->checkBoxTorrentSaveDialog->setChecked(quazaaSettings.BitTorrent.UseSaveDialog);
	m_ui->checkBoxTorrentsStartPaused->setChecked(quazaaSettings.BitTorrent.StartPaused);
	m_ui->checkBoxTorrentsUseTemp->setChecked(quazaaSettings.BitTorrent.UseTemp);
	m_ui->checkBoxManagedTorrent->setChecked(quazaaSettings.BitTorrent.Managed);
	m_ui->checkBoxTorrentsEndgame->setChecked(quazaaSettings.BitTorrent.Endgame);
	m_ui->spinBoxTorrentsSimultaneous->setValue(quazaaSettings.BitTorrent.DownloadTorrents);
	m_ui->spinBoxTorrentsClientConnections->setValue(quazaaSettings.BitTorrent.DownloadConnections);
	m_ui->checkBoxTorrentsClearDownloaded->setChecked(quazaaSettings.BitTorrent.AutoClear);
	m_ui->spinBoxTorrentsRatioClear->setValue(quazaaSettings.BitTorrent.ClearRatio);
	m_ui->checkBoxTorrentsPreferTorrent->setChecked(quazaaSettings.BitTorrent.PreferBTSources);
	m_ui->checkBoxTorrentsUseKademlia->setChecked(quazaaSettings.BitTorrent.UseKademlia);
	m_ui->lineEditTorrentFolder->setText(quazaaSettings.BitTorrent.TorrentPath);

	// Set Generic Control States
	m_ui->pagesSettings->setCurrentIndex(0);
	m_ui->toolButtonNavigationGeneral->setChecked(true);
	m_ui->toolButtonNavigationInternet->setChecked(false);
	m_ui->toolButtonNavigationNetworks->setChecked(false);
	m_ui->groupBoxAdultFilter->setVisible(false);
	m_ui->pushButtonApply->setEnabled(false);

	// Load Skin Settings
	QDir dir(qApp->applicationDirPath() + "/Skin/");
	QFileInfoList skinDirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
	QListWidgetItem *itemToSet = new QListWidgetItem();
	foreach(QFileInfo i, skinDirs)
	{
		QListWidgetItem *item = new QListWidgetItem(i.fileName());
		m_ui->listWidgetSkins->addItem(item);
		if (item->text() == skinSettings.skinName)
		{
			itemToSet = item;
		}
	}
	m_ui->listWidgetSkins->setCurrentItem(itemToSet);

	connect(this, SIGNAL(skinChanged()), &skinSettings, SIGNAL(skinChanged()));
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogSettings::~DialogSettings()
{
	delete m_ui;
}

void DialogSettings::changeEvent(QEvent *e)
{
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogSettings::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogSettings::on_pushButtonOK_clicked()
{
	if(m_ui->pushButtonApply->isEnabled())
	{
		m_ui->pushButtonApply->click();
	}
	emit closed();
	close();
}

void DialogSettings::switchSettingsPage(int pageIndex)
{
	m_ui->pagesSettings->setCurrentIndex(pageIndex);
	switch (pageIndex)
	{
	case 20:
		m_ui->toolButtonNavigationGeneral->setChecked(false);
		m_ui->toolButtonNavigationInternet->setChecked(false);
		m_ui->toolButtonNavigationNetworks->setChecked(true);
		break;
	}
}

void DialogSettings::on_pushButtonEditProfile_clicked()
{
	QSkinDialog *dlgSkinProfile = new QSkinDialog(false, true, false, false, this);
	DialogProfile *dlgProfile = new DialogProfile(this);

	dlgSkinProfile->addChildWidget(dlgProfile);

	connect(dlgProfile, SIGNAL(closed()), dlgSkinProfile, SLOT(close()));
	dlgSkinProfile->show();
}

void DialogSettings::on_labelConfigureG2_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(16);
}

void DialogSettings::on_labelConfigureAres_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(17);
}

void DialogSettings::on_labelConfigureEDonkey_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(18);
}

void DialogSettings::on_labelConfigureBitTorrent_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(19);
}

void DialogSettings::on_pushButtonShowAdultFilter_clicked()
{
	m_ui->groupBoxAdultFilter->setVisible(true);
}

void DialogSettings::on_listWidgetNavigationGeneral_clicked(QModelIndex index)
{
	m_ui->pagesSettings->setCurrentIndex(index.row() + 1);
}

void DialogSettings::on_listWidgetNavigationInternet_clicked(QModelIndex index)
{
	m_ui->pagesSettings->setCurrentIndex(index.row() + 10);
}

void DialogSettings::on_listWidgetNavigationNetworks_clicked(QModelIndex index)
{
	m_ui->pagesSettings->setCurrentIndex(index.row() + 16);
}

void DialogSettings::on_pushButtonApply_clicked()
{
	// Save Basic Settings
	quazaaSettings.Basic.StartWithSystem = m_ui->checkBoxSystemStart->isChecked();
	quazaaSettings.Basic.ConnectOnStartup = m_ui->checkBoxAutoConnect->isChecked();
	quazaaSettings.Basic.CloseMode = m_ui->comboBoxCloseButton->currentIndex();
	quazaaSettings.Basic.MinimizeToTray = m_ui->checkBoxMinimiseToTray->isChecked();
	quazaaSettings.Basic.SearchTips = m_ui->checkBoxSearchTooltips->isChecked();
	quazaaSettings.Basic.LibraryTips = m_ui->checkBoxLibraryTooltips->isChecked();
	quazaaSettings.Basic.DownloadsTips = m_ui->checkBoxDownloadTooltips->isChecked();
	quazaaSettings.Basic.UploadsTips = m_ui->checkBoxUploadTooltips->isChecked();
	quazaaSettings.Basic.NeighboursTips = m_ui->checkBoxNeighbourTooltips->isChecked();
	quazaaSettings.Basic.MediaTips = m_ui->checkBoxMediaPlayerTooltips->isChecked();
	quazaaSettings.Basic.ChatTips = m_ui->checkBoxChatTooltips->isChecked();
	quazaaSettings.Basic.TipDelay = m_ui->spinBoxToolTipDelay->value();
	quazaaSettings.Basic.TipTransparency = m_ui->horizontalSliderTransparency->value();
	quazaaSettings.Basic.DiskSpaceWarning = m_ui->spinBoxDiskSpaceWarning->value();
	quazaaSettings.Basic.DiskSpaceStop = m_ui->spinBoxDiskSpaceStop->value();

	// Save Parental Settings
	quazaaSettings.Parental.ChatAdultCensor = m_ui->checkBoxCensor->isChecked();
	quazaaSettings.Parental.FilterAdultSearchResults = m_ui->checkBoxFilterAdult->isChecked();
	quazaaSettings.Parental.AdultFilter.clear();
	for (int m_iAdultFilterRow = 0; m_iAdultFilterRow < m_ui->listWidgetAdultFilter->count(); m_iAdultFilterRow++)
	{
		m_ui->listWidgetAdultFilter->setCurrentRow(m_iAdultFilterRow);
		quazaaSettings.Parental.AdultFilter.append(m_ui->listWidgetAdultFilter->currentItem()->text());
	}

	// Save Library Settings
	quazaaSettings.Library.RememberViews = m_ui->checkBoxRememberViews->isChecked();
	quazaaSettings.Library.WatchFolders = m_ui->checkBoxWatchFolders->isChecked();
	quazaaSettings.Library.HighPriorityHashing = m_ui->checkBoxQuickHashing->isChecked();
	quazaaSettings.Library.HashWindow = m_ui->checkBoxDisplayHashingProgress->isChecked();
	quazaaSettings.Library.GhostFiles = m_ui->checkBoxGhostFiles->isChecked();
	quazaaSettings.Library.SmartSeriesDetection = m_ui->checkBoxSeriesDetection->isChecked();
	quazaaSettings.Library.HistoryTotal = m_ui->spinBoxRememberFiles->value();
	quazaaSettings.Library.HistoryDays = m_ui->spinBoxUpToDays->value();
	quazaaSettings.Library.SafeExecuteTypes.clear();
	for (int m_iSafeOpenRow = 0; m_iSafeOpenRow < m_ui->listWidgetSafeOpen->count(); m_iSafeOpenRow++)
	{
		m_ui->listWidgetSafeOpen->setCurrentRow(m_iSafeOpenRow);
		quazaaSettings.Library.SafeExecuteTypes.append(m_ui->listWidgetSafeOpen->currentItem()->text());
	}
	quazaaSettings.Library.NeverShareTypes.clear();
	for (int m_iNeverShareRow = 0; m_iNeverShareRow < m_ui->listWidgetNeverShare->count(); m_iNeverShareRow++)
	{
		m_ui->listWidgetNeverShare->setCurrentRow(m_iNeverShareRow);
		quazaaSettings.Library.NeverShareTypes.append(m_ui->listWidgetNeverShare->currentItem()->text());
	}

	// Save Media Player Settings
	quazaaSettings.Media.Handler = m_ui->comboBoxChoosePlayer->currentIndex();
	quazaaSettings.Media.CustomPlayerPath = m_ui->lineEditCustomPlayerPath->text();
	quazaaSettings.Media.FileTypes.clear();
	for (int m_iPlayerFilesListRow = 0; m_iPlayerFilesListRow < m_ui->listWidgetPlayerFilesList->count(); m_iPlayerFilesListRow++)
	{
		m_ui->listWidgetPlayerFilesList->setCurrentRow(m_iPlayerFilesListRow);
		quazaaSettings.Media.FileTypes.append(m_ui->listWidgetPlayerFilesList->currentItem()->text());
	}

	// Save Search Settings
	quazaaSettings.Search.ExpandSearchMatches = m_ui->checkBoxExpandMultiSorce->isChecked();
	quazaaSettings.Search.SwitchOnDownload = m_ui->checkBoxSwitchOnDownload->isChecked();
	quazaaSettings.Search.HighlightNew = m_ui->checkBoxHighlightNewSearchMatches->isChecked();

	// Save Chat Settings
	quazaaSettings.Chat.GnutellaChatEnable = m_ui->checkBoxGnutellaChatEnable->isChecked();
	quazaaSettings.Chat.EnableChatAllNetworks = m_ui->checkBoxEnableChatOther->isChecked();
	quazaaSettings.Chat.ConnectOnStartup = m_ui->checkBoxConnectIRCOnStart->isChecked();
	quazaaSettings.Chat.EnableFileTransfers = m_ui->checkBoxEnableIRCFileTransfers->isChecked();
	quazaaSettings.Chat.ShowTimestamp = m_ui->checkBoxIRCShowTimestamp->isChecked();
	quazaaSettings.Chat.AwayMessageIdleTime = m_ui->spinBoxChatIdleMessage->value();
	quazaaSettings.Chat.ScreenFont = fontTempChat;
	quazaaSettings.Chat.IrcServerName = m_ui->lineEditIRCServer->text();
	quazaaSettings.Chat.IrcServerPort = m_ui->spinBoxIRCPort->value();
	quazaaSettings.Chat.IrcUseSSL = m_ui->checkBoxIRCSSL->isChecked();
	quazaaSettings.Chat.ColorChatBackground.setNamedColor(colorTempChatBackground.name());
	quazaaSettings.Chat.ColorNormalText.setNamedColor(colorTempNormalText.name());
	quazaaSettings.Chat.ColorServerMessagesText.setNamedColor(colorTempServerMessages.name());
	quazaaSettings.Chat.ColorTopicsText.setNamedColor(colorTempTopics.name());
	quazaaSettings.Chat.ColorRoomActionsText.setNamedColor(colorTempRoomActions.name());
	quazaaSettings.Chat.ColorNoticesText.setNamedColor(colorTempNotices.name());

	// Save Connection Settings
	quazaaSettings.Connection.InSpeed = (m_ui->doubleSpinBoxInSpeed->value()/8)*1024;
	quazaaSettings.Connection.OutSpeed = (m_ui->doubleSpinBoxOutSpeed->value()/8)*1024;
	quazaaSettings.Connection.Port = m_ui->spinBoxQuazaaPort->value();
	quazaaSettings.Connection.RandomPort = m_ui->checkBoxRandomPort->isChecked();
	quazaaSettings.Connection.TimeoutConnect = m_ui->spinBoxConnectionTimeout->value();

	// Save Web Settings
	quazaaSettings.Web.Magnet = m_ui->checkBoxMagnetLinks->isChecked();
	quazaaSettings.Web.Gnutella = m_ui->checkBoxGnutellaLinks->isChecked();
	quazaaSettings.Web.Ares = m_ui->checkBoxAresLinks->isChecked();
	quazaaSettings.Web.Torrent = m_ui->checkBoxBitTorrentLinks->isChecked();
	quazaaSettings.Web.Piolet = m_ui->checkBoxPioletLinks->isChecked();
	quazaaSettings.Web.ED2K = m_ui->checkBoxEDonkeyLinks->isChecked();
	quazaaSettings.Web.BrowserIntegration = m_ui->checkBoxManageWebDownloads->isChecked();
	quazaaSettings.Web.ManageDownloadTypes.clear();
	for (int m_iDownloadTypesRow = 0; m_iDownloadTypesRow < m_ui->listWidgetManageDownloadTypes->count(); m_iDownloadTypesRow++)
	{
		m_ui->listWidgetManageDownloadTypes->setCurrentRow(m_iDownloadTypesRow);
		quazaaSettings.Web.ManageDownloadTypes.append(m_ui->listWidgetManageDownloadTypes->currentItem()->text());
	}

	// Save Transfer Settings
	quazaaSettings.Transfers.RequireConnectedNetwork = m_ui->checkBoxOnlyDownloadConnectedNetworks->isChecked();
	quazaaSettings.Transfers.SimpleProgressBar = m_ui->checkBoxSimpleProgress->isChecked();
	quazaaSettings.Transfers.RatesUnit = m_ui->comboBoxRates->currentIndex();

	// Save Download Settings
	quazaaSettings.Downloads.ExpandDownloads = m_ui->checkBoxExpandDownloads->isChecked();
	quazaaSettings.Downloads.CompletePath = m_ui->lineEditSaveFolder->text();
	quazaaSettings.Downloads.IncompletePath = m_ui->lineEditTempFolder->text();
	quazaaSettings.Downloads.QueueLimit = m_ui->comboBoxQueLength->currentIndex();
	quazaaSettings.Downloads.MaxFiles = m_ui->spinBoxMaxFiles->value();
	quazaaSettings.Downloads.MaxTransfers = m_ui->spinBoxMaxTransfers->value();
	quazaaSettings.Downloads.MaxTransfersPerFile = m_ui->spinBoxTransfersPerFile->value();

	// Save Upload Settings
	quazaaSettings.Uploads.SharePartials = m_ui->checkBoxSharePartials->isChecked();
	quazaaSettings.Uploads.HubShareLimiting = m_ui->checkBoxSharingLimitHub->isChecked();
	quazaaSettings.Uploads.SharePreviews = m_ui->checkBoxSharePreviews->isChecked();
	quazaaSettings.Uploads.MaxPerHost = m_ui->spinBoxUniqueHostLimit->value();

	// Save Security Settings
	quazaaSettings.Security.ChatFilter = m_ui->checkBoxFilterChatSpam->isChecked();
	quazaaSettings.Security.ED2kChatFilter = m_ui->checkBoxFilterEDonkeyChatSpam->isChecked();
	quazaaSettings.Security.AllowProfileBrowse = m_ui->checkBoxAllowBrowseProfile->isChecked();
	quazaaSettings.Security.IrcFloodProtection = m_ui->checkBoxChatFloodProtection->isChecked();
	quazaaSettings.Security.IrcFloodLimit = m_ui->spinBoxChatFloodLimit->value();
	quazaaSettings.Security.RemoteEnable = m_ui->checkBoxEnableRemote->isChecked();
	quazaaSettings.Security.RemoteUsername = m_ui->lineEditRemoteUserName->text();
	quazaaSettings.Security.RemotePassword = m_ui->lineEditRemotePassword->text();
	quazaaSettings.Security.SearchIgnoreLocalIP = m_ui->checkBoxIgnoreLocalIP->isChecked();
	quazaaSettings.Security.EnableUPnP = m_ui->checkBoxEnableUPnP->isChecked();
	quazaaSettings.Security.AllowSharesBrowse = m_ui->checkBoxAllowBrowseShares->isChecked();
	quazaaSettings.Security.BlockedAgentUploadFilter.clear();
	for (int m_iUserAgentsRow = 0; m_iUserAgentsRow < m_ui->listWidgetUserAgents->count(); m_iUserAgentsRow++)
	{
		m_ui->listWidgetUserAgents->setCurrentRow(m_iUserAgentsRow);
		quazaaSettings.Security.BlockedAgentUploadFilter.append(m_ui->listWidgetUserAgents->currentItem()->text());
	}

	// Save Gnutella 2 Settings
	quazaaSettings.Gnutella2.Enable = m_ui->checkBoxConnectG2->isChecked();
	quazaaSettings.Gnutella2.ClientMode = m_ui->comboBoxG2Mode->currentIndex();
	quazaaSettings.Gnutella2.NumHubs = m_ui->spinBoxG2LeafToHub->value();
	quazaaSettings.Gnutella2.NumLeafs = m_ui->spinBoxG2HubToLeaf->value();
	quazaaSettings.Gnutella2.NumPeers = m_ui->spinBoxG2HubToHub->value();

	// Save Ares Settings
	quazaaSettings.Ares.Enable = m_ui->checkBoxConnectAres->isChecked();

	// Save eDonkey 2k Settings
	quazaaSettings.EDonkey.Enable = m_ui->checkBoxConnectEDonkey->isChecked();
	quazaaSettings.EDonkey.EnableKad = m_ui->checkBoxConnectKAD->isChecked();
	quazaaSettings.EDonkey.SearchCachedServers = m_ui->checkBoxED2kSearchCahedServers->isChecked();
	quazaaSettings.EDonkey.MaxResults = m_ui->spinBoxED2kMaxResults->value();
	quazaaSettings.EDonkey.LearnNewServers = m_ui->checkBoxED2kUpdateServerList->isChecked();
	quazaaSettings.EDonkey.MaxClients = m_ui->spinBoxED2kMaxClients->value();
	quazaaSettings.EDonkey.MetAutoQuery = m_ui->checkBoxAutoQueryServerList->isChecked();
	quazaaSettings.EDonkey.ServerListURL = m_ui->lineEditEDonkeyServerListUrl->text();

	// Save BitTorrent Settings
	quazaaSettings.BitTorrent.UseSaveDialog = m_ui->checkBoxTorrentSaveDialog->isChecked();
	quazaaSettings.BitTorrent.StartPaused = m_ui->checkBoxTorrentsStartPaused->isChecked();
	quazaaSettings.BitTorrent.UseTemp = m_ui->checkBoxTorrentsUseTemp->isChecked();
	quazaaSettings.BitTorrent.Managed = m_ui->checkBoxManagedTorrent->isChecked();
	quazaaSettings.BitTorrent.Endgame = m_ui->checkBoxTorrentsEndgame->isChecked();
	quazaaSettings.BitTorrent.DownloadTorrents = m_ui->spinBoxTorrentsSimultaneous->value();
	quazaaSettings.BitTorrent.DownloadConnections = m_ui->spinBoxTorrentsClientConnections->value();
	quazaaSettings.BitTorrent.AutoClear = m_ui->checkBoxTorrentsClearDownloaded->isChecked();
	quazaaSettings.BitTorrent.ClearRatio = m_ui->spinBoxTorrentsRatioClear->value();
	quazaaSettings.BitTorrent.PreferBTSources = m_ui->checkBoxTorrentsPreferTorrent->isChecked();
	quazaaSettings.BitTorrent.UseKademlia = m_ui->checkBoxTorrentsUseKademlia->isChecked();
	quazaaSettings.BitTorrent.TorrentPath = m_ui->lineEditTorrentFolder->text();

	if (newSkinSelected)
	{
		// Save Skin Settings
		quazaaSettings.Skin.File = skinFile;
		skinSettings.loadSkin(skinFile);

		emit skinChanged();
		quazaaSettings.saveSkinSettings();
	}

	// Reset Apply Enabled To False
	quazaaSettings.saveSettings();
	m_ui->pushButtonApply->setEnabled(false);
}

void DialogSettings::on_checkBoxSystemStart_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxAutoConnect_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_comboBoxCloseButton_currentIndexChanged(int index)
{
	Q_UNUSED(index);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxMinimiseToTray_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxSearchTooltips_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxLibraryTooltips_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxDownloadTooltips_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxUploadTooltips_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxNeighbourTooltips_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxMediaPlayerTooltips_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxChatTooltips_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxToolTipDelay_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_horizontalSliderTransparency_valueChanged(int value)
{
	Q_UNUSED(value);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxDiskSpaceWarning_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxDiskSpaceStop_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxCensor_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxFilterAdult_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_pushButtonAddWord_clicked()
{
	if (m_ui->lineEditAdultFilter->text() != "")
	{
		m_ui->listWidgetAdultFilter->addItem(m_ui->lineEditAdultFilter->text());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonRemoveWord_clicked()
{
	if(m_ui->listWidgetAdultFilter->currentRow() != -1)
	{
		m_ui->listWidgetAdultFilter->takeItem(m_ui->listWidgetAdultFilter->currentRow());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_checkBoxRememberViews_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxWatchFolders_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxQuickHashing_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxDisplayHashingProgress_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxGhostFiles_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxSeriesDetection_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxRememberFiles_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxUpToDays_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_pushButtonAddSafe_clicked()
{
	if (m_ui->lineEditSafeOpen->text() != "")
	{
		m_ui->listWidgetSafeOpen->addItem(m_ui->lineEditSafeOpen->text());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonRemoveSafe_clicked()
{
	if(m_ui->listWidgetSafeOpen->currentRow() != -1)
	{
		m_ui->listWidgetSafeOpen->takeItem(m_ui->listWidgetSafeOpen->currentRow());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonNeverShareAdd_clicked()
{
	if (m_ui->lineEditNeverShare->text() != "")
	{
		m_ui->listWidgetNeverShare->addItem(m_ui->lineEditNeverShare->text());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonNeverShareRemove_clicked()
{
	if(m_ui->listWidgetNeverShare->currentRow() != -1)
	{
		m_ui->listWidgetNeverShare->takeItem(m_ui->listWidgetNeverShare->currentRow());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_comboBoxChoosePlayer_currentIndexChanged(int index)
{
	Q_UNUSED(index);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_pushButtonAddPlayerFiles_clicked()
{
	if (m_ui->lineEditPlayerFileTypes->text() != "")
	{
		m_ui->listWidgetPlayerFilesList->addItem(m_ui->lineEditPlayerFileTypes->text());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonRemovePlayerFiles_clicked()
{
	if(m_ui->listWidgetPlayerFilesList->currentRow() != -1)
	{
		m_ui->listWidgetPlayerFilesList->takeItem(m_ui->listWidgetPlayerFilesList->currentRow());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_checkBoxExpandMultiSorce_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxSwitchOnDownload_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxHighlightNewSearchMatches_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxGnutellaChatEnable_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEnableChatOther_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxConnectIRCOnStart_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEnableIRCFileTransfers_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxIRCShowTimestamp_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxChatIdleMessage_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_lineEditIRCServer_editingFinished()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxIRCPort_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_toolButtonColorChatBackground_clicked()
{
	QColor color = QColorDialog::getColor(colorTempChatBackground, this);
	if (color.isValid()) {
		m_ui->toolButtonColorChatBackground->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + color.name() + ";}");
		m_ui->frameIRCColors->setStyleSheet("background-color: " + color.name() + ";");
		colorTempChatBackground.setNamedColor(color.name());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_toolButtonColorNormalText_clicked()
{
	QColor color = QColorDialog::getColor(colorTempChatBackground, this);
	if (color.isValid()) {
		m_ui->toolButtonColorNormalText->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + color.name() + ";}");
		m_ui->labelNormalText->setStyleSheet("color: " + color.name() + ";");
		colorTempNormalText.setNamedColor(color.name());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_toolButtonColorNotices_clicked()
{
	QColor color = QColorDialog::getColor(colorTempChatBackground, this);
	if (color.isValid()) {
		m_ui->toolButtonColorNotices->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + color.name() + ";}");
		m_ui->labelNotices->setStyleSheet("color: " + color.name() + ";");
		colorTempNotices.setNamedColor(color.name());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_toolButtonColorRoomActions_clicked()
{
	QColor color = QColorDialog::getColor(colorTempChatBackground, this);
	if (color.isValid()) {
		m_ui->toolButtonColorRoomActions->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + color.name() + ";}");
		m_ui->labelRoomActions->setStyleSheet("color: " + color.name() + ";");
		colorTempRoomActions.setNamedColor(color.name());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_toolButtonColorServerMessages_clicked()
{
	QColor color = QColorDialog::getColor(colorTempChatBackground, this);
	if (color.isValid()) {
		m_ui->toolButtonColorServerMessages->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + color.name() + ";}");
		m_ui->labelServerMessages->setStyleSheet("color: " + color.name() + ";");
		colorTempServerMessages.setNamedColor(color.name());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_toolButtonColorTopics_clicked()
{
	QColor color = QColorDialog::getColor(colorTempChatBackground, this);
	if (color.isValid()) {
		m_ui->toolButtonColorTopics->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + color.name() + ";}");
		m_ui->labelTopics->setStyleSheet("color: " + color.name() + ";");
		colorTempTopics.setNamedColor(color.name());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_comboBoxInboundAddress_activated(QString )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxForceBindingAddress_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_comboBoxOutboundAddress_activated(QString )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxQuazaaPort_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxRandomPort_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxConnectionTimeout_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxHandshakeTimeout_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxMagnetLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxGnutellaLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxAresLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxBitTorrentLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxPioletLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEDonkeyLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxManageWebDownloads_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_pushButtonAddManageDownloadTypes_clicked()
{
	if (m_ui->lineEditManageDownloadTypes->text() != "")
	{
		m_ui->listWidgetManageDownloadTypes->addItem(m_ui->lineEditManageDownloadTypes->text());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonRemoveManageDownloadTypes_clicked()
{
	if(m_ui->listWidgetManageDownloadTypes->currentRow() != -1)
	{
		m_ui->listWidgetManageDownloadTypes->takeItem(m_ui->listWidgetManageDownloadTypes->currentRow());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_checkBoxOnlyDownloadConnectedNetworks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxSimpleProgress_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_comboBoxRates_currentIndexChanged(int index)
{
	Q_UNUSED(index);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_doubleSpinBoxInSpeed_valueChanged(double )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_doubleSpinBoxOutSpeed_valueChanged(double )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxExpandDownloads_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxURIPrompt_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_toolButtonSaveBrowse_clicked()
{
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QString directory = QFileDialog::getExistingDirectory(this,
							tr("Select A Folder For Completed Files"),
							m_ui->lineEditSaveFolder->text(),
							options);
	if (!directory.isEmpty())
	{
		m_ui->lineEditSaveFolder->setText(directory);
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonChangeChatFont_clicked()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, fontTempChat, this);
	if (ok) {
		fontTempChat = font;
		m_ui->labelChatFontPreview->setText(fontTempChat.rawName());
		m_ui->labelChatFontPreview->setFont(fontTempChat);
		m_ui->labelNormalText->setFont(fontTempChat);
		m_ui->labelNotices->setFont(fontTempChat);
		m_ui->labelRoomActions->setFont(fontTempChat);
		m_ui->labelServerMessages->setFont(fontTempChat);
		m_ui->labelTopics->setFont(fontTempChat);
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_toolButtonTempBrowse_clicked()
{
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QString directory = QFileDialog::getExistingDirectory(this,
							tr("Select A Folder For Incomplete Files"),
							m_ui->lineEditTempFolder->text(),
							options);
	if (!directory.isEmpty())
	{
		m_ui->lineEditTempFolder->setText(directory);
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_comboBoxQueLength_currentIndexChanged(int index)
{
	Q_UNUSED(index);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxMaxTransfers_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxMaxFiles_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxTransfersPerFile_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxSharePartials_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxSharePreviews_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxSharingLimitHub_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxUniqueHostLimit_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxFilterChatSpam_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxFilterEDonkeyChatSpam_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxAllowBrowseProfile_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxChatFloodProtection_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxChatFloodLimit_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEnableRemote_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_lineEditRemoteUserName_editingFinished()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_lineEditRemotePassword_editingFinished()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxIgnoreLocalIP_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEnableUPnP_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxAllowBrowseShares_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_pushButtonUserAgentAdd_clicked()
{
	if (m_ui->lineEditUserAgents->text() != "")
	{
		m_ui->listWidgetUserAgents->addItem(m_ui->lineEditUserAgents->text());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_pushButtonUserAgentRemove_clicked()
{
	if(m_ui->listWidgetUserAgents->currentRow() != -1)
	{
		m_ui->listWidgetUserAgents->takeItem(m_ui->listWidgetUserAgents->currentRow());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_comboBoxG2Mode_currentIndexChanged(int index)
{
	Q_UNUSED(index);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxG2LeafToHub_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxG2HubToLeaf_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxG2HubToHub_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxED2kSearchCahedServers_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxED2kMaxResults_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxED2kUpdateServerList_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxED2kMaxClients_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxAutoQueryServerList_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_lineEditEDonkeyServerListUrl_editingFinished()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxTorrentSaveDialog_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxManagedTorrent_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxTorrentsStartPaused_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxTorrentsEndgame_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxTorrentsUseTemp_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxTorrentsSimultaneous_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxTorrentsClientConnections_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxTorrentsClearDownloaded_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxTorrentsRatioClear_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxTorrentsPreferTorrent_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxTorrentsUseKademlia_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_toolButtonTorrentBrowse_clicked()
{
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QString directory = QFileDialog::getExistingDirectory(this,
							tr("Select A Folder For .torrent Files"),
							m_ui->lineEditTorrentFolder->text(),
							options);
	if (!directory.isEmpty())
	{
		m_ui->lineEditTorrentFolder->setText(directory);
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogSettings::on_checkBoxConnectG2_toggled(bool checked)
{
	Q_UNUSED(checked);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxConnectAres_toggled(bool checked)
{
	Q_UNUSED(checked);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxConnectEDonkey_toggled(bool checked)
{
	Q_UNUSED(checked);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxG2Connect_toggled(bool checked)
{
	Q_UNUSED(checked);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxAresConnect_toggled(bool checked)
{
	Q_UNUSED(checked);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEDonkeyConnect_toggled(bool checked)
{
	Q_UNUSED(checked);

	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_listWidgetSkins_itemClicked(QListWidgetItem* item)
{
	newSkinSelected = true;
	m_ui->pushButtonApply->setEnabled(true);
	QSettings reader((qApp->applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk"), QSettings::IniFormat);
	skinFile = (qApp->applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk");
	tempSkinName = reader.value("skinName", "").toString();
	tempSkinAuthor = reader.value("skinAuthor", "").toString();
	tempSkinVersion = reader.value("skinVersion", "").toString();
	tempSkinDescription = reader.value("skinDescription", "").toString();

	m_ui->labelSkinAuthor->setText(tempSkinAuthor);
	m_ui->labelSkinVersion->setText(tempSkinVersion);
	m_ui->plainTextEditSkinDescription->setPlainText(tempSkinDescription);
}

void DialogSettings::on_pushButtonPreviewSkin_clicked()
{
	if (m_ui->listWidgetSkins->currentRow() != -1)
	{
		QSkinDialog *dlgSkinPreviewFrame = new QSkinDialog(true, true, false, true);
		DialogSkinPreview *dlgSkinPreview = new DialogSkinPreview(this);

		dlgSkinPreviewFrame->addChildWidget(dlgSkinPreview);

		connect(dlgSkinPreview, SIGNAL(closed()), dlgSkinPreviewFrame, SLOT(close()));
		dlgSkinPreviewFrame->loadPreviewSkin(skinFile);
		dlgSkinPreview->loadSkin(skinFile);
		dlgSkinPreviewFrame->exec();
	}
}

void DialogSettings::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
	m_ui->frameSettingsNavigator->setStyleSheet(skinSettings.sidebarBackground);
	m_ui->toolButtonNavigationGeneral->setStyleSheet(skinSettings.sidebarTaskHeader);
	m_ui->toolButtonNavigationInternet->setStyleSheet(skinSettings.sidebarTaskHeader);
	m_ui->toolButtonNavigationNetworks->setStyleSheet(skinSettings.sidebarTaskHeader);
}

void DialogSettings::on_checkBoxIRCSSL_toggled(bool checked)
{
	Q_UNUSED(checked);

	m_ui->pushButtonApply->setEnabled(true);
}
