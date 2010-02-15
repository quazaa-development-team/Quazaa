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
	m_ui->checkBoxSystemStart->setChecked(quazaaSettings.BasicStartWithSystem);
	m_ui->checkBoxAutoConnect->setChecked(quazaaSettings.BasicConnectOnStartup);
	m_ui->comboBoxCloseButton->setCurrentIndex(quazaaSettings.BasicCloseMode);
	m_ui->checkBoxMinimiseToTray->setChecked(quazaaSettings.BasicMinimizeToTray);
	m_ui->checkBoxSearchTooltips->setChecked(quazaaSettings.BasicSearchTips);
	m_ui->checkBoxLibraryTooltips->setChecked(quazaaSettings.BasicLibraryTips);
	m_ui->checkBoxDownloadTooltips->setChecked(quazaaSettings.BasicDownloadsTips);
	m_ui->checkBoxUploadTooltips->setChecked(quazaaSettings.BasicUploadsTips);
	m_ui->checkBoxNeighbourTooltips->setChecked(quazaaSettings.BasicNeighboursTips);
	m_ui->checkBoxMediaPlayerTooltips->setChecked(quazaaSettings.BasicMediaTips);
	m_ui->checkBoxChatTooltips->setChecked(quazaaSettings.BasicChatTips);
	m_ui->spinBoxToolTipDelay->setValue(quazaaSettings.BasicTipDelay);
	m_ui->horizontalSliderTransparency->setValue(quazaaSettings.BasicTipTransparency);
	m_ui->spinBoxDiskSpaceWarning->setValue(quazaaSettings.BasicDiskSpaceWarning);
	m_ui->spinBoxDiskSpaceStop->setValue(quazaaSettings.BasicDiskSpaceStop);

	// Load Parental Settings
	m_ui->checkBoxCensor->setChecked(quazaaSettings.ParentalChatAdultCensor);
	m_ui->checkBoxFilterAdult->setChecked(quazaaSettings.ParentalFilterAdultSearchResults);
	m_ui->listWidgetAdultFilter->addItems(quazaaSettings.ParentalAdultFilter);

	// Load Library Settings
	m_ui->checkBoxRememberViews->setChecked(quazaaSettings.LibraryRememberViews);
	m_ui->checkBoxWatchFolders->setChecked(quazaaSettings.LibraryWatchFolders);
	m_ui->checkBoxQuickHashing->setChecked(quazaaSettings.LibraryHighPriorityHashing);
	m_ui->checkBoxDisplayHashingProgress->setChecked(quazaaSettings.LibraryHashWindow);
	m_ui->checkBoxGhostFiles->setChecked(quazaaSettings.LibraryGhostFiles);
	m_ui->checkBoxSeriesDetection->setChecked(quazaaSettings.LibrarySmartSeriesDetection);
	m_ui->spinBoxRememberFiles->setValue(quazaaSettings.LibraryHistoryTotal);
	m_ui->spinBoxUpToDays->setValue(quazaaSettings.LibraryHistoryDays);
	m_ui->listWidgetSafeOpen->addItems(quazaaSettings.LibrarySafeExecuteTypes);
	m_ui->listWidgetNeverShare->addItems(quazaaSettings.LibraryNeverShareTypes);

	// Load Media Player Settings
	m_ui->comboBoxChoosePlayer->setCurrentIndex(quazaaSettings.MediaHandler);
	m_ui->lineEditCustomPlayerPath->setText(quazaaSettings.MediaCustomPlayerPath);
	m_ui->listWidgetPlayerFilesList->addItems(quazaaSettings.MediaFileTypes);

	// Load Search Settings
	m_ui->checkBoxExpandMultiSorce->setChecked(quazaaSettings.SearchExpandSearchMatches);
	m_ui->checkBoxSwitchOnDownload->setChecked(quazaaSettings.SearchSwitchOnDownload);
	m_ui->checkBoxHighlightNewSearchMatches->setChecked(quazaaSettings.SearchHighlightNew);

	// Load Chat Settings
	m_ui->checkBoxGnutellaChatEnable->setChecked(quazaaSettings.ChatGnutellaChatEnable);
	m_ui->checkBoxEnableChatOther->setChecked(quazaaSettings.ChatEnableChatAllNetworks);
	m_ui->checkBoxConnectIRCOnStart->setChecked(quazaaSettings.ChatConnectOnStartup);
	m_ui->checkBoxEnableIRCFileTransfers->setChecked(quazaaSettings.ChatEnableFileTransfers);
	m_ui->checkBoxIRCShowTimestamp->setChecked(quazaaSettings.ChatShowTimestamp);
	m_ui->spinBoxChatIdleMessage->setValue(quazaaSettings.ChatAwayMessageIdleTime);
	m_ui->lineEditIRCServer->setText(quazaaSettings.ChatIrcServerName);
	m_ui->spinBoxIRCPort->setValue(quazaaSettings.ChatIrcServerPort);
	m_ui->labelChatFontPreview->setText(quazaaSettings.ChatScreenFont.rawName());
	m_ui->labelChatFontPreview->setFont(quazaaSettings.ChatScreenFont);
	fontTempChat = quazaaSettings.ChatScreenFont;
	m_ui->toolButtonColorChatBackground->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.ChatColorChatBackground.name() + ";}");
	m_ui->frameIRCColors->setStyleSheet("background-color: " + quazaaSettings.ChatColorChatBackground.name() + ";");
	colorTempChatBackground.setNamedColor(quazaaSettings.ChatColorChatBackground.name());
	m_ui->toolButtonColorNormalText->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.ChatColorNormalText.name() + ";}");
	m_ui->labelNormalText->setFont(quazaaSettings.ChatScreenFont);
	m_ui->labelNormalText->setStyleSheet("color: " + quazaaSettings.ChatColorNormalText.name() + ";");
	colorTempNormalText.setNamedColor(quazaaSettings.ChatColorNormalText.name());
	m_ui->toolButtonColorServerMessages->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.ChatColorServerMessagesText.name() + ";}");
	m_ui->labelServerMessages->setFont(quazaaSettings.ChatScreenFont);
	m_ui->labelServerMessages->setStyleSheet("color: " + quazaaSettings.ChatColorServerMessagesText.name() + ";");
	colorTempServerMessages.setNamedColor(quazaaSettings.ChatColorServerMessagesText.name());
	m_ui->toolButtonColorTopics->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.ChatColorTopicsText.name() + ";}");
	m_ui->labelTopics->setFont(quazaaSettings.ChatScreenFont);
	m_ui->labelTopics->setStyleSheet("color: " + quazaaSettings.ChatColorTopicsText.name() + ";");
	colorTempTopics.setNamedColor(quazaaSettings.ChatColorTopicsText.name());
	m_ui->toolButtonColorRoomActions->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.ChatColorRoomActionsText.name() + ";}");
	m_ui->labelRoomActions->setFont(quazaaSettings.ChatScreenFont);
	m_ui->labelRoomActions->setStyleSheet("color: " + quazaaSettings.ChatColorRoomActionsText.name() + ";");
	colorTempRoomActions.setNamedColor(quazaaSettings.ChatColorRoomActionsText.name());
	m_ui->toolButtonColorNotices->setStyleSheet("QToolButton {border: 1px solid rgb(0, 0, 0); background-color: " + quazaaSettings.ChatColorNoticesText.name() + ";}");
	m_ui->labelNotices->setFont(quazaaSettings.ChatScreenFont);
	m_ui->labelNotices->setStyleSheet("color: " + quazaaSettings.ChatColorNoticesText.name() + ";");
	colorTempNotices.setNamedColor(quazaaSettings.ChatColorNoticesText.name());

	// Load Connection Settings
	if (quazaaSettings.ConnectionInAddress != "")
	{
		m_ui->comboBoxInboundAddress->addItem(quazaaSettings.ConnectionInAddress);
	}
	m_ui->checkBoxForceBindingAddress->setChecked(quazaaSettings.ConnectionInBind);
	if (quazaaSettings.ConnectionOutAddress != "")
	{
		m_ui->comboBoxOutboundAddress->addItem(quazaaSettings.ConnectionOutAddress);
	}
	m_ui->spinBoxQuazaaPort->setValue(quazaaSettings.ConnectionPort);
	m_ui->checkBoxRandomPort->setChecked(quazaaSettings.ConnectionRandomPort);
	m_ui->comboBoxAcceptIncoming->setCurrentIndex(quazaaSettings.ConnectionCanAcceptIncoming);
	m_ui->spinBoxConnectionTimeout->setValue(quazaaSettings.ConnectionTimeout);
	m_ui->spinBoxHandshakeTimeout->setValue(quazaaSettings.ConnectionTimeoutHandshake);

	// Load Web Settings
	m_ui->checkBoxMagnetLinks->setChecked(quazaaSettings.WebMagnet);
	m_ui->checkBoxGnutellaLinks->setChecked(quazaaSettings.WebGnutella);
	m_ui->checkBoxAresLinks->setChecked(quazaaSettings.WebAres);
	m_ui->checkBoxBitTorrentLinks->setChecked(quazaaSettings.WebTorrent);
	m_ui->checkBoxPioletLinks->setChecked(quazaaSettings.WebPiolet);
	m_ui->checkBoxEDonkeyLinks->setChecked(quazaaSettings.WebED2K);
	m_ui->checkBoxManageWebDownloads->setChecked(quazaaSettings.WebBrowserIntegration);
	m_ui->listWidgetManageDownloadTypes->addItems(quazaaSettings.WebManageDownloadTypes);

	// Load Transfer Settings
	m_ui->checkBoxOnlyDownloadConnectedNetworks->setChecked(quazaaSettings.TransfersRequireConnectedNetwork);
	m_ui->checkBoxSimpleProgress->setChecked(quazaaSettings.TransfersSimpleProgressBar);
	m_ui->comboBoxRates->setCurrentIndex(quazaaSettings.TransfersRatesUnit);
	m_ui->doubleSpinBoxDownloadSpeed->setValue(quazaaSettings.TransfersBandwidthDownloads);
	m_ui->doubleSpinBoxUploadSpeed->setValue(quazaaSettings.TransfersBandwidthUploads);

	// Load Download Settings
	m_ui->checkBoxExpandDownloads->setChecked(quazaaSettings.DownloadsExpandDownloads);
	m_ui->lineEditSaveFolder->setText(quazaaSettings.DownloadsCompletePath);
	m_ui->lineEditTempFolder->setText(quazaaSettings.DownloadsIncompletePath);
	m_ui->comboBoxQueLength->setCurrentIndex(quazaaSettings.DownloadsQueueLimit);
	m_ui->spinBoxMaxFiles->setValue(quazaaSettings.DownloadsMaxFiles);
	m_ui->spinBoxMaxTransfers->setValue(quazaaSettings.DownloadsMaxTransfers);
	m_ui->spinBoxTransfersPerFile->setValue(quazaaSettings.DownloadsMaxTransfersPerFile);

	// Load Upload Settings
	m_ui->checkBoxSharePartials->setChecked(quazaaSettings.UploadsSharePartials);
	m_ui->checkBoxSharingLimitHub->setChecked(quazaaSettings.UploadsHubShareLimiting);
	m_ui->checkBoxSharePreviews->setChecked(quazaaSettings.UploadsSharePreviews);
	m_ui->spinBoxUniqueHostLimit->setValue(quazaaSettings.UploadsMaxPerHost);

	// Load Security Settings
	m_ui->checkBoxFilterChatSpam->setChecked(quazaaSettings.SecurityChatFilter);
	m_ui->checkBoxFilterEDonkeyChatSpam->setChecked(quazaaSettings.SecurityED2kChatFilter);
	m_ui->checkBoxAllowBrowseProfile->setChecked(quazaaSettings.SecurityAllowProfileBrowse);
	m_ui->checkBoxChatFloodProtection->setChecked(quazaaSettings.SecurityIrcFloodProtection);
	m_ui->spinBoxChatFloodLimit->setValue(quazaaSettings.SecurityIrcFloodLimit);
	m_ui->checkBoxEnableRemote->setChecked(quazaaSettings.SecurityRemoteEnable);
	m_ui->lineEditRemoteUserName->setText(quazaaSettings.SecurityRemoteUsername);
	m_ui->lineEditRemotePassword->setText(quazaaSettings.SecurityRemotePassword);
	m_ui->checkBoxIgnoreLocalIP->setChecked(quazaaSettings.SecuritySearchIgnoreLocalIP);
	m_ui->checkBoxEnableUPnP->setChecked(quazaaSettings.SecurityEnableUPnP);
	m_ui->checkBoxAllowBrowseShares->setChecked(quazaaSettings.SecurityAllowSharesBrowse);
	m_ui->listWidgetUserAgents->addItems(quazaaSettings.SecurityBlockedAgentUploadFilter);

	// Load Gnutella 2 Settings
	m_ui->checkBoxConnectG2->setChecked(quazaaSettings.Gnutella2Enable);
	m_ui->comboBoxG2Mode->setCurrentIndex(quazaaSettings.Gnutella2ClientMode);
	m_ui->spinBoxG2LeafToHub->setValue(quazaaSettings.Gnutella2NumHubs);
	m_ui->spinBoxG2HubToLeaf->setValue(quazaaSettings.Gnutella2NumLeafs);
	m_ui->spinBoxG2HubToHub->setValue(quazaaSettings.Gnutella2NumPeers);

	// Load Gnutella 1 Settings
	m_ui->checkBoxConnectG1->setChecked(quazaaSettings.Gnutella1Enable);
	m_ui->spinBoxG1LeafToUltrapeer->setValue(quazaaSettings.Gnutella1NumHubs);
	m_ui->spinBoxG1UltrapeerToLeaf->setValue(quazaaSettings.Gnutella1NumLeafs);
	m_ui->spinBoxG1UltrapeerToUltrapeer->setValue(quazaaSettings.Gnutella1NumPeers);

	// Load Gnutella Settings
	m_ui->checkBoxCompressionLeafToHub->setChecked(quazaaSettings.GnutellaCompressLeaf2Hub);
	m_ui->checkBoxCompressionHubToLeaf->setChecked(quazaaSettings.GnutellaCompressHub2Leaf);
	m_ui->checkBoxCompressionHubToHub->setChecked(quazaaSettings.GnutellaCompressHub2Hub);

	// Load Ares Settings
	m_ui->checkBoxConnectAres->setChecked(quazaaSettings.AresEnable);
	m_ui->checkBoxConnectAres->setChecked(quazaaSettings.AresEnable);

	// Load eDonkey 2k Settings
	m_ui->checkBoxConnectEDonkey->setChecked(quazaaSettings.EDonkeyEnable);
	m_ui->checkBoxConnectKAD->setChecked(quazaaSettings.EDonkeyEnableKad);
	m_ui->checkBoxED2kSearchCahedServers->setChecked(quazaaSettings.EDonkeySearchCachedServers);
	m_ui->spinBoxED2kMaxResults->setValue(quazaaSettings.EDonkeyMaxResults);
	m_ui->checkBoxED2kUpdateServerList->setChecked(quazaaSettings.EDonkeyLearnNewServers);
	m_ui->spinBoxED2kMaxClients->setValue(quazaaSettings.EDonkeyMaxClients);
	m_ui->checkBoxAutoQueryServerList->setChecked(quazaaSettings.EDonkeyMetAutoQuery);
	m_ui->lineEditEDonkeyServerListUrl->setText(quazaaSettings.EDonkeyServerListURL);

	// Load BitTorrent Settings
	m_ui->checkBoxTorrentSaveDialog->setChecked(quazaaSettings.BitTorrentUseSaveDialog);
	m_ui->checkBoxTorrentsStartPaused->setChecked(quazaaSettings.BitTorrentStartPaused);
	m_ui->checkBoxTorrentsUseTemp->setChecked(quazaaSettings.BitTorrentUseTemp);
	m_ui->checkBoxManagedTorrent->setChecked(quazaaSettings.BitTorrentManaged);
	m_ui->checkBoxTorrentsEndgame->setChecked(quazaaSettings.BitTorrentEndgame);
	m_ui->spinBoxTorrentsSimultaneous->setValue(quazaaSettings.BitTorrentDownloadTorrents);
	m_ui->spinBoxTorrentsClientConnections->setValue(quazaaSettings.BitTorrentDownloadConnections);
	m_ui->checkBoxTorrentsClearDownloaded->setChecked(quazaaSettings.BitTorrentAutoClear);
	m_ui->spinBoxTorrentsRatioClear->setValue(quazaaSettings.BitTorrentClearRatio);
	m_ui->checkBoxTorrentsPreferTorrent->setChecked(quazaaSettings.BitTorrentPreferBTSources);
	m_ui->checkBoxTorrentsUseKademlia->setChecked(quazaaSettings.BitTorrentUseKademlia);
	m_ui->lineEditTorrentFolder->setText(quazaaSettings.BitTorrentTorrentPath);

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
	QSkinDialog *dlgSkinProfile = new QSkinDialog(false, true, false);
	DialogProfile *dlgProfile = new DialogProfile(this);

	dlgSkinProfile->addChildWidget(dlgProfile);

	connect(dlgProfile, SIGNAL(closed()), dlgSkinProfile, SLOT(close()));
	dlgSkinProfile->show();
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
	quazaaSettings.BasicStartWithSystem = m_ui->checkBoxSystemStart->isChecked();
	quazaaSettings.BasicConnectOnStartup = m_ui->checkBoxAutoConnect->isChecked();
	quazaaSettings.BasicCloseMode = m_ui->comboBoxCloseButton->currentIndex();
	quazaaSettings.BasicMinimizeToTray = m_ui->checkBoxMinimiseToTray->isChecked();
	quazaaSettings.BasicSearchTips = m_ui->checkBoxSearchTooltips->isChecked();
	quazaaSettings.BasicLibraryTips = m_ui->checkBoxLibraryTooltips->isChecked();
	quazaaSettings.BasicDownloadsTips = m_ui->checkBoxDownloadTooltips->isChecked();
	quazaaSettings.BasicUploadsTips = m_ui->checkBoxUploadTooltips->isChecked();
	quazaaSettings.BasicNeighboursTips = m_ui->checkBoxNeighbourTooltips->isChecked();
	quazaaSettings.BasicMediaTips = m_ui->checkBoxMediaPlayerTooltips->isChecked();
	quazaaSettings.BasicChatTips = m_ui->checkBoxChatTooltips->isChecked();
	quazaaSettings.BasicTipDelay = m_ui->spinBoxToolTipDelay->value();
	quazaaSettings.BasicTipTransparency = m_ui->horizontalSliderTransparency->value();
	quazaaSettings.BasicDiskSpaceWarning = m_ui->spinBoxDiskSpaceWarning->value();
	quazaaSettings.BasicDiskSpaceStop = m_ui->spinBoxDiskSpaceStop->value();

	// Save Parental Settings
	quazaaSettings.ParentalChatAdultCensor = m_ui->checkBoxCensor->isChecked();
	quazaaSettings.ParentalFilterAdultSearchResults = m_ui->checkBoxFilterAdult->isChecked();
	quazaaSettings.ParentalAdultFilter.clear();
	for (int m_iAdultFilterRow = 0; m_iAdultFilterRow < m_ui->listWidgetAdultFilter->count(); m_iAdultFilterRow++)
	{
		m_ui->listWidgetAdultFilter->setCurrentRow(m_iAdultFilterRow);
		quazaaSettings.ParentalAdultFilter.append(m_ui->listWidgetAdultFilter->currentItem()->text());
	}

	// Save Library Settings
	quazaaSettings.LibraryRememberViews = m_ui->checkBoxRememberViews->isChecked();
	quazaaSettings.LibraryWatchFolders = m_ui->checkBoxWatchFolders->isChecked();
	quazaaSettings.LibraryHighPriorityHashing = m_ui->checkBoxQuickHashing->isChecked();
	quazaaSettings.LibraryHashWindow = m_ui->checkBoxDisplayHashingProgress->isChecked();
	quazaaSettings.LibraryGhostFiles = m_ui->checkBoxGhostFiles->isChecked();
	quazaaSettings.LibrarySmartSeriesDetection = m_ui->checkBoxSeriesDetection->isChecked();
	quazaaSettings.LibraryHistoryTotal = m_ui->spinBoxRememberFiles->value();
	quazaaSettings.LibraryHistoryDays = m_ui->spinBoxUpToDays->value();
	quazaaSettings.LibrarySafeExecuteTypes.clear();
	for (int m_iSafeOpenRow = 0; m_iSafeOpenRow < m_ui->listWidgetSafeOpen->count(); m_iSafeOpenRow++)
	{
		m_ui->listWidgetSafeOpen->setCurrentRow(m_iSafeOpenRow);
		quazaaSettings.LibrarySafeExecuteTypes.append(m_ui->listWidgetSafeOpen->currentItem()->text());
	}
	quazaaSettings.LibraryNeverShareTypes.clear();
	for (int m_iNeverShareRow = 0; m_iNeverShareRow < m_ui->listWidgetNeverShare->count(); m_iNeverShareRow++)
	{
		m_ui->listWidgetNeverShare->setCurrentRow(m_iNeverShareRow);
		quazaaSettings.LibraryNeverShareTypes.append(m_ui->listWidgetNeverShare->currentItem()->text());
	}

	// Save Media Player Settings
	quazaaSettings.MediaHandler = m_ui->comboBoxChoosePlayer->currentIndex();
	quazaaSettings.MediaCustomPlayerPath = m_ui->lineEditCustomPlayerPath->text();
	quazaaSettings.MediaFileTypes.clear();
	for (int m_iPlayerFilesListRow = 0; m_iPlayerFilesListRow < m_ui->listWidgetPlayerFilesList->count(); m_iPlayerFilesListRow++)
	{
		m_ui->listWidgetPlayerFilesList->setCurrentRow(m_iPlayerFilesListRow);
		quazaaSettings.MediaFileTypes.append(m_ui->listWidgetPlayerFilesList->currentItem()->text());
	}

	// Save Search Settings
	quazaaSettings.SearchExpandSearchMatches = m_ui->checkBoxExpandMultiSorce->isChecked();
	quazaaSettings.SearchSwitchOnDownload = m_ui->checkBoxSwitchOnDownload->isChecked();
	quazaaSettings.SearchHighlightNew = m_ui->checkBoxHighlightNewSearchMatches->isChecked();

	// Save Chat Settings
	quazaaSettings.ChatGnutellaChatEnable = m_ui->checkBoxGnutellaChatEnable->isChecked();
	quazaaSettings.ChatEnableChatAllNetworks = m_ui->checkBoxEnableChatOther->isChecked();
	quazaaSettings.ChatConnectOnStartup = m_ui->checkBoxConnectIRCOnStart->isChecked();
	quazaaSettings.ChatEnableFileTransfers = m_ui->checkBoxEnableIRCFileTransfers->isChecked();
	quazaaSettings.ChatShowTimestamp = m_ui->checkBoxIRCShowTimestamp->isChecked();
	quazaaSettings.ChatAwayMessageIdleTime = m_ui->spinBoxChatIdleMessage->value();
	quazaaSettings.ChatScreenFont = fontTempChat;
	quazaaSettings.ChatIrcServerName = m_ui->lineEditIRCServer->text();
	quazaaSettings.ChatIrcServerPort = m_ui->spinBoxIRCPort->value();
	quazaaSettings.ChatColorChatBackground.setNamedColor(colorTempChatBackground.name());
	quazaaSettings.ChatColorNormalText.setNamedColor(colorTempNormalText.name());
	quazaaSettings.ChatColorServerMessagesText.setNamedColor(colorTempServerMessages.name());
	quazaaSettings.ChatColorTopicsText.setNamedColor(colorTempTopics.name());
	quazaaSettings.ChatColorRoomActionsText.setNamedColor(colorTempRoomActions.name());
	quazaaSettings.ChatColorNoticesText.setNamedColor(colorTempNotices.name());

	// Save Connection Settings
	if (m_ui->comboBoxInboundAddress->currentText() != tr("Automatic"))
	{
		quazaaSettings.ConnectionInAddress = m_ui->comboBoxInboundAddress->currentText();
	} else {
		quazaaSettings.ConnectionInAddress = "";
	}
	quazaaSettings.ConnectionInBind = m_ui->checkBoxForceBindingAddress->isChecked();
	if (m_ui->comboBoxOutboundAddress->currentText() != tr("Automatic"))
	{
		quazaaSettings.ConnectionOutAddress = m_ui->comboBoxOutboundAddress->currentText();
	} else {
		quazaaSettings.ConnectionOutAddress = "";
	}
	quazaaSettings.ConnectionPort = m_ui->spinBoxQuazaaPort->value();
	quazaaSettings.ConnectionRandomPort = m_ui->checkBoxRandomPort->isChecked();
	quazaaSettings.ConnectionCanAcceptIncoming = m_ui->comboBoxAcceptIncoming->currentIndex();
	quazaaSettings.ConnectionTimeout = m_ui->spinBoxConnectionTimeout->value();
	quazaaSettings.ConnectionTimeoutHandshake = m_ui->spinBoxHandshakeTimeout->value();

	// Save Web Settings
	quazaaSettings.WebMagnet = m_ui->checkBoxMagnetLinks->isChecked();
	quazaaSettings.WebGnutella = m_ui->checkBoxGnutellaLinks->isChecked();
	quazaaSettings.WebAres = m_ui->checkBoxAresLinks->isChecked();
	quazaaSettings.WebTorrent = m_ui->checkBoxBitTorrentLinks->isChecked();
	quazaaSettings.WebPiolet = m_ui->checkBoxPioletLinks->isChecked();
	quazaaSettings.WebED2K = m_ui->checkBoxEDonkeyLinks->isChecked();
	quazaaSettings.WebBrowserIntegration = m_ui->checkBoxManageWebDownloads->isChecked();
	quazaaSettings.WebManageDownloadTypes.clear();
	for (int m_iDownloadTypesRow = 0; m_iDownloadTypesRow < m_ui->listWidgetManageDownloadTypes->count(); m_iDownloadTypesRow++)
	{
		m_ui->listWidgetManageDownloadTypes->setCurrentRow(m_iDownloadTypesRow);
		quazaaSettings.WebManageDownloadTypes.append(m_ui->listWidgetManageDownloadTypes->currentItem()->text());
	}

	// Save Transfer Settings
	quazaaSettings.TransfersRequireConnectedNetwork = m_ui->checkBoxOnlyDownloadConnectedNetworks->isChecked();
	quazaaSettings.TransfersSimpleProgressBar = m_ui->checkBoxSimpleProgress->isChecked();
	quazaaSettings.TransfersRatesUnit = m_ui->comboBoxRates->currentIndex();
	quazaaSettings.TransfersBandwidthDownloads = m_ui->doubleSpinBoxDownloadSpeed->value();
	quazaaSettings.TransfersBandwidthUploads = m_ui->doubleSpinBoxUploadSpeed->value();

	// Save Download Settings
	quazaaSettings.DownloadsExpandDownloads = m_ui->checkBoxExpandDownloads->isChecked();
	quazaaSettings.DownloadsCompletePath = m_ui->lineEditSaveFolder->text();
	quazaaSettings.DownloadsIncompletePath = m_ui->lineEditTempFolder->text();
	quazaaSettings.DownloadsQueueLimit = m_ui->comboBoxQueLength->currentIndex();
	quazaaSettings.DownloadsMaxFiles = m_ui->spinBoxMaxFiles->value();
	quazaaSettings.DownloadsMaxTransfers = m_ui->spinBoxMaxTransfers->value();
	quazaaSettings.DownloadsMaxTransfersPerFile = m_ui->spinBoxTransfersPerFile->value();

	// Save Upload Settings
	quazaaSettings.UploadsSharePartials = m_ui->checkBoxSharePartials->isChecked();
	quazaaSettings.UploadsHubShareLimiting = m_ui->checkBoxSharingLimitHub->isChecked();
	quazaaSettings.UploadsSharePreviews = m_ui->checkBoxSharePreviews->isChecked();
	quazaaSettings.UploadsMaxPerHost = m_ui->spinBoxUniqueHostLimit->value();

	// Save Security Settings
	quazaaSettings.SecurityChatFilter = m_ui->checkBoxFilterChatSpam->isChecked();
	quazaaSettings.SecurityED2kChatFilter = m_ui->checkBoxFilterEDonkeyChatSpam->isChecked();
	quazaaSettings.SecurityAllowProfileBrowse = m_ui->checkBoxAllowBrowseProfile->isChecked();
	quazaaSettings.SecurityIrcFloodProtection = m_ui->checkBoxChatFloodProtection->isChecked();
	quazaaSettings.SecurityIrcFloodLimit = m_ui->spinBoxChatFloodLimit->value();
	quazaaSettings.SecurityRemoteEnable = m_ui->checkBoxEnableRemote->isChecked();
	quazaaSettings.SecurityRemoteUsername = m_ui->lineEditRemoteUserName->text();
	quazaaSettings.SecurityRemotePassword = m_ui->lineEditRemotePassword->text();
	quazaaSettings.SecuritySearchIgnoreLocalIP = m_ui->checkBoxIgnoreLocalIP->isChecked();
	quazaaSettings.SecurityEnableUPnP = m_ui->checkBoxEnableUPnP->isChecked();
	quazaaSettings.SecurityAllowSharesBrowse = m_ui->checkBoxAllowBrowseShares->isChecked();
	quazaaSettings.SecurityBlockedAgentUploadFilter.clear();
	for (int m_iUserAgentsRow = 0; m_iUserAgentsRow < m_ui->listWidgetUserAgents->count(); m_iUserAgentsRow++)
	{
		m_ui->listWidgetUserAgents->setCurrentRow(m_iUserAgentsRow);
		quazaaSettings.SecurityBlockedAgentUploadFilter.append(m_ui->listWidgetUserAgents->currentItem()->text());
	}

	// Save Gnutella 2 Settings
	quazaaSettings.Gnutella2Enable = m_ui->checkBoxConnectG2->isChecked();
	quazaaSettings.Gnutella2ClientMode = m_ui->comboBoxG2Mode->currentIndex();
	quazaaSettings.Gnutella2NumHubs = m_ui->spinBoxG2LeafToHub->value();
	quazaaSettings.Gnutella2NumLeafs = m_ui->spinBoxG2HubToLeaf->value();
	quazaaSettings.Gnutella2NumPeers = m_ui->spinBoxG2HubToHub->value();

	// Save Gnutella 1 Settings
	quazaaSettings.Gnutella1Enable = m_ui->checkBoxConnectG1->isChecked();
	quazaaSettings.Gnutella1NumHubs = m_ui->spinBoxG1LeafToUltrapeer->value();
	quazaaSettings.Gnutella1NumLeafs = m_ui->spinBoxG1UltrapeerToLeaf->value();
	quazaaSettings.Gnutella1NumPeers = m_ui->spinBoxG1UltrapeerToUltrapeer->value();

	// Save Gnutella Settings
	quazaaSettings.GnutellaCompressLeaf2Hub = m_ui->checkBoxCompressionLeafToHub->isChecked();
	quazaaSettings.GnutellaCompressHub2Leaf = m_ui->checkBoxCompressionHubToLeaf->isChecked();
	quazaaSettings.GnutellaCompressHub2Hub = m_ui->checkBoxCompressionHubToHub->isChecked();

	// Save Ares Settings
	quazaaSettings.AresEnable = m_ui->checkBoxConnectAres->isChecked();

	// Save eDonkey 2k Settings
	quazaaSettings.EDonkeyEnable = m_ui->checkBoxConnectEDonkey->isChecked();
	quazaaSettings.EDonkeyEnableKad = m_ui->checkBoxConnectKAD->isChecked();
	quazaaSettings.EDonkeySearchCachedServers = m_ui->checkBoxED2kSearchCahedServers->isChecked();
	quazaaSettings.EDonkeyMaxResults = m_ui->spinBoxED2kMaxResults->value();
	quazaaSettings.EDonkeyLearnNewServers = m_ui->checkBoxED2kUpdateServerList->isChecked();
	quazaaSettings.EDonkeyMaxClients = m_ui->spinBoxED2kMaxClients->value();
	quazaaSettings.EDonkeyMetAutoQuery = m_ui->checkBoxAutoQueryServerList->isChecked();
	quazaaSettings.EDonkeyServerListURL = m_ui->lineEditEDonkeyServerListUrl->text();

	// Save BitTorrent Settings
	quazaaSettings.BitTorrentUseSaveDialog = m_ui->checkBoxTorrentSaveDialog->isChecked();
	quazaaSettings.BitTorrentStartPaused = m_ui->checkBoxTorrentsStartPaused->isChecked();
	quazaaSettings.BitTorrentUseTemp = m_ui->checkBoxTorrentsUseTemp->isChecked();
	quazaaSettings.BitTorrentManaged = m_ui->checkBoxManagedTorrent->isChecked();
	quazaaSettings.BitTorrentEndgame = m_ui->checkBoxTorrentsEndgame->isChecked();
	quazaaSettings.BitTorrentDownloadTorrents = m_ui->spinBoxTorrentsSimultaneous->value();
	quazaaSettings.BitTorrentDownloadConnections = m_ui->spinBoxTorrentsClientConnections->value();
	quazaaSettings.BitTorrentAutoClear = m_ui->checkBoxTorrentsClearDownloaded->isChecked();
	quazaaSettings.BitTorrentClearRatio = m_ui->spinBoxTorrentsRatioClear->value();
	quazaaSettings.BitTorrentPreferBTSources = m_ui->checkBoxTorrentsPreferTorrent->isChecked();
	quazaaSettings.BitTorrentUseKademlia = m_ui->checkBoxTorrentsUseKademlia->isChecked();
	quazaaSettings.BitTorrentTorrentPath = m_ui->lineEditTorrentFolder->text();

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

void DialogSettings::on_checkBoxConnectEDonkey_toggled(bool checked)
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

void DialogSettings::on_checkBoxEDonkeyConnect_toggled(bool checked)
{
	m_ui->pushButtonApply->setEnabled(true);
}
