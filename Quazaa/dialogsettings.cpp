//
// dialogsettings.cpp
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

#include "dialogsettings.h"
#include "ui_dialogsettings.h"
#include "dialogprofile.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>

DialogSettings::DialogSettings(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogSettings)
{
	m_ui->setupUi(this);

	// Load Basic Settings
	m_ui->checkBoxSystemStart->setChecked(Settings.Basic.StartWithSystem);
	m_ui->checkBoxAutoConnect->setChecked(Settings.Basic.ConnectOnStartup);
	m_ui->comboBoxCloseButton->setCurrentIndex(Settings.Basic.CloseMode);
	m_ui->checkBoxMinimiseToTray->setChecked(Settings.Basic.TrayMinimise);
	m_ui->checkBoxSearchTooltips->setChecked(Settings.Basic.SearchTips);
	m_ui->checkBoxLibraryTooltips->setChecked(Settings.Basic.LibraryTips);
	m_ui->checkBoxDownloadTooltips->setChecked(Settings.Basic.DownloadsTips);
	m_ui->checkBoxUploadTooltips->setChecked(Settings.Basic.UploadsTips);
	m_ui->checkBoxNeighbourTooltips->setChecked(Settings.Basic.NeighboursTips);
	m_ui->checkBoxMediaPlayerTooltips->setChecked(Settings.Basic.MediaTips);
	m_ui->checkBoxChatTooltips->setChecked(Settings.Basic.ChatTips);
	m_ui->spinBoxToolTipDelay->setValue(Settings.Basic.TipDelay);
	m_ui->horizontalSliderTransparency->setValue(Settings.Basic.TipTransparency);
	m_ui->spinBoxDiskSpaceWarning->setValue(Settings.Basic.DiskSpaceWarning);
	m_ui->spinBoxDiskSpaceStop->setValue(Settings.Basic.DiskSpaceStop);

	// Load Parental Settings
	m_ui->checkBoxCensor->setChecked(Settings.Parental.ChatAdultCensor);
	m_ui->checkBoxFilterAdult->setChecked(Settings.Parental.FilterAdultSearchResults);
	m_ui->listWidgetAdultFilter->addItems(Settings.Parental.AdultFilter);

	// Load Library Settings
	m_ui->checkBoxRememberViews->setChecked(Settings.Library.RememberViews);
	m_ui->checkBoxWatchFolders->setChecked(Settings.Library.WatchFolders);
	m_ui->checkBoxQuickHashing->setChecked(Settings.Library.HighPriorityHashing);
	m_ui->checkBoxDisplayHashingProgress->setChecked(Settings.Library.HashWindow);
	m_ui->checkBoxGhostFiles->setChecked(Settings.Library.GhostFiles);
	m_ui->checkBoxSeriesDetection->setChecked(Settings.Library.SmartSeriesDetection);
	m_ui->spinBoxRememberFiles->setValue(Settings.Library.HistoryTotal);
	m_ui->spinBoxUpToDays->setValue(Settings.Library.HistoryDays);
	m_ui->listWidgetSafeOpen->addItems(Settings.Library.SafeExecuteTypes);
	m_ui->listWidgetNeverShare->addItems(Settings.Library.NeverShareTypes);

	// Load Media Player Settings
	m_ui->comboBoxChoosePlayer->setCurrentIndex(Settings.MediaPlayer.MediaHandler);
	m_ui->lineEditCustomPlayerPath->setText(Settings.MediaPlayer.CustomPlayerPath);
	m_ui->listWidgetPlayerFilesList->addItems(Settings.MediaPlayer.FileTypes);

	// Load Search Settings
	m_ui->checkBoxExpandMultiSorce->setChecked(Settings.Search.ExpandSearchMatches);
	m_ui->checkBoxSwitchOnDownload->setChecked(Settings.Search.SwitchOnDownload);
	m_ui->checkBoxHighlightNewSearchMatches->setChecked(Settings.Search.HighlightNew);

	// Load Chat Settings
	m_ui->checkBoxGnutellaChatEnable->setChecked(Settings.Chat.GnutellaChatEnable);
	m_ui->checkBoxEnableChatOther->setChecked(Settings.Chat.EnableChatAllNetworks);
	m_ui->checkBoxConnectIRCOnStart->setChecked(Settings.Chat.ConnectOnStartup);
	m_ui->checkBoxEnableIRCFileTransfers->setChecked(Settings.Chat.EnableFileTransfers);
	m_ui->checkBoxIRCShowTimestamp->setChecked(Settings.Chat.ShowTimestamp);
	m_ui->spinBoxChatIdleMessage->setValue(Settings.Chat.AwayMessageIdleTime);
	m_ui->lineEditIRCServer->setText(Settings.Chat.IrcServerName);
	m_ui->spinBoxIRCPort->setValue(Settings.Chat.IrcServerPort);
	m_ui->labelChatFontPreview->setText(Settings.Chat.ScreenFont.rawName());
	m_ui->labelChatFontPreview->setFont(Settings.Chat.ScreenFont);
	fontTempChat = Settings.Chat.ScreenFont;
	m_ui->toolButtonColorChatBackground->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + Settings.Chat.ColorChatBackground.name() + ";}");
	m_ui->frameIRCColors->setStyleSheet("background-color: " + Settings.Chat.ColorChatBackground.name() + ";");
	colorTempChatBackground.setNamedColor(Settings.Chat.ColorChatBackground.name());
	m_ui->toolButtonColorNormalText->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + Settings.Chat.ColorNormalText.name() + ";}");
	m_ui->labelNormalText->setFont(Settings.Chat.ScreenFont);
	m_ui->labelNormalText->setStyleSheet("color: " + Settings.Chat.ColorNormalText.name() + ";");
	colorTempNormalText.setNamedColor(Settings.Chat.ColorNormalText.name());
	m_ui->toolButtonColorServerMessages->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + Settings.Chat.ColorServerMessagesText.name() + ";}");
	m_ui->labelServerMessages->setFont(Settings.Chat.ScreenFont);
	m_ui->labelServerMessages->setStyleSheet("color: " + Settings.Chat.ColorServerMessagesText.name() + ";");
	colorTempServerMessages.setNamedColor(Settings.Chat.ColorServerMessagesText.name());
	m_ui->toolButtonColorTopics->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + Settings.Chat.ColorTopicsText.name() + ";}");
	m_ui->labelTopics->setFont(Settings.Chat.ScreenFont);
	m_ui->labelTopics->setStyleSheet("color: " + Settings.Chat.ColorTopicsText.name() + ";");
	colorTempTopics.setNamedColor(Settings.Chat.ColorTopicsText.name());
	m_ui->toolButtonColorRoomActions->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + Settings.Chat.ColorRoomActionsText.name() + ";}");
	m_ui->labelRoomActions->setFont(Settings.Chat.ScreenFont);
	m_ui->labelRoomActions->setStyleSheet("color: " + Settings.Chat.ColorRoomActionsText.name() + ";");
	colorTempRoomActions.setNamedColor(Settings.Chat.ColorRoomActionsText.name());
	m_ui->toolButtonColorNotices->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + Settings.Chat.ColorNoticesText.name() + ";}");
	m_ui->labelNotices->setFont(Settings.Chat.ScreenFont);
	m_ui->labelNotices->setStyleSheet("color: " + Settings.Chat.ColorNoticesText.name() + ";");
	colorTempNotices.setNamedColor(Settings.Chat.ColorNoticesText.name());

	// Load Connection Settings
	if (Settings.Connection.InAddress != "")
	{
		m_ui->comboBoxInboundAddress->addItem(Settings.Connection.InAddress);
	}
	m_ui->checkBoxForceBindingAddress->setChecked(Settings.Connection.InBind);
	if (Settings.Connection.OutAddress != "")
	{
		m_ui->comboBoxOutboundAddress->addItem(Settings.Connection.OutAddress);
	}
	m_ui->spinBoxQuazaaPort->setValue(Settings.Connection.Port);
	m_ui->checkBoxRandomPort->setChecked(Settings.Connection.RandomPort);
	m_ui->comboBoxAcceptIncoming->setCurrentIndex(Settings.Connection.CanAcceptIncoming);
	m_ui->spinBoxConnectionTimeout->setValue(Settings.Connection.TimeoutConnect);
	m_ui->spinBoxHandshakeTimeout->setValue(Settings.Connection.TimeoutHandshake);

	// Load Web Settings
	m_ui->checkBoxMagnetLinks->setChecked(Settings.Web.Magnet);
	m_ui->checkBoxGnutellaLinks->setChecked(Settings.Web.Gnutella);
	m_ui->checkBoxAresLinks->setChecked(Settings.Web.Ares);
	m_ui->checkBoxBittorrentLinks->setChecked(Settings.Web.Torrent);
	m_ui->checkBoxPioletLinks->setChecked(Settings.Web.Piolet);
	m_ui->checkBoxEDonkey2kLinks->setChecked(Settings.Web.ED2K);
	m_ui->checkBoxManageWebDownloads->setChecked(Settings.Web.BrowserIntegration);
	m_ui->listWidgetManageDownloadTypes->addItems(Settings.Web.ManageDownloadTypes);

	// Load Transfer Settings
	m_ui->checkBoxOnlyDownloadConnectedNetworks->setChecked(Settings.Transfers.RequireConnectedNetwork);
	m_ui->checkBoxSimpleProgress->setChecked(Settings.Transfers.SimpleProgressBar);
	m_ui->comboBoxRates->setCurrentIndex(Settings.Transfers.RatesUnit);
	m_ui->doubleSpinBoxDownloadSpeed->setValue(Settings.Transfers.BandwidthDownloads);
	m_ui->doubleSpinBoxUploadSpeed->setValue(Settings.Transfers.BandwidthUploads);

	// Load Download Settings
	m_ui->checkBoxExpandDownloads->setChecked(Settings.Downloads.ExpandDownloads);
	m_ui->lineEditSaveFolder->setText(Settings.Downloads.CompletePath);
	m_ui->lineEditTempFolder->setText(Settings.Downloads.IncompletePath);
	m_ui->comboBoxQueLength->setCurrentIndex(Settings.Downloads.QueueLimit);
	m_ui->spinBoxMaxFiles->setValue(Settings.Downloads.MaxFiles);
	m_ui->spinBoxMaxTransfers->setValue(Settings.Downloads.MaxTransfers);
	m_ui->spinBoxTransfersPerFile->setValue(Settings.Downloads.MaxTransfersPerFile);

	// Load Upload Settings
	m_ui->checkBoxSharePartials->setChecked(Settings.Uploads.SharePartials);
	m_ui->checkBoxSharingLimitHub->setChecked(Settings.Uploads.HubShareLimiting);
	m_ui->checkBoxSharePreviews->setChecked(Settings.Uploads.SharePreviews);
	m_ui->spinBoxUniqueHostLimit->setValue(Settings.Uploads.MaxPerHost);

	// Load Security Settings
	m_ui->checkBoxFilterChatSpam->setChecked(Settings.Security.ChatFilter);
	m_ui->checkBoxFilterEDonkeyChatSpam->setChecked(Settings.Security.ED2kChatFilter);
	m_ui->checkBoxAllowBrowseProfile->setChecked(Settings.Security.AllowProfileBrowse);
	m_ui->checkBoxChatFloodProtection->setChecked(Settings.Security.IrcFloodProtection);
	m_ui->spinBoxChatFloodLimit->setValue(Settings.Security.IrcFloodLimit);
	m_ui->checkBoxEnableRemote->setChecked(Settings.Security.RemoteEnable);
	m_ui->lineEditRemoteUserName->setText(Settings.Security.RemoteUsername);
	m_ui->lineEditRemotePassword->setText(Settings.Security.RemotePassword);
	m_ui->checkBoxIgnoreLocalIP->setChecked(Settings.Security.SearchIgnoreLocalIP);
	m_ui->checkBoxEnableUPnP->setChecked(Settings.Security.EnableUPnP);
	m_ui->checkBoxAllowBrowseShares->setChecked(Settings.Security.AllowSharesBrowse);
	m_ui->listWidgetUserAgents->addItems(Settings.Security.BlockedAgentUploadFilter);

	// Load Gnutella 2 Settings
	m_ui->checkBoxConnectG2->setChecked(Settings.Gnutella2.Enable);
	m_ui->comboBoxG2Mode->setCurrentIndex(Settings.Gnutella2.ClientMode);
	m_ui->spinBoxG2LeafToHub->setValue(Settings.Gnutella2.NumHubs);
	m_ui->spinBoxG2HubToLeaf->setValue(Settings.Gnutella2.NumLeafs);
	m_ui->spinBoxG2HubToHub->setValue(Settings.Gnutella2.NumPeers);

	// Load Gnutella 1 Settings
	m_ui->checkBoxConnectG1->setChecked(Settings.Gnutella1.Enable);
	m_ui->spinBoxG1LeafToUltrapeer->setValue(Settings.Gnutella1.NumHubs);
	m_ui->spinBoxG1UltrapeerToLeaf->setValue(Settings.Gnutella1.NumLeafs);
	m_ui->spinBoxG1UltrapeerToUltrapeer->setValue(Settings.Gnutella1.NumPeers);

	// Load Gnutella Settings
	m_ui->checkBoxCompressionLeafToHub->setChecked(Settings.Gnutella.CompressLeaf2Hub);
	m_ui->checkBoxCompressionHubToLeaf->setChecked(Settings.Gnutella.CompressHub2Leaf);
	m_ui->checkBoxCompressionHubToHub->setChecked(Settings.Gnutella.CompressHub2Hub);

	// Load Ares Settings
	m_ui->checkBoxConnectAres->setChecked(Settings.Ares.Enable);
	m_ui->checkBoxConnectAres->setChecked(Settings.Ares.Enable);

	// Load eDonkey 2k Settings
	m_ui->checkBoxConnectEDonkey2k->setChecked(Settings.eDonkey2k.Enable);
	m_ui->checkBoxConnectKAD->setChecked(Settings.eDonkey2k.EnableKad);
	m_ui->checkBoxED2kSearchCahedServers->setChecked(Settings.eDonkey2k.SearchCachedServers);
	m_ui->spinBoxED2kMaxResults->setValue(Settings.eDonkey2k.MaxResults);
	m_ui->checkBoxED2kUpdateServerList->setChecked(Settings.eDonkey2k.LearnNewServers);
	m_ui->spinBoxED2kMaxClients->setValue(Settings.eDonkey2k.MaxClients);
	m_ui->checkBoxAutoQueryServerList->setChecked(Settings.eDonkey2k.MetAutoQuery);
	m_ui->lineEditEDonkeyServerListUrl->setText(Settings.eDonkey2k.ServerListURL);

	// Load Bittorrent Settings
	m_ui->checkBoxTorrentSaveDialog->setChecked(Settings.Bittorrent.UseSaveDialog);
	m_ui->checkBoxTorrentsStartPaused->setChecked(Settings.Bittorrent.StartPaused);
	m_ui->checkBoxTorrentsUseTemp->setChecked(Settings.Bittorrent.UseTemp);
	m_ui->checkBoxManagedTorrent->setChecked(Settings.Bittorrent.Managed);
	m_ui->checkBoxTorrentsEndgame->setChecked(Settings.Bittorrent.Endgame);
	m_ui->spinBoxTorrentsSimultaneous->setValue(Settings.Bittorrent.DownloadTorrents);
	m_ui->spinBoxTorrentsClientConnections->setValue(Settings.Bittorrent.DownloadConnections);
	m_ui->checkBoxTorrentsClearDownloaded->setChecked(Settings.Bittorrent.AutoClear);
	m_ui->spinBoxTorrentsRatioClear->setValue(Settings.Bittorrent.ClearRatio);
	m_ui->checkBoxTorrentsPreferTorrent->setChecked(Settings.Bittorrent.PreferBTSources);
	m_ui->checkBoxTorrentsUseKademlia->setChecked(Settings.Bittorrent.UseKademlia);
	m_ui->lineEditTorrentFolder->setText(Settings.Bittorrent.TorrentPath);

	// Set Generic Control States
	m_ui->pagesSettings->setCurrentIndex(0);
	m_ui->toolButtonNavigationGeneral->setChecked(true);
	m_ui->toolButtonNavigationInternet->setChecked(false);
	m_ui->toolButtonNavigationNetworks->setChecked(false);
	m_ui->groupBoxAdultFilter->setVisible(false);
	m_ui->pushButtonApply->setEnabled(false);
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
	close();
}

void DialogSettings::on_pushButtonOK_clicked()
{
	if(m_ui->pushButtonApply->isEnabled())
	{
		m_ui->pushButtonApply->click();
	}
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
	DialogProfile *dlgProfile = new DialogProfile(this);
	dlgProfile->show();
}

void DialogSettings::on_toolButtonNavigationGeneral_toggled(bool checked)
{
	if (checked)
	{
		m_ui->frameNavigationGeneral->setMaximumHeight(16777215);
	} else {
		m_ui->frameNavigationGeneral->setMaximumHeight(25);
	}
}

void DialogSettings::on_toolButtonNavigationInternet_toggled(bool checked)
{
	if (checked)
	{
		m_ui->frameNavigationInternet->setMaximumHeight(16777215);
	} else {
		m_ui->frameNavigationInternet->setMaximumHeight(25);
	}
}

void DialogSettings::on_toolButtonNavigationNetworks_toggled(bool checked)
{
	if (checked)
	{
		m_ui->frameNavigationNetworks->setMaximumHeight(16777215);
	} else {
		m_ui->frameNavigationNetworks->setMaximumHeight(25);
	}
}

void DialogSettings::on_labelConfigureG2_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(16);
}

void DialogSettings::on_labelConfigureG1_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(16);
}

void DialogSettings::on_labelConfigureAres_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(17);
}

void DialogSettings::on_labelConfigureEDonkey2k_linkActivated(QString link)
{
	link.clear();
	m_ui->pagesSettings->setCurrentIndex(18);
}

void DialogSettings::on_labelConfigreBittorrent_linkActivated(QString link)
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
	Settings.Basic.StartWithSystem = m_ui->checkBoxSystemStart->isChecked();
	Settings.Basic.ConnectOnStartup = m_ui->checkBoxAutoConnect->isChecked();
	Settings.Basic.CloseMode = m_ui->comboBoxCloseButton->currentIndex();
	Settings.Basic.TrayMinimise = m_ui->checkBoxMinimiseToTray->isChecked();
	Settings.Basic.SearchTips = m_ui->checkBoxSearchTooltips->isChecked();
	Settings.Basic.LibraryTips = m_ui->checkBoxLibraryTooltips->isChecked();
	Settings.Basic.DownloadsTips = m_ui->checkBoxDownloadTooltips->isChecked();
	Settings.Basic.UploadsTips = m_ui->checkBoxUploadTooltips->isChecked();
	Settings.Basic.NeighboursTips = m_ui->checkBoxNeighbourTooltips->isChecked();
	Settings.Basic.MediaTips = m_ui->checkBoxMediaPlayerTooltips->isChecked();
	Settings.Basic.ChatTips = m_ui->checkBoxChatTooltips->isChecked();
	Settings.Basic.TipDelay = m_ui->spinBoxToolTipDelay->value();
	Settings.Basic.TipTransparency = m_ui->horizontalSliderTransparency->value();
	Settings.Basic.DiskSpaceWarning = m_ui->spinBoxDiskSpaceWarning->value();
	Settings.Basic.DiskSpaceStop = m_ui->spinBoxDiskSpaceStop->value();

	// Save Parental Settings
	Settings.Parental.ChatAdultCensor = m_ui->checkBoxCensor->isChecked();
	Settings.Parental.FilterAdultSearchResults = m_ui->checkBoxFilterAdult->isChecked();
	Settings.Parental.AdultFilter.clear();
	for (int m_iAdultFilterRow = 0; m_iAdultFilterRow < m_ui->listWidgetAdultFilter->count(); m_iAdultFilterRow++)
	{
		m_ui->listWidgetAdultFilter->setCurrentRow(m_iAdultFilterRow);
		Settings.Parental.AdultFilter.append(m_ui->listWidgetAdultFilter->currentItem()->text());
	}

	// Save Library Settings
	Settings.Library.RememberViews = m_ui->checkBoxRememberViews->isChecked();
	Settings.Library.WatchFolders = m_ui->checkBoxWatchFolders->isChecked();
	Settings.Library.HighPriorityHashing = m_ui->checkBoxQuickHashing->isChecked();
	Settings.Library.HashWindow = m_ui->checkBoxDisplayHashingProgress->isChecked();
	Settings.Library.GhostFiles = m_ui->checkBoxGhostFiles->isChecked();
	Settings.Library.SmartSeriesDetection = m_ui->checkBoxSeriesDetection->isChecked();
	Settings.Library.HistoryTotal = m_ui->spinBoxRememberFiles->value();
	Settings.Library.HistoryDays = m_ui->spinBoxUpToDays->value();
	Settings.Library.SafeExecuteTypes.clear();
	for (int m_iSafeOpenRow = 0; m_iSafeOpenRow < m_ui->listWidgetSafeOpen->count(); m_iSafeOpenRow++)
	{
		m_ui->listWidgetSafeOpen->setCurrentRow(m_iSafeOpenRow);
		Settings.Library.SafeExecuteTypes.append(m_ui->listWidgetSafeOpen->currentItem()->text());
	}
	Settings.Library.NeverShareTypes.clear();
	for (int m_iNeverShareRow = 0; m_iNeverShareRow < m_ui->listWidgetNeverShare->count(); m_iNeverShareRow++)
	{
		m_ui->listWidgetNeverShare->setCurrentRow(m_iNeverShareRow);
		Settings.Library.NeverShareTypes.append(m_ui->listWidgetNeverShare->currentItem()->text());
	}

	// Save Media Player Settings
	Settings.MediaPlayer.MediaHandler = m_ui->comboBoxChoosePlayer->currentIndex();
	Settings.MediaPlayer.CustomPlayerPath = m_ui->lineEditCustomPlayerPath->text();
	Settings.MediaPlayer.FileTypes.clear();
	for (int m_iPlayerFilesListRow = 0; m_iPlayerFilesListRow < m_ui->listWidgetPlayerFilesList->count(); m_iPlayerFilesListRow++)
	{
		m_ui->listWidgetPlayerFilesList->setCurrentRow(m_iPlayerFilesListRow);
		Settings.MediaPlayer.FileTypes.append(m_ui->listWidgetPlayerFilesList->currentItem()->text());
	}

	// Save Search Settings
	Settings.Search.ExpandSearchMatches = m_ui->checkBoxExpandMultiSorce->isChecked();
	Settings.Search.SwitchOnDownload = m_ui->checkBoxSwitchOnDownload->isChecked();
	Settings.Search.HighlightNew = m_ui->checkBoxHighlightNewSearchMatches->isChecked();

	// Save Chat Settings
	Settings.Chat.GnutellaChatEnable = m_ui->checkBoxGnutellaChatEnable->isChecked();
	Settings.Chat.EnableChatAllNetworks = m_ui->checkBoxEnableChatOther->isChecked();
	Settings.Chat.ConnectOnStartup = m_ui->checkBoxConnectIRCOnStart->isChecked();
	Settings.Chat.EnableFileTransfers = m_ui->checkBoxEnableIRCFileTransfers->isChecked();
	Settings.Chat.ShowTimestamp = m_ui->checkBoxIRCShowTimestamp->isChecked();
	Settings.Chat.AwayMessageIdleTime = m_ui->spinBoxChatIdleMessage->value();
	Settings.Chat.ScreenFont = fontTempChat;
	Settings.Chat.IrcServerName = m_ui->lineEditIRCServer->text();
	Settings.Chat.IrcServerPort = m_ui->spinBoxIRCPort->value();
	Settings.Chat.ColorChatBackground.setNamedColor(colorTempChatBackground.name());
	Settings.Chat.ColorNormalText.setNamedColor(colorTempNormalText.name());
	Settings.Chat.ColorServerMessagesText.setNamedColor(colorTempServerMessages.name());
	Settings.Chat.ColorTopicsText.setNamedColor(colorTempTopics.name());
	Settings.Chat.ColorRoomActionsText.setNamedColor(colorTempRoomActions.name());
	Settings.Chat.ColorNoticesText.setNamedColor(colorTempNotices.name());

	// Save Connection Settings
	if (m_ui->comboBoxInboundAddress->currentText() != tr("Automatic"))
	{
		Settings.Connection.InAddress = m_ui->comboBoxInboundAddress->currentText();
	} else {
		Settings.Connection.InAddress = "";
	}
	Settings.Connection.InBind = m_ui->checkBoxForceBindingAddress->isChecked();
	if (m_ui->comboBoxOutboundAddress->currentText() != tr("Automatic"))
	{
		Settings.Connection.OutAddress = m_ui->comboBoxOutboundAddress->currentText();
	} else {
		Settings.Connection.OutAddress = "";
	}
	Settings.Connection.Port = m_ui->spinBoxQuazaaPort->value();
	Settings.Connection.RandomPort = m_ui->checkBoxRandomPort->isChecked();
	Settings.Connection.CanAcceptIncoming = m_ui->comboBoxAcceptIncoming->currentIndex();
	Settings.Connection.TimeoutConnect = m_ui->spinBoxConnectionTimeout->value();
	Settings.Connection.TimeoutHandshake = m_ui->spinBoxHandshakeTimeout->value();

	// Save Web Settings
	Settings.Web.Magnet = m_ui->checkBoxMagnetLinks->isChecked();
	Settings.Web.Gnutella = m_ui->checkBoxGnutellaLinks->isChecked();
	Settings.Web.Ares = m_ui->checkBoxAresLinks->isChecked();
	Settings.Web.Torrent = m_ui->checkBoxBittorrentLinks->isChecked();
	Settings.Web.Piolet = m_ui->checkBoxPioletLinks->isChecked();
	Settings.Web.ED2K = m_ui->checkBoxEDonkey2kLinks->isChecked();
	Settings.Web.BrowserIntegration = m_ui->checkBoxManageWebDownloads->isChecked();
	Settings.Web.ManageDownloadTypes.clear();
	for (int m_iDownloadTypesRow = 0; m_iDownloadTypesRow < m_ui->listWidgetManageDownloadTypes->count(); m_iDownloadTypesRow++)
	{
		m_ui->listWidgetManageDownloadTypes->setCurrentRow(m_iDownloadTypesRow);
		Settings.Web.ManageDownloadTypes.append(m_ui->listWidgetManageDownloadTypes->currentItem()->text());
	}

	// Save Transfer Settings
	Settings.Transfers.RequireConnectedNetwork = m_ui->checkBoxOnlyDownloadConnectedNetworks->isChecked();
	Settings.Transfers.SimpleProgressBar = m_ui->checkBoxSimpleProgress->isChecked();
	Settings.Transfers.RatesUnit = m_ui->comboBoxRates->currentIndex();
	Settings.Transfers.BandwidthDownloads = m_ui->doubleSpinBoxDownloadSpeed->value();
	Settings.Transfers.BandwidthUploads = m_ui->doubleSpinBoxUploadSpeed->value();

	// Save Download Settings
	Settings.Downloads.ExpandDownloads = m_ui->checkBoxExpandDownloads->isChecked();
	Settings.Downloads.CompletePath = m_ui->lineEditSaveFolder->text();
	Settings.Downloads.IncompletePath = m_ui->lineEditTempFolder->text();
	Settings.Downloads.QueueLimit = m_ui->comboBoxQueLength->currentIndex();
	Settings.Downloads.MaxFiles = m_ui->spinBoxMaxFiles->value();
	Settings.Downloads.MaxTransfers = m_ui->spinBoxMaxTransfers->value();
	Settings.Downloads.MaxTransfersPerFile = m_ui->spinBoxTransfersPerFile->value();

	// Save Upload Settings
	Settings.Uploads.SharePartials = m_ui->checkBoxSharePartials->isChecked();
	Settings.Uploads.HubShareLimiting = m_ui->checkBoxSharingLimitHub->isChecked();
	Settings.Uploads.SharePreviews = m_ui->checkBoxSharePreviews->isChecked();
	Settings.Uploads.MaxPerHost = m_ui->spinBoxUniqueHostLimit->value();

	// Save Security Settings
	Settings.Security.ChatFilter = m_ui->checkBoxFilterChatSpam->isChecked();
	Settings.Security.ED2kChatFilter = m_ui->checkBoxFilterEDonkeyChatSpam->isChecked();
	Settings.Security.AllowProfileBrowse = m_ui->checkBoxAllowBrowseProfile->isChecked();
	Settings.Security.IrcFloodProtection = m_ui->checkBoxChatFloodProtection->isChecked();
	Settings.Security.IrcFloodLimit = m_ui->spinBoxChatFloodLimit->value();
	Settings.Security.RemoteEnable = m_ui->checkBoxEnableRemote->isChecked();
	Settings.Security.RemoteUsername = m_ui->lineEditRemoteUserName->text();
	Settings.Security.RemotePassword = m_ui->lineEditRemotePassword->text();
	Settings.Security.SearchIgnoreLocalIP = m_ui->checkBoxIgnoreLocalIP->isChecked();
	Settings.Security.EnableUPnP = m_ui->checkBoxEnableUPnP->isChecked();
	Settings.Security.AllowSharesBrowse = m_ui->checkBoxAllowBrowseShares->isChecked();
	Settings.Security.BlockedAgentUploadFilter.clear();
	for (int m_iUserAgentsRow = 0; m_iUserAgentsRow < m_ui->listWidgetUserAgents->count(); m_iUserAgentsRow++)
	{
		m_ui->listWidgetUserAgents->setCurrentRow(m_iUserAgentsRow);
		Settings.Security.BlockedAgentUploadFilter.append(m_ui->listWidgetUserAgents->currentItem()->text());
	}

	// Save Gnutella 2 Settings
	Settings.Gnutella2.Enable = m_ui->checkBoxConnectG2->isChecked();
	Settings.Gnutella2.ClientMode = m_ui->comboBoxG2Mode->currentIndex();
	Settings.Gnutella2.NumHubs = m_ui->spinBoxG2LeafToHub->value();
	Settings.Gnutella2.NumLeafs = m_ui->spinBoxG2HubToLeaf->value();
	Settings.Gnutella2.NumPeers = m_ui->spinBoxG2HubToHub->value();

	// Save Gnutella 1 Settings
	Settings.Gnutella1.Enable = m_ui->checkBoxConnectG1->isChecked();
	Settings.Gnutella1.NumHubs = m_ui->spinBoxG1LeafToUltrapeer->value();
	Settings.Gnutella1.NumLeafs = m_ui->spinBoxG1UltrapeerToLeaf->value();
	Settings.Gnutella1.NumPeers = m_ui->spinBoxG1UltrapeerToUltrapeer->value();

	// Save Gnutella Settings
	Settings.Gnutella.CompressLeaf2Hub = m_ui->checkBoxCompressionLeafToHub->isChecked();
	Settings.Gnutella.CompressHub2Leaf = m_ui->checkBoxCompressionHubToLeaf->isChecked();
	Settings.Gnutella.CompressHub2Hub = m_ui->checkBoxCompressionHubToHub->isChecked();

	// Save Ares Settings
	Settings.Ares.Enable = m_ui->checkBoxConnectAres->isChecked();

	// Save eDonkey 2k Settings
	Settings.eDonkey2k.Enable = m_ui->checkBoxConnectEDonkey2k->isChecked();
	Settings.eDonkey2k.EnableKad = m_ui->checkBoxConnectKAD->isChecked();
	Settings.eDonkey2k.SearchCachedServers = m_ui->checkBoxED2kSearchCahedServers->isChecked();
	Settings.eDonkey2k.MaxResults = m_ui->spinBoxED2kMaxResults->value();
	Settings.eDonkey2k.LearnNewServers = m_ui->checkBoxED2kUpdateServerList->isChecked();
	Settings.eDonkey2k.MaxClients = m_ui->spinBoxED2kMaxClients->value();
	Settings.eDonkey2k.MetAutoQuery = m_ui->checkBoxAutoQueryServerList->isChecked();
	Settings.eDonkey2k.ServerListURL = m_ui->lineEditEDonkeyServerListUrl->text();

	// Save Bittorrent Settings
	Settings.Bittorrent.UseSaveDialog = m_ui->checkBoxTorrentSaveDialog->isChecked();
	Settings.Bittorrent.StartPaused = m_ui->checkBoxTorrentsStartPaused->isChecked();
	Settings.Bittorrent.UseTemp = m_ui->checkBoxTorrentsUseTemp->isChecked();
	Settings.Bittorrent.Managed = m_ui->checkBoxManagedTorrent->isChecked();
	Settings.Bittorrent.Endgame = m_ui->checkBoxTorrentsEndgame->isChecked();
	Settings.Bittorrent.DownloadTorrents = m_ui->spinBoxTorrentsSimultaneous->value();
	Settings.Bittorrent.DownloadConnections = m_ui->spinBoxTorrentsClientConnections->value();
	Settings.Bittorrent.AutoClear = m_ui->checkBoxTorrentsClearDownloaded->isChecked();
	Settings.Bittorrent.ClearRatio = m_ui->spinBoxTorrentsRatioClear->value();
	Settings.Bittorrent.PreferBTSources = m_ui->checkBoxTorrentsPreferTorrent->isChecked();
	Settings.Bittorrent.UseKademlia = m_ui->checkBoxTorrentsUseKademlia->isChecked();
	Settings.Bittorrent.TorrentPath = m_ui->lineEditTorrentFolder->text();

	// Reset Apply Enabled To False
	Settings.saveSettings();
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

void DialogSettings::on_comboBoxAcceptIncoming_currentIndexChanged(int index)
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

void DialogSettings::on_checkBoxBittorrentLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxPioletLinks_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEDonkey2kLinks_clicked()
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
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_doubleSpinBoxDownloadSpeed_valueChanged(double )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_doubleSpinBoxUploadSpeed_valueChanged(double )
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

void DialogSettings::on_comboBoxG1Mode_currentIndexChanged(int index)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxG1LeafToUltrapeer_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxG1UltrapeerToLeaf_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_spinBoxG1UltrapeerToUltrapeer_valueChanged(int )
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxCompressionHubToLeaf_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxCompressionLeafToHub_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxCompressionHubToHub_clicked()
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
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxConnectG1_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxConnectAres_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxConnectEDonkey2k_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxG2Connect_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxG1Connect_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxAresConnect_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogSettings::on_checkBoxEDonkey2kConnect_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}
