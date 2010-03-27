//
// quazaasettings.cpp
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

#include "quazaasettings.h"
#include "quazaaglobals.h"
#include <QSettings>
#include <QUuid>
#include <QDesktopServices>

QuazaaSettings quazaaSettings;

QuazaaSettings::QuazaaSettings()
{
}

void QuazaaSettings::saveSettings()
{
	QSettings m_qSettings(quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName());

	m_qSettings.beginGroup("Live");
	m_qSettings.setValue("AdultWarning", quazaaSettings.LiveAdultWarning);
	m_qSettings.setValue("AutoClose", quazaaSettings.LiveAutoClose);
	m_qSettings.setValue("BandwidthScale", quazaaSettings.LiveBandwidthScale);
	m_qSettings.setValue("DefaultED2KServersLoaded", quazaaSettings.LiveDefaultED2KServersLoaded);
	m_qSettings.setValue("DiskSpaceStop", quazaaSettings.LiveDiskSpaceStop);
	m_qSettings.setValue("DiskSpaceWarning", quazaaSettings.LiveDiskSpaceWarning);
	m_qSettings.setValue("DiskWriteWarning", quazaaSettings.LiveDiskWriteWarning);
	m_qSettings.setValue("DonkeyServerWarning", quazaaSettings.LiveDonkeyServerWarning);
	m_qSettings.setValue("LastDuplicateHash", quazaaSettings.LiveLastDuplicateHash);
	m_qSettings.setValue("MaliciousWarning", quazaaSettings.LiveMaliciousWarning);
	m_qSettings.setValue("QueueLimitWarning", quazaaSettings.LiveQueueLimitWarning);
	m_qSettings.setValue("UploadLimitWarning", quazaaSettings.LiveUploadLimitWarning);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Basic");
	m_qSettings.setValue("ChatTips", quazaaSettings.BasicChatTips);
	m_qSettings.setValue("CloseMode", quazaaSettings.BasicCloseMode);
	m_qSettings.setValue("ConnectOnStartup", quazaaSettings.BasicConnectOnStartup);
	m_qSettings.setValue("DiskSpaceStop", quazaaSettings.BasicDiskSpaceStop);
	m_qSettings.setValue("DiskSpaceWarning", quazaaSettings.BasicDiskSpaceWarning);
	m_qSettings.setValue("DownloadsTips", quazaaSettings.BasicDownloadsTips);
	m_qSettings.setValue("LibraryTips", quazaaSettings.BasicLibraryTips);
	m_qSettings.setValue("MediaTips", quazaaSettings.BasicMediaTips);
	m_qSettings.setValue("MinimizeToTray", quazaaSettings.BasicMinimizeToTray);
	m_qSettings.setValue("NeighboursTips", quazaaSettings.BasicNeighboursTips);
	m_qSettings.setValue("SearchTips", quazaaSettings.BasicSearchTips);
	m_qSettings.setValue("StartWithSystem", quazaaSettings.BasicStartWithSystem);
	m_qSettings.setValue("TipDelay", quazaaSettings.BasicTipDelay);
	m_qSettings.setValue("TipLowResMode", quazaaSettings.BasicTipLowResMode);
	m_qSettings.setValue("TipTransparency", quazaaSettings.BasicTipTransparency);
	m_qSettings.setValue("UploadsTips", quazaaSettings.BasicUploadsTips);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Parental");
	m_qSettings.setValue("AdultFilter", quazaaSettings.ParentalAdultFilter);
	m_qSettings.setValue("ChatAdultCensor", quazaaSettings.ParentalChatAdultCensor);
	m_qSettings.setValue("FilterAdultSearchResults", quazaaSettings.ParentalFilterAdultSearchResults);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Logging");
	m_qSettings.setValue("DebugLog", quazaaSettings.LoggingDebugLog);
	m_qSettings.setValue("LogLevel", quazaaSettings.LoggingLogLevel);
	m_qSettings.setValue("LogShowTimestamp", quazaaSettings.LoggingLogShowTimestamp);
	m_qSettings.setValue("MaxDebugLogSize", quazaaSettings.LoggingMaxDebugLogSize);
	m_qSettings.setValue("SearchLog", quazaaSettings.LoggingSearchLog);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Library");
	m_qSettings.setValue("FilterURI", quazaaSettings.LibraryFilterURI);
	m_qSettings.setValue("GhostFiles", quazaaSettings.LibraryGhostFiles);
	m_qSettings.setValue("HashWindow", quazaaSettings.LibraryHashWindow);
	m_qSettings.setValue("HighPriorityHashing", quazaaSettings.LibraryHighPriorityHashing);
	m_qSettings.setValue("HighPriorityHashingSpeed", quazaaSettings.LibraryHighPriorityHashingSpeed);
	m_qSettings.setValue("HistoryDays", quazaaSettings.LibraryHistoryDays);
	m_qSettings.setValue("HistoryTotal", quazaaSettings.LibraryHistoryTotal);
	m_qSettings.setValue("LowPriorityHashingSpeed", quazaaSettings.LibraryLowPriorityHashingSpeed);
	m_qSettings.setValue("NeverShareTypes", quazaaSettings.LibraryNeverShareTypes);
	m_qSettings.setValue("PartialMatch", quazaaSettings.LibraryPartialMatch);
	m_qSettings.setValue("PreferAPETags", quazaaSettings.LibraryPreferAPETags);
	m_qSettings.setValue("QueryRouteSize", quazaaSettings.LibraryQueryRouteSize);
	m_qSettings.setValue("RememberViews", quazaaSettings.LibraryRememberViews);
	m_qSettings.setValue("SafeExecuteTypes", quazaaSettings.LibrarySafeExecuteTypes);
	m_qSettings.setValue("ScanAPE", quazaaSettings.LibraryScanAPE);
	m_qSettings.setValue("ScanASF", quazaaSettings.LibraryScanASF);
	m_qSettings.setValue("ScanAVI", quazaaSettings.LibraryScanAVI);
	m_qSettings.setValue("ScanCHM", quazaaSettings.LibraryScanCHM);
	m_qSettings.setValue("ScanEXE", quazaaSettings.LibraryScanEXE);
	m_qSettings.setValue("ScanImage", quazaaSettings.LibraryScanImage);
	m_qSettings.setValue("ScanMP3", quazaaSettings.LibraryScanMP3);
	m_qSettings.setValue("ScanMPC", quazaaSettings.LibraryScanMPC);
	m_qSettings.setValue("ScanMPEG", quazaaSettings.LibraryScanMPEG);
	m_qSettings.setValue("ScanMSI", quazaaSettings.LibraryScanMSI);
	m_qSettings.setValue("ScanOGG", quazaaSettings.LibraryScanOGG);
	m_qSettings.setValue("ScanPDF", quazaaSettings.LibraryScanPDF);
	m_qSettings.setValue("SchemaURI", quazaaSettings.LibrarySchemaURI);
	quazaaSettings.LibraryShares.removeDuplicates();
	m_qSettings.setValue("Shares", quazaaSettings.LibraryShares);
	m_qSettings.setValue("ShowCoverArt", quazaaSettings.LibraryShowCoverArt);
	m_qSettings.setValue("SmartSeriesDetection", quazaaSettings.LibrarySmartSeriesDetection);
	m_qSettings.setValue("SourceExpire", quazaaSettings.LibrarySourceExpire);
	m_qSettings.setValue("SourceMesh", quazaaSettings.LibrarySourceMesh);
	m_qSettings.setValue("ThumbSize", quazaaSettings.LibraryThumbSize);
	m_qSettings.setValue("TigerHeight", quazaaSettings.LibraryTigerHeight);
	m_qSettings.setValue("TreeSize", quazaaSettings.LibraryTreeSize);
	m_qSettings.setValue("WatchFolders", quazaaSettings.LibraryWatchFolders);
	m_qSettings.setValue("WatchFoldersTimeout", quazaaSettings.LibraryWatchFoldersTimeout);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("MediaPlayer");
	m_qSettings.setValue("Aspect", quazaaSettings.MediaAspect);
	m_qSettings.setValue("AudioVisualPlugin", quazaaSettings.MediaAudioVisualPlugin);
	m_qSettings.setValue("CustomPlayerPath", quazaaSettings.MediaCustomPlayerPath);
	m_qSettings.setValue("MediaHandler", quazaaSettings.MediaHandler);
	m_qSettings.setValue("FileTypes", quazaaSettings.MediaFileTypes);
	m_qSettings.setValue("ListVisible", quazaaSettings.MediaListVisible);
	m_qSettings.setValue("Mute", quazaaSettings.MediaMute);
	m_qSettings.setValue("OpenPath", quazaaSettings.MediaOpenPath);
	m_qSettings.setValue("Playlist", quazaaSettings.MediaPlaylist);
	m_qSettings.setValue("Shuffle", quazaaSettings.MediaShuffle);
	m_qSettings.setValue("Repeat", quazaaSettings.MediaRepeat);
	m_qSettings.setValue("StatusVisible", quazaaSettings.MediaStatusVisible);
	m_qSettings.setValue("Volume", quazaaSettings.MediaVolume);
	m_qSettings.setValue("Zoom", quazaaSettings.MediaZoom);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Search");
	m_qSettings.setValue("BlankSchemaURI", quazaaSettings.SearchBlankSchemaURI);
	m_qSettings.setValue("BrowseTreeSize", quazaaSettings.SearchBrowseTreeSize);
	m_qSettings.setValue("ClearPrevious", quazaaSettings.SearchClearPrevious);
	m_qSettings.setValue("ExpandSearchMatches", quazaaSettings.SearchExpandSearchMatches);
	m_qSettings.setValue("FilterMask", quazaaSettings.SearchFilterMask);
	m_qSettings.setValue("HighlightNew", quazaaSettings.SearchHighlightNew);
	m_qSettings.setValue("LastSchemaURI", quazaaSettings.SearchLastSchemaURI);
	m_qSettings.setValue("MaxPreviewLength", quazaaSettings.SearchMaxPreviewLength);
	m_qSettings.setValue("MonitorFilter", quazaaSettings.SearchMonitorFilter);
	m_qSettings.setValue("MonitorQueue", quazaaSettings.SearchMonitorQueue);
	m_qSettings.setValue("MonitorSchemaURI", quazaaSettings.SearchMonitorSchemaURI);
	m_qSettings.setValue("SchemaTypes", quazaaSettings.SearchSchemaTypes);
	m_qSettings.setValue("SearchThrottle", quazaaSettings.SearchThrottle);
	m_qSettings.setValue("ShareMonkeyBaseURL", quazaaSettings.SearchShareMonkeyBaseURL);
	m_qSettings.setValue("ShowNames", quazaaSettings.SearchShowNames);
	m_qSettings.setValue("SwitchOnDownload", quazaaSettings.SearchSwitchOnDownload);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Chat");
	m_qSettings.setValue("AwayMessageIdleTime", quazaaSettings.ChatAwayMessageIdleTime);
	m_qSettings.setValue("Background", quazaaSettings.ChatBackground);
	m_qSettings.setValue("ConnectOnStartup", quazaaSettings.ChatConnectOnStartup);
	m_qSettings.setValue("ColorChatBackground", quazaaSettings.ChatColorChatBackground);
	m_qSettings.setValue("ColorNormalText", quazaaSettings.ChatColorNormalText);
	m_qSettings.setValue("ColorNoticesText", quazaaSettings.ChatColorNoticesText);
	m_qSettings.setValue("ColorRoomActionsText", quazaaSettings.ChatColorRoomActionsText);
	m_qSettings.setValue("ColorServerMessagesText", quazaaSettings.ChatColorServerMessagesText);
	m_qSettings.setValue("ColorTopicsText", quazaaSettings.ChatColorTopicsText);
	m_qSettings.setValue("EnableChatAllNetworks", quazaaSettings.ChatEnableChatAllNetworks);
	m_qSettings.setValue("EnableFileTransfers", quazaaSettings.ChatEnableFileTransfers);
	m_qSettings.setValue("GnutellaChatEnable", quazaaSettings.ChatGnutellaChatEnable);
	m_qSettings.setValue("IrcServerName", quazaaSettings.ChatIrcServerName);
	m_qSettings.setValue("IrcServerPort", quazaaSettings.ChatIrcServerPort);
	m_qSettings.setValue("ScreenFont", quazaaSettings.ChatScreenFont);
	m_qSettings.setValue("ShowTimestamp", quazaaSettings.ChatShowTimestamp);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Connection");
	m_qSettings.setValue("CanAcceptIncoming", quazaaSettings.ConnectionCanAcceptIncoming);
	m_qSettings.setValue("ConnectThrottle", quazaaSettings.ConnectionThrottle);
	m_qSettings.setValue("DetectConnectionLoss", quazaaSettings.ConnectionDetectLoss);
	m_qSettings.setValue("DetectConnectionReset", quazaaSettings.ConnectionDetectReset);
	m_qSettings.setValue("FailureLimit", quazaaSettings.ConnectionFailureLimit);
	m_qSettings.setValue("FailurePenalty", quazaaSettings.ConnectionFailurePenalty);
	m_qSettings.setValue("InAddress", quazaaSettings.ConnectionInAddress);
	m_qSettings.setValue("InBind", quazaaSettings.ConnectionInBind);
	m_qSettings.setValue("InSpeed", quazaaSettings.ConnectionInSpeed);
	m_qSettings.setValue("OutAddress", quazaaSettings.ConnectionOutAddress);
	m_qSettings.setValue("OutSpeed", quazaaSettings.ConnectionOutSpeed);
	m_qSettings.setValue("Port", quazaaSettings.ConnectionPort);
	m_qSettings.setValue("RandomPort", quazaaSettings.ConnectionRandomPort);
	m_qSettings.setValue("SendBuffer", quazaaSettings.ConnectionSendBuffer);
	m_qSettings.setValue("TimeoutConnect", quazaaSettings.ConnectionTimeout);
	m_qSettings.setValue("TimeoutHandshake", quazaaSettings.ConnectionTimeoutHandshake);
	m_qSettings.setValue("TimeoutTraffic", quazaaSettings.ConnectionTimeoutTraffic);
	m_qSettings.setValue("XPsp2PatchedConnect", quazaaSettings.ConnectionXPsp2PatchedConnect);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Web");
	m_qSettings.setValue("Ares", quazaaSettings.WebAres);
	m_qSettings.setValue("BrowserIntegration", quazaaSettings.WebBrowserIntegration);
	m_qSettings.setValue("ED2K", quazaaSettings.WebED2K);
	m_qSettings.setValue("Foxy", quazaaSettings.WebFoxy);
	m_qSettings.setValue("Gnutella", quazaaSettings.WebGnutella);
	m_qSettings.setValue("Magnet", quazaaSettings.WebMagnet);
	m_qSettings.setValue("ManageDownloadTypes", quazaaSettings.WebManageDownloadTypes);
	m_qSettings.setValue("Piolet", quazaaSettings.WebPiolet);
	m_qSettings.setValue("Torrent", quazaaSettings.WebTorrent);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("WebServices");
	m_qSettings.setValue("BitziAgent", quazaaSettings.WebServicesBitziAgent);
	m_qSettings.setValue("BitziOkay", quazaaSettings.WebServicesBitziOkay);
	m_qSettings.setValue("BitziWebSubmit", quazaaSettings.WebServicesBitziWebSubmit);
	m_qSettings.setValue("BitziWebView", quazaaSettings.WebServicesBitziWebView);
	m_qSettings.setValue("BitziXML", quazaaSettings.WebServicesBitziXML);
	m_qSettings.setValue("ShareMonkeyCid", quazaaSettings.WebServicesShareMonkeyCid);
	m_qSettings.setValue("ShareMonkeyOkay", quazaaSettings.WebServicesShareMonkeyOkay);
	m_qSettings.setValue("ShareMonkeySaveThumbnail", quazaaSettings.WebServicesShareMonkeySaveThumbnail);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Transfers");
	m_qSettings.setValue("BandwidthDownloads", quazaaSettings.TransfersBandwidthDownloads);
	m_qSettings.setValue("BandwidthHubIn", quazaaSettings.TransfersBandwidthHubIn);
	m_qSettings.setValue("BandwidthHubOut", quazaaSettings.TransfersBandwidthHubOut);
	m_qSettings.setValue("BandwidthHubUploads", quazaaSettings.TransfersBandwidthHubUploads);
	m_qSettings.setValue("BandwidthLeafIn", quazaaSettings.TransfersBandwidthLeafIn);
	m_qSettings.setValue("BandwidthLeafOut", quazaaSettings.TransfersBandwidthLeafOut);
	m_qSettings.setValue("BandwidthPeerIn", quazaaSettings.TransfersBandwidthPeerIn);
	m_qSettings.setValue("BandwidthPeerOut", quazaaSettings.TransfersBandwidthPeerOut);
	m_qSettings.setValue("BandwidthRequest", quazaaSettings.TransfersBandwidthRequest);
	m_qSettings.setValue("BandwidthUdpOut", quazaaSettings.TransfersBandwidthUdpOut);
	m_qSettings.setValue("BandwidthUploads", quazaaSettings.TransfersBandwidthUploads);
	m_qSettings.setValue("MinTransfersRest", quazaaSettings.TransfersMinTransfersRest);
	m_qSettings.setValue("RatesUnit", quazaaSettings.TransfersRatesUnit);
	m_qSettings.setValue("RequireConnectedNetwork", quazaaSettings.TransfersRequireConnectedNetwork);
	m_qSettings.setValue("SimpleProgressBar", quazaaSettings.TransfersSimpleProgressBar);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Downloads");
	m_qSettings.setValue("AllowBackwards", quazaaSettings.DownloadsAllowBackwards);
	m_qSettings.setValue("AutoClear", quazaaSettings.DownloadsAutoClear);
	m_qSettings.setValue("BufferSize", quazaaSettings.DownloadsBufferSize);
	m_qSettings.setValue("ChunkSize", quazaaSettings.DownloadsChunkSize);
	m_qSettings.setValue("ChunkStrap", quazaaSettings.DownloadsChunkStrap);
	m_qSettings.setValue("ClearDelay", quazaaSettings.DownloadsClearDelay);
	m_qSettings.setValue("CompletePath", quazaaSettings.DownloadsCompletePath);
	m_qSettings.setValue("ConnectThrottle", quazaaSettings.DownloadsConnectThrottle);
	m_qSettings.setValue("DropFailedSourcesThreshold", quazaaSettings.DownloadsDropFailedSourcesThreshold);
	m_qSettings.setValue("ExpandDownloads", quazaaSettings.DownloadsExpandDownloads);
	m_qSettings.setValue("FlushSD", quazaaSettings.DownloadsFlushSD);
	m_qSettings.setValue("IncompletePath", quazaaSettings.DownloadsIncompletePath);
	m_qSettings.setValue("MaxAllowedFailures", quazaaSettings.DownloadsMaxAllowedFailures);
	m_qSettings.setValue("MaxConnectingSources", quazaaSettings.DownloadsMaxConnectingSources);
	m_qSettings.setValue("MaxFiles", quazaaSettings.DownloadsMaxFiles);
	m_qSettings.setValue("MaxFileSearches", quazaaSettings.DownloadsMaxFileSearches);
	m_qSettings.setValue("MaxReviews", quazaaSettings.DownloadsMaxReviews);
	m_qSettings.setValue("MaxTransfers", quazaaSettings.DownloadsMaxTransfers);
	m_qSettings.setValue("MaxTransfersPerFile", quazaaSettings.DownloadsMaxTransfersPerFile);
	m_qSettings.setValue("Metadata", quazaaSettings.DownloadsMetadata);
	m_qSettings.setValue("MinSources", quazaaSettings.DownloadsMinSources);
	m_qSettings.setValue("NeverDrop", quazaaSettings.DownloadsNeverDrop);
	m_qSettings.setValue("PushTimeout", quazaaSettings.DownloadsPushTimeout);
	m_qSettings.setValue("QueueLimit", quazaaSettings.DownloadsQueueLimit);
	m_qSettings.setValue("RequestHash", quazaaSettings.DownloadsRequestHash);
	m_qSettings.setValue("RequestHTTP11", quazaaSettings.DownloadsRequestHTTP11);
	m_qSettings.setValue("RequestURLENC", quazaaSettings.DownloadsRequestURLENC);
	m_qSettings.setValue("RetryDelay", quazaaSettings.DownloadsRetryDelay);
	m_qSettings.setValue("SaveInterval", quazaaSettings.DownloadsSaveInterval);
	m_qSettings.setValue("SearchPeriod", quazaaSettings.DownloadsSearchPeriod);
	m_qSettings.setValue("ShowGroups", quazaaSettings.DownloadsShowGroups);
	m_qSettings.setValue("ShowMonitorURLs", quazaaSettings.DownloadsShowMonitorURLs);
	m_qSettings.setValue("ShowPercent", quazaaSettings.DownloadsShowPercent);
	m_qSettings.setValue("ShowSources", quazaaSettings.DownloadsShowSources);
	m_qSettings.setValue("SortColumns", quazaaSettings.DownloadsSortColumns);
	m_qSettings.setValue("SortSources", quazaaSettings.DownloadsSortSources);
	m_qSettings.setValue("SourcesWanted", quazaaSettings.DownloadsSourcesWanted);
	m_qSettings.setValue("StaggardStart", quazaaSettings.DownloadsStaggardStart);
	m_qSettings.setValue("StarveGiveUp", quazaaSettings.DownloadsStarveGiveUp);
	m_qSettings.setValue("StarveTimeout", quazaaSettings.DownloadsStarveTimeout);
	m_qSettings.setValue("URIPrompt", quazaaSettings.DownloadsURIPrompt);
	m_qSettings.setValue("VerifyED2K", quazaaSettings.DownloadsVerifyED2K);
	m_qSettings.setValue("VerifyFiles", quazaaSettings.DownloadsVerifyFiles);
	m_qSettings.setValue("VerifyTiger", quazaaSettings.DownloadsVerifyTiger);
	m_qSettings.setValue("WebHookEnable", quazaaSettings.DownloadsWebHookEnable);
	m_qSettings.setValue("WebHookExtensions", quazaaSettings.DownloadsWebHookExtensions);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Uploads");
	m_qSettings.setValue("AllowBackwards", quazaaSettings.UploadsAllowBackwards);
	m_qSettings.setValue("AutoClear", quazaaSettings.UploadsAutoClear);
	m_qSettings.setValue("ClampdownFactor", quazaaSettings.UploadsClampdownFactor);
	m_qSettings.setValue("ClampdownFloor", quazaaSettings.UploadsClampdownFloor);
	m_qSettings.setValue("ClearDelay", quazaaSettings.UploadsClearDelay);
	m_qSettings.setValue("DynamicPreviews", quazaaSettings.UploadsDynamicPreviews);
	m_qSettings.setValue("FreeBandwidthFactor", quazaaSettings.UploadsFreeBandwidthFactor);
	m_qSettings.setValue("FreeBandwidthValue", quazaaSettings.UploadsFreeBandwidthValue);
	m_qSettings.setValue("HubShareLimiting", quazaaSettings.UploadsHubShareLimiting);
	m_qSettings.setValue("MaxPerHost", quazaaSettings.UploadsMaxPerHost);
	m_qSettings.setValue("PreviewQuality", quazaaSettings.UploadsPreviewQuality);
	m_qSettings.setValue("PreviewTransfers", quazaaSettings.UploadsPreviewTransfers);
	m_qSettings.setValue("QueuePollMax", quazaaSettings.UploadsQueuePollMax);
	m_qSettings.setValue("QueuePollMin", quazaaSettings.UploadsQueuePollMin);
	m_qSettings.setValue("RewardQueuePercentage", quazaaSettings.UploadsRewardQueuePercentage);
	m_qSettings.setValue("RotateChunkLimit", quazaaSettings.UploadsRotateChunkLimit);
	m_qSettings.setValue("ShareHashset", quazaaSettings.UploadsShareHashset);
	m_qSettings.setValue("ShareMetadata", quazaaSettings.UploadsShareMetadata);
	m_qSettings.setValue("SharePartials", quazaaSettings.UploadsSharePartials);
	m_qSettings.setValue("SharePreviews", quazaaSettings.UploadsSharePreviews);
	m_qSettings.setValue("ShareTiger", quazaaSettings.UploadsShareTiger);
	m_qSettings.setValue("ThrottleMode", quazaaSettings.UploadsThrottleMode);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Security");
	m_qSettings.setValue("AllowProfileBrowse", quazaaSettings.SecurityAllowProfileBrowse);
	m_qSettings.setValue("AllowSharesBrowse", quazaaSettings.SecurityAllowSharesBrowse);
	m_qSettings.setValue("BlockedAgentUploadFilter", quazaaSettings.SecurityBlockedAgentUploadFilter);
	m_qSettings.setValue("DeleteFirewallException", quazaaSettings.SecurityDeleteFirewallException);
	m_qSettings.setValue("DeleteUPnPPorts", quazaaSettings.SecurityDeleteUPnPPorts);
	m_qSettings.setValue("ED2kChatFilter", quazaaSettings.SecurityED2kChatFilter);
	m_qSettings.setValue("EnableFirewallException", quazaaSettings.SecurityEnableFirewallException);
	m_qSettings.setValue("EnableUPnP", quazaaSettings.SecurityEnableUPnP);
	m_qSettings.setValue("FirewallState", quazaaSettings.SecurityFirewallState);
	m_qSettings.setValue("ChatFilter", quazaaSettings.SecurityChatFilter);
	m_qSettings.setValue("IrcFloodLimit", quazaaSettings.SecurityIrcFloodLimit);
	m_qSettings.setValue("IrcFloodProtection", quazaaSettings.SecurityIrcFloodProtection);
	m_qSettings.setValue("MaxMaliciousFileSize", quazaaSettings.SecurityMaxMaliciousFileSize);
	m_qSettings.setValue("RemoteEnable", quazaaSettings.SecurityRemoteEnable);
	m_qSettings.setValue("RemotePassword", quazaaSettings.SecurityRemotePassword);
	m_qSettings.setValue("RemoteUsername", quazaaSettings.SecurityRemoteUsername);
	m_qSettings.setValue("SearchIgnoreLocalIP", quazaaSettings.SecuritySearchIgnoreLocalIP);
	m_qSettings.setValue("SearchIgnoreOwnIP", quazaaSettings.SecuritySearchIgnoreOwnIP);
	m_qSettings.setValue("SearchSpamFilterThreshold", quazaaSettings.SecuritySearchSpamFilterThreshold);
	m_qSettings.setValue("UPnPSkipWANIPSetup", quazaaSettings.SecurityUPnPSkipWANIPSetup);
	m_qSettings.setValue("UPnPSkipWANPPPSetup", quazaaSettings.SecurityUPnPSkipWANPPPSetup);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella");
	m_qSettings.setValue("CompressHub2Hub", quazaaSettings.GnutellaCompressHub2Hub);
	m_qSettings.setValue("CompressHub2Leaf", quazaaSettings.GnutellaCompressHub2Leaf);
	m_qSettings.setValue("CompressLeaf2Hub", quazaaSettings.GnutellaCompressLeaf2Hub);
	m_qSettings.setValue("ConnectFactor", quazaaSettings.GnutellaConnectFactor);
	m_qSettings.setValue("ConnectThrottle", quazaaSettings.GnutellaConnectThrottle);
	m_qSettings.setValue("HitsPerPacket", quazaaSettings.GnutellaHitsPerPacket);
	m_qSettings.setValue("HostCacheSize", quazaaSettings.GnutellaHostCacheSize);
	m_qSettings.setValue("HostCacheView", quazaaSettings.GnutellaHostCacheView);
	m_qSettings.setValue("MaxHits", quazaaSettings.GnutellaMaxHits);
	m_qSettings.setValue("MaxResults", quazaaSettings.GnutellaMaxResults);
	m_qSettings.setValue("RouteCache", quazaaSettings.GnutellaRouteCache);
	m_qSettings.setValue("SpecifyProtocol", quazaaSettings.GnutellaSpecifyProtocol);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella2");
	m_qSettings.setValue("ClientMode", quazaaSettings.Gnutella2ClientMode);
	m_qSettings.setValue("Enable", quazaaSettings.Gnutella2Enable);
	m_qSettings.setValue("HAWPeriod", quazaaSettings.Gnutella2HAWPeriod);
	m_qSettings.setValue("HostCount", quazaaSettings.Gnutella2HostCount);
	m_qSettings.setValue("HostCurrent", quazaaSettings.Gnutella2HostCurrent);
	m_qSettings.setValue("HostExpire", quazaaSettings.Gnutella2HostExpire);
	m_qSettings.setValue("HubHorizonSize", quazaaSettings.Gnutella2HubHorizonSize);
	m_qSettings.setValue("HubVerified", quazaaSettings.Gnutella2HubVerified);
	m_qSettings.setValue("KHLHubCount", quazaaSettings.Gnutella2KHLHubCount);
	m_qSettings.setValue("KHLPeriod", quazaaSettings.Gnutella2KHLPeriod);
	m_qSettings.setValue("LNIPeriod", quazaaSettings.Gnutella2LNIPeriod);
	m_qSettings.setValue("NumHubs", quazaaSettings.Gnutella2NumHubs);
	m_qSettings.setValue("NumLeafs", quazaaSettings.Gnutella2NumLeafs);
	m_qSettings.setValue("NumPeers", quazaaSettings.Gnutella2NumPeers);
	m_qSettings.setValue("PingRate", quazaaSettings.Gnutella2PingRate);
	m_qSettings.setValue("PingRelayLimit", quazaaSettings.Gnutella2PingRelayLimit);
	m_qSettings.setValue("QueryGlobalThrottle", quazaaSettings.Gnutella2QueryGlobalThrottle);
	m_qSettings.setValue("QueryHostDeadline", quazaaSettings.Gnutella2QueryHostDeadline);
	m_qSettings.setValue("QueryHostThrottle", quazaaSettings.Gnutella2QueryHostThrottle);
	m_qSettings.setValue("QueryLimit", quazaaSettings.Gnutella2QueryLimit);
	m_qSettings.setValue("RequeryDelay", quazaaSettings.Gnutella2RequeryDelay);
	m_qSettings.setValue("UdpBuffers", quazaaSettings.Gnutella2UdpBuffers);
	m_qSettings.setValue("UdpGlobalThrottle", quazaaSettings.Gnutella2UdpGlobalThrottle);
	m_qSettings.setValue("UdpInExpire", quazaaSettings.Gnutella2UdpInExpire);
	m_qSettings.setValue("UdpInFrames", quazaaSettings.Gnutella2UdpInFrames);
	m_qSettings.setValue("UdpMTU", quazaaSettings.Gnutella2UdpMTU);
	m_qSettings.setValue("UdpOutExpire", quazaaSettings.Gnutella2UdpOutExpire);
	m_qSettings.setValue("UdpOutFrames", quazaaSettings.Gnutella2UdpOutFrames);
	m_qSettings.setValue("UdpOutResend", quazaaSettings.Gnutella2UdpOutResend);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella1");
	m_qSettings.setValue("ClientMode", quazaaSettings.Gnutella1ClientMode);
	m_qSettings.setValue("DefaultTTL", quazaaSettings.Gnutella1DefaultTTL);
	m_qSettings.setValue("Enable", quazaaSettings.Gnutella1Enable);
	m_qSettings.setValue("EnableDIPPSupport", quazaaSettings.Gnutella1EnableDIPPSupport);
	m_qSettings.setValue("EnableGGEP", quazaaSettings.Gnutella1EnableGGEP);
	m_qSettings.setValue("HitQueueLimit", quazaaSettings.Gnutella1HitQueueLimit);
	m_qSettings.setValue("HostCount", quazaaSettings.Gnutella1HostCount);
	m_qSettings.setValue("HostExpire", quazaaSettings.Gnutella1HostExpire);
	m_qSettings.setValue("MaxHostsInPongs", quazaaSettings.Gnutella1MaxHostsInPongs);
	m_qSettings.setValue("MaximumPacket", quazaaSettings.Gnutella1MaximumPacket);
	m_qSettings.setValue("MaximumQuery", quazaaSettings.Gnutella1MaximumQuery);
	m_qSettings.setValue("MaximumTTL", quazaaSettings.Gnutella1MaximumTTL);
	m_qSettings.setValue("NumHubs", quazaaSettings.Gnutella1NumHubs);
	m_qSettings.setValue("NumLeafs", quazaaSettings.Gnutella1NumLeafs);
	m_qSettings.setValue("NumPeers", quazaaSettings.Gnutella1NumPeers);
	m_qSettings.setValue("PacketBufferSize", quazaaSettings.Gnutella1PacketBufferSize);
	m_qSettings.setValue("PacketBufferTime", quazaaSettings.Gnutella1PacketBufferTime);
	m_qSettings.setValue("PingFlood", quazaaSettings.Gnutella1PingFlood);
	m_qSettings.setValue("PingRate", quazaaSettings.Gnutella1PingRate);
	m_qSettings.setValue("PongCache", quazaaSettings.Gnutella1PongCache);
	m_qSettings.setValue("PongCount", quazaaSettings.Gnutella1PongCount);
	m_qSettings.setValue("QueryHitUTF8", quazaaSettings.Gnutella1QueryHitUTF8);
	m_qSettings.setValue("QuerySearchUTF8", quazaaSettings.Gnutella1QuerySearchUTF8);
	m_qSettings.setValue("QueryThrottle", quazaaSettings.Gnutella1QueryThrottle);
	m_qSettings.setValue("RequeryDelay", quazaaSettings.Gnutella1RequeryDelay);
	m_qSettings.setValue("SearchTTL", quazaaSettings.Gnutella1SearchTTL);
	m_qSettings.setValue("StrictPackets", quazaaSettings.Gnutella1StrictPackets);
	m_qSettings.setValue("TranslateTTL", quazaaSettings.Gnutella1TranslateTTL);
	m_qSettings.setValue("VendorMsg", quazaaSettings.Gnutella1VendorMsg);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Ares");
	m_qSettings.setValue("Enable", quazaaSettings.AresEnable);
	m_qSettings.setValue("MaximumDownloads", quazaaSettings.AresMaximumDownloads);
	m_qSettings.setValue("MaximumUploads", quazaaSettings.AresMaximumUploads);
	m_qSettings.setValue("MaximumUploadsPerUser", quazaaSettings.AresMaximumUploadsPerUser);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("eDonkey");
	m_qSettings.setValue("DefaultServerFlags", quazaaSettings.EDonkeyDefaultServerFlags);
	m_qSettings.setValue("DequeueTime", quazaaSettings.EDonkeyDequeueTime);
	m_qSettings.setValue("Enable", quazaaSettings.EDonkeyEnable);
	m_qSettings.setValue("EnableKad", quazaaSettings.EDonkeyEnableKad);
	m_qSettings.setValue("Endgame", quazaaSettings.EDonkeyEndgame);
	m_qSettings.setValue("ExtendedRequest", quazaaSettings.EDonkeyExtendedRequest);
	m_qSettings.setValue("FastConnect", quazaaSettings.EDonkeyFastConnect);
	m_qSettings.setValue("ForceHighID", quazaaSettings.EDonkeyForceHighID);
	m_qSettings.setValue("FrameSize", quazaaSettings.EDonkeyFrameSize);
	m_qSettings.setValue("GetSourcesThrottle", quazaaSettings.EDonkeyGetSourcesThrottle);
	m_qSettings.setValue("LargeFileSupport", quazaaSettings.EDonkeyLargeFileSupport);
	m_qSettings.setValue("LearnNewServers", quazaaSettings.EDonkeyLearnNewServers);
	m_qSettings.setValue("LearnNewServersClient", quazaaSettings.EDonkeyLearnNewServersClient);
	m_qSettings.setValue("MagnetSearch", quazaaSettings.EDonkeyMagnetSearch);
	m_qSettings.setValue("MaxClients", quazaaSettings.EDonkeyMaxClients);
	m_qSettings.setValue("MaxResults", quazaaSettings.EDonkeyMaxResults);
	m_qSettings.setValue("MaxShareCount", quazaaSettings.EDonkeyMaxShareCount);
	m_qSettings.setValue("MetAutoQuery", quazaaSettings.EDonkeyMetAutoQuery);
	m_qSettings.setValue("MinServerFileSize", quazaaSettings.EDonkeyMinServerFileSize);
	m_qSettings.setValue("NumServers", quazaaSettings.EDonkeyNumServers);
	m_qSettings.setValue("PacketThrottle", quazaaSettings.EDonkeyPacketThrottle);
	m_qSettings.setValue("QueryFileThrottle", quazaaSettings.EDonkeyQueryFileThrottle);
	m_qSettings.setValue("QueryGlobalThrottle", quazaaSettings.EDonkeyQueryGlobalThrottle);
	m_qSettings.setValue("QueueRankThrottle", quazaaSettings.EDonkeyQueueRankThrottle);
	m_qSettings.setValue("QueryServerThrottle", quazaaSettings.EDonkeyQueryServerThrottle);
	m_qSettings.setValue("ReAskTime", quazaaSettings.EDonkeyReAskTime);
	m_qSettings.setValue("RequestPipe", quazaaSettings.EDonkeyRequestPipe);
	m_qSettings.setValue("RequestSize", quazaaSettings.EDonkeyRequestSize);
	m_qSettings.setValue("SearchCachedServers", quazaaSettings.EDonkeySearchCachedServers);
	m_qSettings.setValue("SendPortServer", quazaaSettings.EDonkeySendPortServer);
	m_qSettings.setValue("ServerListURL", quazaaSettings.EDonkeyServerListURL);
	m_qSettings.setValue("ServerWalk", quazaaSettings.EDonkeyServerWalk);
	m_qSettings.setValue("SourceThrottle", quazaaSettings.EDonkeySourceThrottle);
	m_qSettings.setValue("StatsGlobalThrottle", quazaaSettings.EDonkeyStatsGlobalThrottle);
	m_qSettings.setValue("StatsServerThrottle", quazaaSettings.EDonkeyStatsServerThrottle);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("BitTorrent");
	m_qSettings.setValue("AutoClear", quazaaSettings.BitTorrentAutoClear);
	m_qSettings.setValue("AutoSeed", quazaaSettings.BitTorrentAutoSeed);
	m_qSettings.setValue("BandwidthPercentage", quazaaSettings.BitTorrentBandwidthPercentage);
	m_qSettings.setValue("ClearRatio", quazaaSettings.BitTorrentClearRatio);
	m_qSettings.setValue("CodePage", quazaaSettings.BitTorrentCodePage);
	m_qSettings.setValue("DefaultTracker", quazaaSettings.BitTorrentDefaultTracker);
	m_qSettings.setValue("DefaultTrackerPeriod", quazaaSettings.BitTorrentDefaultTrackerPeriod);
	m_qSettings.setValue("DhtPruneTime", quazaaSettings.BitTorrentDhtPruneTime);
	m_qSettings.setValue("DownloadConnections", quazaaSettings.BitTorrentDownloadConnections);
	m_qSettings.setValue("DownloadTorrents", quazaaSettings.BitTorrentDownloadTorrents);
	m_qSettings.setValue("Endgame", quazaaSettings.BitTorrentEndgame);
	m_qSettings.setValue("ExtraKeys", quazaaSettings.BitTorrentExtraKeys);
	m_qSettings.setValue("LinkPing", quazaaSettings.BitTorrentLinkPing);
	m_qSettings.setValue("LinkTimeout", quazaaSettings.BitTorrentLinkTimeout);
	m_qSettings.setValue("Managed", quazaaSettings.BitTorrentManaged);
	m_qSettings.setValue("PreferBTSources", quazaaSettings.BitTorrentPreferBTSources);
	m_qSettings.setValue("RandomPeriod", quazaaSettings.BitTorrentRandomPeriod);
	m_qSettings.setValue("RequestLimit", quazaaSettings.BitTorrentRequestLimit);
	m_qSettings.setValue("RequestPipe", quazaaSettings.BitTorrentRequestPipe);
	m_qSettings.setValue("RequestSize", quazaaSettings.BitTorrentRequestSize);
	m_qSettings.setValue("ShowFilesInDownload", quazaaSettings.BitTorrentShowFilesInDownload);
	m_qSettings.setValue("SourceExchangePeriod", quazaaSettings.BitTorrentSourceExchangePeriod);
	m_qSettings.setValue("StartPaused", quazaaSettings.BitTorrentStartPaused);
	m_qSettings.setValue("TestPartials", quazaaSettings.BitTorrentTestPartials);
	m_qSettings.setValue("TorrentPath", quazaaSettings.BitTorrentTorrentPath);
	m_qSettings.setValue("TrackerKey", quazaaSettings.BitTorrentTrackerKey);
	m_qSettings.setValue("UploadCount", quazaaSettings.BitTorrentUploadCount);
	m_qSettings.setValue("UseKademlia", quazaaSettings.BitTorrentUseKademlia);
	m_qSettings.setValue("UseSaveDialog", quazaaSettings.BitTorrentUseSaveDialog);
	m_qSettings.setValue("UseTemp", quazaaSettings.BitTorrentUseTemp);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Discovery");
	m_qSettings.setValue("AccessThrottle", quazaaSettings.DiscoveryAccessThrottle);
	m_qSettings.setValue("BootstrapCount", quazaaSettings.DiscoveryBootstrapCount);
	m_qSettings.setValue("CacheCount", quazaaSettings.DiscoveryCacheCount);
	m_qSettings.setValue("DefaultUpdate", quazaaSettings.DiscoveryDefaultUpdate);
	m_qSettings.setValue("EnableG1GWC", quazaaSettings.DiscoveryEnableG1GWC);
	m_qSettings.setValue("FailureLimit", quazaaSettings.DiscoveryFailureLimit);
	m_qSettings.setValue("Lowpoint", quazaaSettings.DiscoveryLowpoint);
	m_qSettings.setValue("UpdatePeriod", quazaaSettings.DiscoveryUpdatePeriod);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Scheduler");

	m_qSettings.endGroup();
}

void QuazaaSettings::loadSettings()
{
	QSettings m_qSettings(quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName());

	m_qSettings.beginGroup("Live");
	quazaaSettings.LiveAdultWarning = m_qSettings.value("AdultWarning", false).toBool();
	quazaaSettings.LiveAutoClose = m_qSettings.value("AutoClose", false).toBool();
	quazaaSettings.LiveBandwidthScale = m_qSettings.value("BandwidthScale", 100).toInt();
	quazaaSettings.LiveDefaultED2KServersLoaded = m_qSettings.value("DefaultED2KServersLoaded", false).toBool();
	quazaaSettings.LiveDiskSpaceStop = m_qSettings.value("DiskSpaceStop", false).toBool();
	quazaaSettings.LiveDiskSpaceWarning = m_qSettings.value("DiskSpaceWarning", false).toBool();
	quazaaSettings.LiveDiskWriteWarning = m_qSettings.value("DiskWriteWarning", false).toBool();
	quazaaSettings.LiveDonkeyServerWarning = m_qSettings.value("DonkeyServerWarning", false).toBool();
	quazaaSettings.LiveLastDuplicateHash = m_qSettings.value("LastDuplicateHash", "").toString();
	quazaaSettings.LiveMaliciousWarning = m_qSettings.value("MaliciousWarning", false).toBool();
	quazaaSettings.LiveQueueLimitWarning = m_qSettings.value("QueueLimitWarning", false).toBool();
	quazaaSettings.LiveUploadLimitWarning = m_qSettings.value("UploadLimitWarning", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Basic");
	quazaaSettings.BasicChatTips = m_qSettings.value("ChatTips", true).toBool();
	quazaaSettings.BasicCloseMode = m_qSettings.value("CloseMode", 0).toInt();
	quazaaSettings.BasicConnectOnStartup = m_qSettings.value("ConnectOnStartup", true).toBool();
	quazaaSettings.BasicDiskSpaceStop = m_qSettings.value("DiskSpaceStop", 25).toInt();
	quazaaSettings.BasicDiskSpaceWarning = m_qSettings.value("DiskSpaceWarning", 500).toInt();
	quazaaSettings.BasicDownloadsTips = m_qSettings.value("DownloadsTips", true).toBool();
	quazaaSettings.BasicLibraryTips = m_qSettings.value("LibraryTips", true).toBool();
	quazaaSettings.BasicMediaTips = m_qSettings.value("MediaTips", true).toBool();
	quazaaSettings.BasicMinimizeToTray = m_qSettings.value("MinimizeToTray", false).toBool();
	quazaaSettings.BasicNeighboursTips = m_qSettings.value("NeighboursTips", true).toBool();
	quazaaSettings.BasicSearchTips = m_qSettings.value("SearchTips", true).toBool();
	quazaaSettings.BasicStartWithSystem = m_qSettings.value("StartWithSystem", false).toBool();
	quazaaSettings.BasicTipDelay = m_qSettings.value("TipDelay", 600).toInt();
	quazaaSettings.BasicTipLowResMode = m_qSettings.value("TipLowResMode", true).toBool();
	quazaaSettings.BasicTipTransparency = m_qSettings.value("TipTransparency", 255).toInt();
	quazaaSettings.BasicUploadsTips = m_qSettings.value("UploadsTips", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Parental");
	quazaaSettings.ParentalAdultFilter = m_qSettings.value("AdultFilter",
														 QStringList() << "shit" << "fuck" << "damn" << "bitch"
														 << "asshole" << "ass" << "pussy" << "cock" << "cunt" << "dick"
														 << "whore" << "slut" << "clit").toStringList();
	quazaaSettings.ParentalChatAdultCensor = m_qSettings.value("ChatAdultCensor", true).toBool();
	quazaaSettings.ParentalFilterAdultSearchResults = m_qSettings.value("FilterAdultSearchResults", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Logging");
	quazaaSettings.LoggingDebugLog = m_qSettings.value("DebugLog", false).toBool();
	quazaaSettings.LoggingLogLevel = m_qSettings.value("LogLevel", 3).toInt();
	quazaaSettings.LoggingLogShowTimestamp = m_qSettings.value("LogShowTimestamp", true).toBool();
	quazaaSettings.LoggingMaxDebugLogSize = m_qSettings.value("MaxDebugLogSize", 10).toInt();
	quazaaSettings.LoggingSearchLog = m_qSettings.value("SearchLog", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Library");
	quazaaSettings.LibraryFilterURI = m_qSettings.value("FilterURI", "").toString();
	quazaaSettings.LibraryGhostFiles = m_qSettings.value("GhostFiles", true).toBool();
	quazaaSettings.LibraryHashWindow = m_qSettings.value("HashWindow", true).toBool();
	quazaaSettings.LibraryHighPriorityHashing = m_qSettings.value("HighPriorityHashing", false).toInt();
	quazaaSettings.LibraryHighPriorityHashingSpeed = m_qSettings.value("HighPriorityHashingSpeed", 20).toBool();
	quazaaSettings.LibraryHistoryDays = m_qSettings.value("HistoryDays", 3).toInt();
	quazaaSettings.LibraryHistoryTotal = m_qSettings.value("HistoryTotal", 32).toInt();
	quazaaSettings.LibraryLowPriorityHashingSpeed = m_qSettings.value("LowPriorityHashingSpeed", 2).toInt();
	quazaaSettings.LibraryNeverShareTypes = m_qSettings.value("NeverShareTypes",
														 QStringList() << "vbs" << "js" << "jc!" << "fb!" << "bc!" << "!ut"
														 << "dbx" << "part" << "partial" << "pst" << "reget" << "getright"
														 << "pif" << "lnk" << "sd" << "url" << "wab" << "m4p" << "infodb"
														 << "racestats" << "chk" << "tmp" << "temp" << "ini" << "inf" << "log"
														 << "old" << "manifest" << "met" << "bak" << "$$$" << "---" << "~~~"
														 << "###" << "__incomplete___" << "wma" << "wmv").toStringList();
	quazaaSettings.LibraryPartialMatch = m_qSettings.value("PartialMatch", true).toBool();
	quazaaSettings.LibraryPreferAPETags = m_qSettings.value("PreferAPETags", true).toBool();
	quazaaSettings.LibraryQueryRouteSize = m_qSettings.value("QueryRouteSize", 20).toInt();
	quazaaSettings.LibraryRememberViews = m_qSettings.value("RememberViews", true).toBool();
	quazaaSettings.LibrarySafeExecuteTypes = m_qSettings.value("SafeExecuteTypes",
														  QStringList() << "3gp" << "7z" << "aac" << "ace" << "ape" << "avi"
														  << "bmp" << "co" << "collection" << "flv" << "gif" << "iso"
														  << "jpg" << "jpeg" << "lit" << "mid" << "mov" << "m1v" << "m2v"
														  << "m3u" << "m4a" << "mkv" << "mp2" << "mp3" << "mp4" << "mpa"
														  << "mpe" << "mpg" << "mpeg" << "ogg" << "ogm" << "pdf" << "png"
														  << "qt" << "rar" << "rm" << "sks" << "tar" << "tgz" << "torrent"
														  << "txt" << "wav" << "zip").toStringList();
	quazaaSettings.LibraryScanAPE = m_qSettings.value("ScanAPE", true).toBool();
	quazaaSettings.LibraryScanASF = m_qSettings.value("ScanASF", true).toBool();
	quazaaSettings.LibraryScanAVI = m_qSettings.value("ScanAVI", true).toBool();
	quazaaSettings.LibraryScanCHM = m_qSettings.value("ScanCHM", true).toBool();
	quazaaSettings.LibraryScanEXE = m_qSettings.value("ScanEXE", true).toBool();
	quazaaSettings.LibraryScanImage = m_qSettings.value("ScanImage", true).toBool();
	quazaaSettings.LibraryScanMP3 = m_qSettings.value("ScanMP3", true).toBool();
	quazaaSettings.LibraryScanMPC = m_qSettings.value("ScanMPC", true).toBool();
	quazaaSettings.LibraryScanMPEG = m_qSettings.value("ScanMPEG", true).toBool();
	quazaaSettings.LibraryScanMSI = m_qSettings.value("ScanMSI", true).toBool();
	quazaaSettings.LibraryScanOGG = m_qSettings.value("ScanOGG", true).toBool();
	quazaaSettings.LibraryScanPDF = m_qSettings.value("ScanPDF", true).toBool();
	quazaaSettings.LibrarySchemaURI = m_qSettings.value("SchemaURI", "http://www.limewire.com/schemas/audio.xsd").toString();
	quazaaSettings.LibraryShares = m_qSettings.value("Shares", QStringList() << QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).replace(QString("\\"), QString("/")) + "/Quazaa Downloads"
												<< QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/Quazaa/Torrents").toStringList();
	quazaaSettings.LibraryShowCoverArt = m_qSettings.value("ShowCoverArt", true).toBool();
	quazaaSettings.LibrarySmartSeriesDetection = m_qSettings.value("SmartSeriesDetection", false).toBool();
	quazaaSettings.LibrarySourceExpire = m_qSettings.value("SourceExpire", 86400).toInt();
	quazaaSettings.LibrarySourceMesh = m_qSettings.value("SourceMesh", true).toBool();
	quazaaSettings.LibraryThumbSize = m_qSettings.value("ThumbSize", 96).toInt();
	quazaaSettings.LibraryTigerHeight = m_qSettings.value("TigerHeight", 9).toInt();
	quazaaSettings.LibraryTreeSize = m_qSettings.value("TreeSize", 200).toInt();
	quazaaSettings.LibraryWatchFolders = m_qSettings.value("WatchFolders", true).toBool();
	quazaaSettings.LibraryWatchFoldersTimeout = m_qSettings.value("WatchFoldersTimeout", 5).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("MediaPlayer");
	quazaaSettings.MediaAspect = m_qSettings.value("Aspect", 0).toInt();
	quazaaSettings.MediaAudioVisualPlugin = m_qSettings.value("AudioVisualPlugin", "").toString();
	quazaaSettings.MediaCustomPlayerPath = m_qSettings.value("CustomPlayerPath", "").toString();
	quazaaSettings.MediaHandler = m_qSettings.value("MediaHandler", 0).toInt();
	quazaaSettings.MediaFileTypes = m_qSettings.value("FileTypes", QStringList() << "asx" << "wax" << "m3u" << "wvx" << "wmx"
													   << "asf" << "wav" << "snd" << "au" << "aif" << "aifc" << "aiff" << "mp3"
													   << "cda" << "mid" << "rmi" << "midi" << "avi" << "mpeg" << "mpg"
													   << "m1v" << "mp2" << "mpa" << "mpe").toStringList();
	quazaaSettings.MediaListVisible = m_qSettings.value("ListVisible", true).toBool();
	quazaaSettings.MediaMute = m_qSettings.value("Mute", false).toBool();
	quazaaSettings.MediaOpenPath = m_qSettings.value("OpenPath", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).replace(QString("\\"), QString("/")) + "/Quazaa Downloads").toString();
	quazaaSettings.MediaPlaylist = m_qSettings.value("Playlist", QStringList()).toStringList();
	quazaaSettings.MediaShuffle = m_qSettings.value("Shuffle", false).toBool();
	quazaaSettings.MediaRepeat = m_qSettings.value("Repeat", false).toBool();
	quazaaSettings.MediaStatusVisible = m_qSettings.value("StatusVisible", true).toBool();
	quazaaSettings.MediaVolume = m_qSettings.value("Volume", 1.0).toReal();
	quazaaSettings.MediaZoom = m_qSettings.value("Zoom", 0).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Search");
	quazaaSettings.SearchBlankSchemaURI = m_qSettings.value("BlankSchemaURI", "").toString();
	quazaaSettings.SearchBrowseTreeSize = m_qSettings.value("BrowseTreeSize", 180).toInt();
	quazaaSettings.SearchClearPrevious = m_qSettings.value("ClearPrevious", 0).toInt();
	quazaaSettings.SearchExpandSearchMatches = m_qSettings.value("ExpandSearchMatches", false).toBool();
	quazaaSettings.SearchFilterMask = m_qSettings.value("FilterMask", 360).toInt();
	quazaaSettings.SearchHighlightNew = m_qSettings.value("HighlightNew", true).toBool();
	quazaaSettings.SearchLastSchemaURI = m_qSettings.value("LastSchemaURI", "").toString();
	quazaaSettings.SearchMaxPreviewLength = m_qSettings.value("MaxPreviewLength", 20480).toInt();
	quazaaSettings.SearchMonitorFilter = m_qSettings.value("MonitorFilter", "").toString();
	quazaaSettings.SearchMonitorQueue = m_qSettings.value("MonitorQueue", 128).toInt();
	quazaaSettings.SearchMonitorSchemaURI = m_qSettings.value("MonitorSchemaURI", "http://www.limewire.com/schemas/audio.xsd").toString();
	quazaaSettings.SearchSchemaTypes = m_qSettings.value("SchemaTypes", true).toBool();
	quazaaSettings.SearchThrottle = m_qSettings.value("SearchThrottle", 200).toInt();
	quazaaSettings.SearchShareMonkeyBaseURL = m_qSettings.value("ShareMonkeyBaseURL", "http://tools.sharemonkey.com/xml/").toString();
	quazaaSettings.SearchShowNames = m_qSettings.value("ShowNames", true).toBool();
	quazaaSettings.SearchSwitchOnDownload = m_qSettings.value("SwitchOnDownload", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Chat");
	quazaaSettings.ChatAwayMessageIdleTime = m_qSettings.value("AwayMessageIdleTime", 30).toInt();
	quazaaSettings.ChatBackground = m_qSettings.value("Background", "").toString();
	quazaaSettings.ChatConnectOnStartup = m_qSettings.value("ConnectOnStartup", false).toBool();
	quazaaSettings.ChatColorChatBackground = m_qSettings.value("ColorChatBackground", QColor(qRgb(255,255,255))).value<QColor>();
	quazaaSettings.ChatColorNormalText = m_qSettings.value("ColorNormalText", QColor(qRgb(0,0,0))).value<QColor>();
	quazaaSettings.ChatColorNoticesText = m_qSettings.value("ColorNoticesText", QColor(qRgb(255,0,0))).value<QColor>();
	quazaaSettings.ChatColorRoomActionsText = m_qSettings.value("ColorRoomActionsText", QColor(qRgb(0,170,0))).value<QColor>();
	quazaaSettings.ChatColorServerMessagesText = m_qSettings.value("ColorServerMessagesText", QColor(qRgb(0,0,255))).value<QColor>();
	quazaaSettings.ChatColorTopicsText = m_qSettings.value("ColorTopicsText", QColor(qRgb(170,85,127))).value<QColor>();
	quazaaSettings.ChatEnableChatAllNetworks = m_qSettings.value("EnableChatAllNetworks", true).toBool();
	quazaaSettings.ChatEnableFileTransfers = m_qSettings.value("EnableFileTransfers", true).toBool();
	quazaaSettings.ChatGnutellaChatEnable = m_qSettings.value("GnutellaChatEnable", true).toBool();
	quazaaSettings.ChatIrcServerName = m_qSettings.value("IrcServerName", "irc.p2pchat.net").toString();
	quazaaSettings.ChatIrcServerPort = m_qSettings.value("IrcServerPort", 6667).toInt();
	quazaaSettings.ChatScreenFont = m_qSettings.value("ScreenFont", QFont()).value<QFont>();
	quazaaSettings.ChatShowTimestamp = m_qSettings.value("ShowTimestamp", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Connection");
	quazaaSettings.ConnectionCanAcceptIncoming = m_qSettings.value("CanAcceptIncoming", 0).toInt();
	quazaaSettings.ConnectionThrottle = m_qSettings.value("ConnectThrottle", 250).toInt();
	quazaaSettings.ConnectionDetectLoss = m_qSettings.value("DetectConnectionLoss", true).toBool();
	quazaaSettings.ConnectionDetectReset = m_qSettings.value("DetectConnectionReset", false).toBool();
	quazaaSettings.ConnectionFailureLimit = m_qSettings.value("FailureLimit", 3).toInt();
	quazaaSettings.ConnectionFailurePenalty = m_qSettings.value("FailurePenalty", 300).toInt();
	quazaaSettings.ConnectionInAddress = m_qSettings.value("InAddress", "").toString();
	quazaaSettings.ConnectionInBind = m_qSettings.value("InBind", false).toBool();
	quazaaSettings.ConnectionInSpeed = m_qSettings.value("InSpeed", 1050.00).toDouble();
	quazaaSettings.ConnectionOutAddress = m_qSettings.value("OutAddress", "").toString();
	quazaaSettings.ConnectionOutSpeed = m_qSettings.value("OutSpeed", 32.00).toDouble();
	quazaaSettings.ConnectionPort = m_qSettings.value("Port", 6350).toInt();
	quazaaSettings.ConnectionRandomPort = m_qSettings.value("RandomPort", false).toBool();
	quazaaSettings.ConnectionSendBuffer = m_qSettings.value("SendBuffer", 2048).toInt();
	quazaaSettings.ConnectionTimeout = m_qSettings.value("TimeoutConnect", 16).toInt();
	quazaaSettings.ConnectionTimeoutHandshake = m_qSettings.value("TimeoutHandshake", 45).toInt();
	quazaaSettings.ConnectionTimeoutTraffic = m_qSettings.value("TimeoutTraffic", 140).toInt();
	quazaaSettings.ConnectionXPsp2PatchedConnect = m_qSettings.value("XPsp2PatchedConnect", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Web");
	quazaaSettings.WebBrowserIntegration = m_qSettings.value("BrowserIntegration", false).toBool();
	quazaaSettings.WebAres = m_qSettings.value("Ares", true).toBool();
	quazaaSettings.WebED2K = m_qSettings.value("ED2K", true).toBool();
	quazaaSettings.WebFoxy = m_qSettings.value("Foxy", true).toBool();
	quazaaSettings.WebGnutella = m_qSettings.value("Gnutella", true).toBool();
	quazaaSettings.WebMagnet = m_qSettings.value("Magnet", true).toBool();
	quazaaSettings.WebManageDownloadTypes = m_qSettings.value("ManageDownloadTypes", QStringList() << "7z" << "ace" << "arj"
														 << "bin" << "exe" << "fml" << "grs" << "gz" << "hqx" << "iso"
														 << "lzh" << "mp3" << "msi" << "r0" << "rar" << "sit" << "tar"
														 << "tgz" << "z" << "zip").toStringList();
	quazaaSettings.WebPiolet = m_qSettings.value("Piolet", true).toBool();
	quazaaSettings.WebTorrent = m_qSettings.value("Torrent", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("WebServices");
	quazaaSettings.WebServicesBitziAgent = m_qSettings.value("BitziAgent", ".").toString();
	quazaaSettings.WebServicesBitziOkay = m_qSettings.value("BitziOkay", false).toBool();
	quazaaSettings.WebServicesBitziWebSubmit = m_qSettings.value("BitziWebSubmit", "http://bitzi.com/lookup/(SHA1).(TTH)?fl=(SIZE)&ff=(FIRST20)&fn=(NAME)&tag.ed2k.ed2khash=(ED2K)&(INFO)&a=(AGENT)&v=Q0.4&ref=quazaa").toString();
	quazaaSettings.WebServicesBitziWebView = m_qSettings.value("BitziWebView", "http://bitzi.com/lookup/(URN)?v=detail&ref=quazaa").toString();
	quazaaSettings.WebServicesBitziXML = m_qSettings.value("BitziXML", "http://ticket.bitzi.com/rdf/(SHA1).(TTH)").toString();
	quazaaSettings.WebServicesShareMonkeyCid = m_qSettings.value("ShareMonkeyCid", "197506").toString();
	quazaaSettings.WebServicesShareMonkeyOkay = m_qSettings.value("ShareMonkeyOkay", false).toBool();
	quazaaSettings.WebServicesShareMonkeySaveThumbnail = m_qSettings.value("ShareMonkeySaveThumbnail", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Transfers");
	quazaaSettings.TransfersBandwidthDownloads = m_qSettings.value("Downloads", 1500.0).toDouble();
	quazaaSettings.TransfersBandwidthHubIn = m_qSettings.value("HubIn", 0).toInt();
	quazaaSettings.TransfersBandwidthHubOut = m_qSettings.value("HubOut", 0).toInt();
	quazaaSettings.TransfersBandwidthHubUploads = m_qSettings.value("HubUploads", 40).toInt();
	quazaaSettings.TransfersBandwidthLeafIn = m_qSettings.value("LeafIn", 0).toInt();
	quazaaSettings.TransfersBandwidthLeafOut = m_qSettings.value("LeafOut", 0).toInt();
	quazaaSettings.TransfersBandwidthPeerIn = m_qSettings.value("PeerIn", 0).toInt();
	quazaaSettings.TransfersBandwidthPeerOut = m_qSettings.value("PeerOut", 0).toInt();
	quazaaSettings.TransfersBandwidthRequest = m_qSettings.value("Request", 32).toInt();
	quazaaSettings.TransfersBandwidthUdpOut = m_qSettings.value("UdpOut", 0).toInt();
	quazaaSettings.TransfersBandwidthUploads = m_qSettings.value("Uploads", 384.0).toDouble();
	quazaaSettings.TransfersMinTransfersRest = m_qSettings.value("MinTransfersRest", 15).toInt();
	quazaaSettings.TransfersRatesUnit = m_qSettings.value("RatesUnit", 1).toInt();
	quazaaSettings.TransfersRequireConnectedNetwork = m_qSettings.value("RequireConnectedNetwork", true).toBool();
	quazaaSettings.TransfersSimpleProgressBar = m_qSettings.value("SimpleProgressBar", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Downloads");
	quazaaSettings.DownloadsAllowBackwards = m_qSettings.value("AllowBackwards", true).toBool();
	quazaaSettings.DownloadsAutoClear = m_qSettings.value("AutoClear", false).toBool();
	quazaaSettings.DownloadsBufferSize = m_qSettings.value("BufferSize", 81920).toInt();
	quazaaSettings.DownloadsChunkSize = m_qSettings.value("ChunkSize", 524288).toInt();
	quazaaSettings.DownloadsChunkStrap = m_qSettings.value("ChunkStrap", 131072).toInt();
	quazaaSettings.DownloadsClearDelay = m_qSettings.value("ClearDelay", 30000).toInt();
	quazaaSettings.DownloadsCompletePath = m_qSettings.value("CompletePath", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).replace(QString("\\"), QString("/")) + "/Quazaa Downloads").toString();
	quazaaSettings.DownloadsConnectThrottle = m_qSettings.value("ConnectThrottle", 800).toInt();
	quazaaSettings.DownloadsDropFailedSourcesThreshold = m_qSettings.value("DropFailedSourcesThreshold", 20).toInt();
	quazaaSettings.DownloadsExpandDownloads = m_qSettings.value("ExpandDownloads", false).toBool();
	quazaaSettings.DownloadsFlushSD = m_qSettings.value("FlushSD", true).toBool();
	quazaaSettings.DownloadsIncompletePath = m_qSettings.value("IncompletePath", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/Quazaa/Incomplete").toString();
	quazaaSettings.DownloadsMaxAllowedFailures = m_qSettings.value("MaxAllowedFailures", 10).toInt();
	quazaaSettings.DownloadsMaxConnectingSources = m_qSettings.value("MaxConnectingSources", 8).toInt();
	quazaaSettings.DownloadsMaxFiles = m_qSettings.value("MaxFiles", 26).toInt();
	quazaaSettings.DownloadsMaxFileSearches = m_qSettings.value("MaxFileSearches", 2).toInt();
	quazaaSettings.DownloadsMaxReviews = m_qSettings.value("MaxReviews", 64).toInt();
	quazaaSettings.DownloadsMaxTransfers = m_qSettings.value("MaxTransfers", 100).toInt();
	quazaaSettings.DownloadsMaxTransfersPerFile = m_qSettings.value("MaxTransfersPerFile", 10).toInt();
	quazaaSettings.DownloadsMetadata = m_qSettings.value("Metadata", true).toBool();
	quazaaSettings.DownloadsMinSources = m_qSettings.value("MinSources", 1).toInt();
	quazaaSettings.DownloadsNeverDrop = m_qSettings.value("NeverDrop", false).toBool();
	quazaaSettings.DownloadsPushTimeout = m_qSettings.value("PushTimeout", 45000).toInt();
	quazaaSettings.DownloadsQueueLimit = m_qSettings.value("QueueLimit", 3).toInt();
	quazaaSettings.DownloadsRequestHash = m_qSettings.value("RequestHash", true).toBool();
	quazaaSettings.DownloadsRequestHTTP11 = m_qSettings.value("RequestHTTP11", true).toBool();
	quazaaSettings.DownloadsRequestURLENC = m_qSettings.value("RequestURLENC", true).toBool();
	quazaaSettings.DownloadsRetryDelay = m_qSettings.value("RetryDelay", 600000).toInt();
	quazaaSettings.DownloadsSaveInterval = m_qSettings.value("SaveInterval", 60000).toInt();
	quazaaSettings.DownloadsSearchPeriod = m_qSettings.value("SearchPeriod", 120000).toInt();
	quazaaSettings.DownloadsShowPercent = m_qSettings.value("ShowPercent", false).toBool();
	quazaaSettings.DownloadsShowSources = m_qSettings.value("ShowSources", false).toBool();
	quazaaSettings.DownloadsSortColumns = m_qSettings.value("SortColumns", true).toBool();
	quazaaSettings.DownloadsSortSources = m_qSettings.value("SortSources", true).toBool();
	quazaaSettings.DownloadsSourcesWanted = m_qSettings.value("SourcesWanted", 500).toInt();
	quazaaSettings.DownloadsStaggardStart = m_qSettings.value("StaggardStart", false).toBool();
	quazaaSettings.DownloadsStarveGiveUp = m_qSettings.value("StarveGiveUp", 3).toInt();
	quazaaSettings.DownloadsStarveTimeout = m_qSettings.value("StarveTimeout", 2700).toInt();
	quazaaSettings.DownloadsURIPrompt = m_qSettings.value("URIPrompt", true).toBool();
	quazaaSettings.DownloadsVerifyED2K = m_qSettings.value("VerifyED2K", true).toBool();
	quazaaSettings.DownloadsVerifyFiles = m_qSettings.value("VerifyFiles", true).toBool();
	quazaaSettings.DownloadsVerifyTiger = m_qSettings.value("VerifyTiger", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Uploads");
	quazaaSettings.UploadsAllowBackwards = m_qSettings.value("AllowBackwards", true).toBool();
	quazaaSettings.UploadsAutoClear = m_qSettings.value("AutoClear", false).toBool();
	quazaaSettings.UploadsClampdownFactor = m_qSettings.value("ClampdownFactor", 20).toInt();
	quazaaSettings.UploadsClampdownFloor = m_qSettings.value("SharePartials", 8).toInt();
	quazaaSettings.UploadsClearDelay = m_qSettings.value("ClearDelay", 30).toInt();
	quazaaSettings.UploadsDynamicPreviews = m_qSettings.value("DynamicPreviews", true).toBool();
	quazaaSettings.UploadsFreeBandwidthFactor = m_qSettings.value("FreeBandwidthFactor", 15).toInt();
	quazaaSettings.UploadsFreeBandwidthValue = m_qSettings.value("FreeBandwidthValue", 20).toInt();
	quazaaSettings.UploadsHubShareLimiting = m_qSettings.value("HubShareLimiting", true).toBool();
	quazaaSettings.UploadsMaxPerHost = m_qSettings.value("MaxPerHost", 2).toInt();
	quazaaSettings.UploadsPreviewQuality = m_qSettings.value("PreviewQuality", 70).toInt();
	quazaaSettings.UploadsPreviewTransfers = m_qSettings.value("PreviewTransfers", 3).toInt();
	quazaaSettings.UploadsQueuePollMax = m_qSettings.value("QueuePollMax", 120000).toInt();
	quazaaSettings.UploadsQueuePollMin = m_qSettings.value("QueuePollMin", 45000).toInt();
	quazaaSettings.UploadsRewardQueuePercentage = m_qSettings.value("RewardQueuePercentage", 10).toInt();
	quazaaSettings.UploadsRotateChunkLimit = m_qSettings.value("RotateChunkLimit", 1024).toInt();
	quazaaSettings.UploadsShareHashset = m_qSettings.value("ShareHashset", true).toBool();
	quazaaSettings.UploadsShareMetadata = m_qSettings.value("ShareMetadata", true).toBool();
	quazaaSettings.UploadsSharePartials = m_qSettings.value("SharePartials", true).toBool();
	quazaaSettings.UploadsSharePreviews = m_qSettings.value("SharePreviews", true).toBool();
	quazaaSettings.UploadsShareTiger = m_qSettings.value("ShareTiger", true).toBool();
	quazaaSettings.UploadsThrottleMode = m_qSettings.value("ThrottleMode", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Security");
	quazaaSettings.SecurityAllowProfileBrowse = m_qSettings.value("AllowProfileBrowse", true).toBool();
	quazaaSettings.SecurityAllowSharesBrowse = m_qSettings.value("AllowSharesBrowse", true).toBool();
	quazaaSettings.SecurityBlockedAgentUploadFilter = m_qSettings.value("BlockedAgentUploadFilter", QStringList() << "Mozilla").toStringList();
	quazaaSettings.SecurityDeleteFirewallException = m_qSettings.value("DeleteFirewallException", true).toBool();
	quazaaSettings.SecurityDeleteUPnPPorts = m_qSettings.value("DeleteUPnPPorts", true).toBool();
	quazaaSettings.SecurityED2kChatFilter = m_qSettings.value("ED2kChatFilter", true).toBool();
	quazaaSettings.SecurityEnableFirewallException = m_qSettings.value("EnableFirewallException", true).toBool();
	quazaaSettings.SecurityEnableUPnP = m_qSettings.value("EnableUPnP", true).toBool();
	quazaaSettings.SecurityFirewallState = m_qSettings.value("FirewallState", 0).toInt();
	quazaaSettings.SecurityChatFilter = m_qSettings.value("ChatFilter", true).toBool();
	quazaaSettings.SecurityIrcFloodLimit = m_qSettings.value("IrcFloodLimit", 24).toInt();
	quazaaSettings.SecurityIrcFloodProtection = m_qSettings.value("IrcFloodProtection", true).toBool();
	quazaaSettings.SecurityRemoteEnable = m_qSettings.value("RemoteEnable", false).toBool();
	quazaaSettings.SecurityRemotePassword = m_qSettings.value("RemotePassword", "").toString();
	quazaaSettings.SecurityRemoteUsername = m_qSettings.value("RemoteUsername", "").toString();
	quazaaSettings.SecuritySearchIgnoreLocalIP = m_qSettings.value("SearchIgnoreLocalIP", true).toBool();
	quazaaSettings.SecuritySearchIgnoreOwnIP = m_qSettings.value("SearchIgnoreOwnIP", true).toBool();
	quazaaSettings.SecuritySearchSpamFilterThreshold = m_qSettings.value("SearchSpamFilterThreshold", 20).toInt();
	quazaaSettings.SecurityUPnPSkipWANIPSetup = m_qSettings.value("UPnPSkipWANIPSetup", false).toBool();
	quazaaSettings.SecurityUPnPSkipWANPPPSetup = m_qSettings.value("UPnPSkipWANPPPSetup", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella");
	quazaaSettings.GnutellaCompressHub2Hub = m_qSettings.value("CompressHub2Hub", true).toBool();
	quazaaSettings.GnutellaCompressHub2Leaf = m_qSettings.value("CompressHub2Leaf", true).toBool();
	quazaaSettings.GnutellaCompressLeaf2Hub = m_qSettings.value("CompressLeaf2Hub", false).toBool();
	quazaaSettings.GnutellaConnectFactor = m_qSettings.value("ConnectFactor", 3).toInt();
	quazaaSettings.GnutellaConnectThrottle = m_qSettings.value("ConnectThrottle", 120).toInt();
	quazaaSettings.GnutellaHitsPerPacket = m_qSettings.value("HitsPerPacket", 64).toInt();
	quazaaSettings.GnutellaHostCacheSize = m_qSettings.value("HostCacheSize", 1024).toInt();
	quazaaSettings.GnutellaHostCacheView = m_qSettings.value("HostCacheView", 3).toInt();
	quazaaSettings.GnutellaMaxHits = m_qSettings.value("MaxHits", 64).toInt();
	quazaaSettings.GnutellaMaxResults = m_qSettings.value("MaxResults", 150).toInt();
	quazaaSettings.GnutellaRouteCache = m_qSettings.value("RouteCache", 10).toInt();
	quazaaSettings.GnutellaSpecifyProtocol = m_qSettings.value("SpecifyProtocol", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella2");
	quazaaSettings.Gnutella2ClientMode = m_qSettings.value("ClientMode", 0).toInt();
	quazaaSettings.Gnutella2Enable = m_qSettings.value("Enable", true).toBool();
	quazaaSettings.Gnutella2HAWPeriod = m_qSettings.value("HAWPeriod", 300000).toInt();
	quazaaSettings.Gnutella2HostCount = m_qSettings.value("HostCount", 15).toInt();
	quazaaSettings.Gnutella2HostCurrent = m_qSettings.value("HostCurrent", 600).toInt();
	quazaaSettings.Gnutella2HostExpire = m_qSettings.value("HostExpire", 172800).toInt();
	quazaaSettings.Gnutella2HubHorizonSize = m_qSettings.value("HubHorizonSize", 128).toInt();
	quazaaSettings.Gnutella2HubVerified = m_qSettings.value("HubVerified", false).toBool();
	quazaaSettings.Gnutella2KHLHubCount = m_qSettings.value("KHLHubCount", 50).toInt();
	quazaaSettings.Gnutella2KHLPeriod = m_qSettings.value("KHLPeriod", 60000).toInt();
	quazaaSettings.Gnutella2LNIPeriod = m_qSettings.value("HubHorizonSize", 60000).toInt();
	quazaaSettings.Gnutella2NumHubs = m_qSettings.value("NumHubs", 2).toInt();
	quazaaSettings.Gnutella2NumLeafs = m_qSettings.value("NumLeafs", 300).toInt();
	quazaaSettings.Gnutella2NumPeers = m_qSettings.value("NumPeers", 6).toInt();
	quazaaSettings.Gnutella2PingRate = m_qSettings.value("PingRate", 15000).toInt();
	quazaaSettings.Gnutella2PingRelayLimit = m_qSettings.value("PingRelayLimit", 10).toInt();
	quazaaSettings.Gnutella2QueryGlobalThrottle = m_qSettings.value("QueryGlobalThrottle", 125).toInt();
	quazaaSettings.Gnutella2QueryHostDeadline = m_qSettings.value("QueryHostDeadline", 600).toInt();
	quazaaSettings.Gnutella2QueryHostThrottle = m_qSettings.value("QueryHostThrottle", 120).toInt();
	quazaaSettings.Gnutella2QueryLimit = m_qSettings.value("QueryLimit", 2400).toInt();
	quazaaSettings.Gnutella2RequeryDelay = m_qSettings.value("RequeryDelay", 14400).toInt();
	quazaaSettings.Gnutella2UdpBuffers = m_qSettings.value("UdpBuffers", 512).toInt();
	quazaaSettings.Gnutella2UdpGlobalThrottle = m_qSettings.value("UdpGlobalThrottle", 1).toInt();
	quazaaSettings.Gnutella2UdpInExpire = m_qSettings.value("UdpInExpire", 30000).toInt();
	quazaaSettings.Gnutella2UdpInFrames = m_qSettings.value("UdpInFrames", 256).toInt();
	quazaaSettings.Gnutella2UdpMTU = m_qSettings.value("UdpMTU", 500).toInt();
	quazaaSettings.Gnutella2UdpOutExpire = m_qSettings.value("UdpOutExpire", 26000).toInt();
	quazaaSettings.Gnutella2UdpOutFrames = m_qSettings.value("UdpOutFrames", 256).toInt();
	quazaaSettings.Gnutella2UdpOutResend = m_qSettings.value("UdpOutResend", 6000).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella1");
	quazaaSettings.Gnutella1ClientMode = m_qSettings.value("ClientMode", 1).toInt();
	quazaaSettings.Gnutella1DefaultTTL = m_qSettings.value("DefaultTTL", 3).toInt();
	quazaaSettings.Gnutella1Enable = m_qSettings.value("Enable", false).toBool();
	quazaaSettings.Gnutella1EnableDIPPSupport = m_qSettings.value("EnableDIPPSupport", true).toBool();
	quazaaSettings.Gnutella1EnableGGEP = m_qSettings.value("EnableGGEP", true).toBool();
	quazaaSettings.Gnutella1HitQueueLimit = m_qSettings.value("HitQueueLimit", 50).toInt();
	quazaaSettings.Gnutella1HostCount = m_qSettings.value("HostCount", 15).toInt();
	quazaaSettings.Gnutella1HostExpire = m_qSettings.value("HostExpire", 10).toInt();
	quazaaSettings.Gnutella1MaxHostsInPongs = m_qSettings.value("MaxHostsInPongs", 10).toInt();
	quazaaSettings.Gnutella1MaximumPacket = m_qSettings.value("MaximumPacket", 65535).toInt();
	quazaaSettings.Gnutella1MaximumQuery = m_qSettings.value("MaximumQuery", 256).toInt();
	quazaaSettings.Gnutella1MaximumTTL = m_qSettings.value("MaximumTTL", 10).toInt();
	quazaaSettings.Gnutella1NumHubs = m_qSettings.value("NumHubs", 3).toInt();
	quazaaSettings.Gnutella1NumLeafs = m_qSettings.value("NumLeafs", 50).toInt();
	quazaaSettings.Gnutella1NumPeers = m_qSettings.value("NumPeers", 32).toInt();
	quazaaSettings.Gnutella1PacketBufferSize = m_qSettings.value("PacketBufferSize", 64).toInt();
	quazaaSettings.Gnutella1PacketBufferTime = m_qSettings.value("PacketBufferTime", 60).toInt();
	quazaaSettings.Gnutella1PingFlood = m_qSettings.value("PingFlood", 0).toInt();
	quazaaSettings.Gnutella1PingRate = m_qSettings.value("PingRate", 15).toInt();
	quazaaSettings.Gnutella1PongCache = m_qSettings.value("PongCache", 1).toInt();
	quazaaSettings.Gnutella1PongCount = m_qSettings.value("PongCount", 1).toInt();
	quazaaSettings.Gnutella1QueryHitUTF8 = m_qSettings.value("QueryHitUTF8", true).toBool();
	quazaaSettings.Gnutella1QuerySearchUTF8 = m_qSettings.value("QuerySearchUTF8", true).toBool();
	quazaaSettings.Gnutella1QueryThrottle = m_qSettings.value("QueryThrottle", 30).toInt();
	quazaaSettings.Gnutella1RequeryDelay = m_qSettings.value("RequeryDelay", 30).toInt();
	quazaaSettings.Gnutella1SearchTTL = m_qSettings.value("SearchTTL", 3).toInt();
	quazaaSettings.Gnutella1StrictPackets = m_qSettings.value("StrictPackets", false).toBool();
	quazaaSettings.Gnutella1TranslateTTL = m_qSettings.value("TranslateTTL", 2).toInt();
	quazaaSettings.Gnutella1VendorMsg = m_qSettings.value("VendorMsg", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Ares");
	quazaaSettings.AresEnable = m_qSettings.value("Enable", true).toBool();
	quazaaSettings.AresMaximumDownloads = m_qSettings.value("MaximumDownloads", 10).toInt();
	quazaaSettings.AresMaximumUploads = m_qSettings.value("MaximumUploads", 6).toInt();
	quazaaSettings.AresMaximumUploadsPerUser = m_qSettings.value("MaximumUploadsPerUser", 3).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("eDonkey");
	quazaaSettings.EDonkeyDefaultServerFlags = m_qSettings.value("DefaultServerFlags", "ffffffff").toString();
	quazaaSettings.EDonkeyDequeueTime = m_qSettings.value("DequeueTime", 60).toInt();
	quazaaSettings.EDonkeyEnable = m_qSettings.value("Enable", false).toBool();
	quazaaSettings.EDonkeyEnableKad = m_qSettings.value("EnableKad", false).toBool();
	quazaaSettings.EDonkeyEndgame = m_qSettings.value("Endgame", true).toBool();
	quazaaSettings.EDonkeyExtendedRequest = m_qSettings.value("ExtendedRequest", 2).toInt();
	quazaaSettings.EDonkeyFastConnect = m_qSettings.value("FastConnect", false).toBool();
	quazaaSettings.EDonkeyForceHighID = m_qSettings.value("ForceHighID", true).toBool();
	quazaaSettings.EDonkeyFrameSize = m_qSettings.value("FrameSize", 10).toInt();
	quazaaSettings.EDonkeyGetSourcesThrottle = m_qSettings.value("GetSourcesThrottle", 8).toInt();
	quazaaSettings.EDonkeyLargeFileSupport = m_qSettings.value("LargeFileSupport", false).toBool();
	quazaaSettings.EDonkeyLearnNewServers = m_qSettings.value("LearnNewServers", false).toBool();
	quazaaSettings.EDonkeyLearnNewServersClient = m_qSettings.value("LearnNewServersClient", false).toBool();
	quazaaSettings.EDonkeyMagnetSearch = m_qSettings.value("MagnetSearch", true).toBool();
	quazaaSettings.EDonkeyMaxClients = m_qSettings.value("MaxClients", 35).toInt();
	quazaaSettings.EDonkeyMaxResults = m_qSettings.value("MaxResults", 100).toInt();
	quazaaSettings.EDonkeyMaxShareCount = m_qSettings.value("MaxShareCount", 1000).toInt();
	quazaaSettings.EDonkeyMetAutoQuery = m_qSettings.value("MetAutoQuery", true).toBool();
	quazaaSettings.EDonkeyMinServerFileSize = m_qSettings.value("MinServerFileSize", 0).toInt();
	quazaaSettings.EDonkeyNumServers = m_qSettings.value("NumServers", 1).toInt();
	quazaaSettings.EDonkeyPacketThrottle = m_qSettings.value("PacketThrottle", 500).toInt();
	quazaaSettings.EDonkeyQueryFileThrottle = m_qSettings.value("QueryFileThrottle", 60).toInt();
	quazaaSettings.EDonkeyQueryGlobalThrottle = m_qSettings.value("QueryGlobalThrottle", 1000).toInt();
	quazaaSettings.EDonkeyQueueRankThrottle = m_qSettings.value("QueueRankThrottle", 120).toInt();
	quazaaSettings.EDonkeyQueryServerThrottle = m_qSettings.value("QueryServerThrottle", 120).toInt();
	quazaaSettings.EDonkeyReAskTime = m_qSettings.value("ReAskTime", 29).toInt();
	quazaaSettings.EDonkeyRequestPipe = m_qSettings.value("RequestPipe", 3).toInt();
	quazaaSettings.EDonkeyRequestSize = m_qSettings.value("RequestSize", 90).toInt();
	quazaaSettings.EDonkeySearchCachedServers = m_qSettings.value("SearchCachedServers", true).toBool();
	quazaaSettings.EDonkeySendPortServer = m_qSettings.value("SendPortServer", false).toBool();
	quazaaSettings.EDonkeyServerListURL = m_qSettings.value("ServerListURL", "http://ocbmaurice.dyndns.org/pl/slist.pl/server.met?download/server-best.met").toString();
	quazaaSettings.EDonkeyServerWalk = m_qSettings.value("ServerWalk", true).toBool();
	quazaaSettings.EDonkeySourceThrottle = m_qSettings.value("SourceThrottle", 1000).toInt();
	quazaaSettings.EDonkeyStatsGlobalThrottle = m_qSettings.value("StatsGlobalThrottle", 30).toInt();
	quazaaSettings.EDonkeyStatsServerThrottle = m_qSettings.value("StatsServerThrottle", 7).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("BitTorrent");
	quazaaSettings.BitTorrentAutoClear = m_qSettings.value("AutoClear", false).toBool();
	quazaaSettings.BitTorrentAutoSeed = m_qSettings.value("AutoSeed", true).toBool();
	quazaaSettings.BitTorrentBandwidthPercentage = m_qSettings.value("BandwidthPercentage", 80).toInt();
	quazaaSettings.BitTorrentClearRatio = m_qSettings.value("ClearRatio", 120).toInt();
	quazaaSettings.BitTorrentCodePage = m_qSettings.value("CodePage", 0).toInt();
	quazaaSettings.BitTorrentDefaultTracker = m_qSettings.value("DefaultTracker", "").toString();
	quazaaSettings.BitTorrentDefaultTrackerPeriod = m_qSettings.value("DefaultTrackerPeriod", 5).toInt();
	quazaaSettings.BitTorrentDhtPruneTime = m_qSettings.value("DhtPruneTime", 30).toInt();
	quazaaSettings.BitTorrentDownloadConnections = m_qSettings.value("DownloadConnections", 40).toInt();
	quazaaSettings.BitTorrentDownloadTorrents = m_qSettings.value("DownloadTorrents", 4).toInt();
	quazaaSettings.BitTorrentEndgame = m_qSettings.value("Endgame", true).toBool();
	quazaaSettings.BitTorrentExtraKeys = m_qSettings.value("ExtraKeys", true).toBool();
	quazaaSettings.BitTorrentLinkPing = m_qSettings.value("LinkPing", 120).toInt();
	quazaaSettings.BitTorrentLinkTimeout = m_qSettings.value("LinkTimeout", 180).toInt();
	quazaaSettings.BitTorrentManaged = m_qSettings.value("Managed", true).toBool();
	quazaaSettings.BitTorrentPreferBTSources = m_qSettings.value("PreferBTSources", true).toBool();
	quazaaSettings.BitTorrentRandomPeriod = m_qSettings.value("RandomPeriod", 30).toInt();
	quazaaSettings.BitTorrentRequestLimit = m_qSettings.value("RequestLimit", 128).toInt();
	quazaaSettings.BitTorrentRequestPipe = m_qSettings.value("RequestPipe", 4).toInt();
	quazaaSettings.BitTorrentRequestSize = m_qSettings.value("RequestSize", 16).toInt();
	quazaaSettings.BitTorrentShowFilesInDownload = m_qSettings.value("ShowFilesInDownload", true).toBool();
	quazaaSettings.BitTorrentSourceExchangePeriod = m_qSettings.value("SourceExchangePeriod", 10).toInt();
	quazaaSettings.BitTorrentStartPaused = m_qSettings.value("StartPaused", false).toBool();
	quazaaSettings.BitTorrentTestPartials = m_qSettings.value("TestPartials", true).toBool();
	quazaaSettings.BitTorrentTorrentPath = m_qSettings.value("TorrentPath", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/Quazaa/Torrents").toString();
	quazaaSettings.BitTorrentTrackerKey = m_qSettings.value("TrackerKey", true).toBool();
	quazaaSettings.BitTorrentUploadCount = m_qSettings.value("UploadCount", 4).toInt();
	quazaaSettings.BitTorrentUseKademlia = m_qSettings.value("UseKademlia", true).toBool();
	quazaaSettings.BitTorrentUseSaveDialog = m_qSettings.value("UseSaveDialog", false).toBool();
	quazaaSettings.BitTorrentUseTemp = m_qSettings.value("UseTemp", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Discovery");
	quazaaSettings.DiscoveryAccessThrottle = m_qSettings.value("AccessThrottle", 60).toInt();
	quazaaSettings.DiscoveryBootstrapCount = m_qSettings.value("BootstrapCount", 10).toInt();
	quazaaSettings.DiscoveryCacheCount = m_qSettings.value("CacheCount", 50).toInt();
	quazaaSettings.DiscoveryDefaultUpdate = m_qSettings.value("DefaultUpdate", 60).toInt();
	quazaaSettings.DiscoveryEnableG1GWC = m_qSettings.value("EnableG1GWC", false).toBool();
	quazaaSettings.DiscoveryFailureLimit = m_qSettings.value("FailureLimit", 2).toInt();
	quazaaSettings.DiscoveryLowpoint = m_qSettings.value("Lowpoint", 10).toInt();
	quazaaSettings.DiscoveryUpdatePeriod = m_qSettings.value("UpdatePeriod", 30).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Scheduler");

	m_qSettings.endGroup();
}

void QuazaaSettings::saveProfile()
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	m_qSettings.beginGroup("Profile");
	m_qSettings.setValue( "Age", quazaaSettings.ProfileAge );
	m_qSettings.setValue( "AolScreenName", quazaaSettings.ProfileAolScreenName );							// Aol screen name
	m_qSettings.setValue( "AvatarPath", quazaaSettings.ProfileAvatarPath );								// Path to an avatar image file. Quazaa logo used if blank
	m_qSettings.setValue( "Biography", quazaaSettings.ProfileBiography );								// Your life story..
	m_qSettings.setValue( "City", quazaaSettings.ProfileCity );									// Your city
	m_qSettings.setValue( "Country", quazaaSettings.ProfileCountry );								// Your country..
	m_qSettings.setValue( "Email", quazaaSettings.ProfileEmail );									// EMail address
	m_qSettings.setValue( "Favorites", quazaaSettings.ProfileFavorites );								// Favorite websites
	m_qSettings.setValue( "FavoritesURL", quazaaSettings.ProfileFavoritesURL );							// Favorite Websites URLs
	m_qSettings.setValue( "Gender", quazaaSettings.ProfileGender );									// Gender..
	m_qSettings.setValue( "GnutellaScreenName", quazaaSettings.ProfileGnutellaScreenName );						// The name displayed in searches, uploads and downloads and when users browse your system
	m_qSettings.setValue( "GUID", quazaaSettings.ProfileGUID );									// Unique ID for each client. Can be regenerated
	m_qSettings.setValue( "ICQuin", quazaaSettings.ProfileICQuin );									// Identification number in ICQ
	m_qSettings.setValue( "Interests", quazaaSettings.ProfileInterests );								// What do you like to do other than use Quazaa
	m_qSettings.setValue( "IrcAlternateNickname", quazaaSettings.ProfileIrcAlternateNickname );					// Alternate nickname in Irc chat if first one is already used
	m_qSettings.setValue( "IrcNickname", quazaaSettings.ProfileIrcNickname );							// Nickname used in Irc chat
	m_qSettings.setValue( "IrcUserName", quazaaSettings.ProfileIrcUserName );							// User name for Irc chat
	m_qSettings.setValue( "JabberID", quazaaSettings.ProfileJabberID );								// Jabber ID
	m_qSettings.setValue( "Latitude", quazaaSettings.ProfileLatitude );								// Location for a disgruntled P2P user to aim a missile
	m_qSettings.setValue( "Longitude", quazaaSettings.ProfileLongitude );								// Location for a disgruntled P2P user to aim a missile
	m_qSettings.setValue( "MSNPassport", quazaaSettings.ProfileMSNPassport );							// Microsoft's Messenger ID
	m_qSettings.setValue( "MyspaceProfile", quazaaSettings.ProfileMyspaceProfile );							// Myspace profile excluding http://quazaaSettings.Profilemyspace.com/
	m_qSettings.setValue( "RealName", quazaaSettings.ProfileRealName );								// User's real name
	m_qSettings.setValue( "StateProvince", quazaaSettings.ProfileStateProvince );							// Your state or province
	m_qSettings.setValue( "YahooID", quazaaSettings.ProfileYahooID );								// Yahoo Messenger ID
	m_qSettings.endGroup();
}

void QuazaaSettings::loadProfile()
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	m_qSettings.beginGroup("Profile");

	quazaaSettings.ProfileAge = m_qSettings.value("Age", 18).toInt();
	quazaaSettings.ProfileAolScreenName = m_qSettings.value("AolScreenName", "").toString();
	quazaaSettings.ProfileAvatarPath = m_qSettings.value("AvatarPath", ":/Resource/Chat/DefaultAvatar.png").toString();
	quazaaSettings.ProfileBiography = m_qSettings.value("Biography", "").toString();
	quazaaSettings.ProfileCity = m_qSettings.value("City", "").toString();
	quazaaSettings.ProfileCountry = m_qSettings.value("Country", "").toString();
	quazaaSettings.ProfileEmail = m_qSettings.value("Email", "").toString();
	quazaaSettings.ProfileFavorites = m_qSettings.value("Favorites", QStringList()).toStringList();
	quazaaSettings.ProfileFavoritesURL = m_qSettings.value("FavoritesURL", QStringList()).toStringList();
	quazaaSettings.ProfileGender = m_qSettings.value("Gender", 0).toInt();
	quazaaSettings.ProfileGnutellaScreenName = m_qSettings.value("GnutellaScreenName", tr("Quazaa User")).toString();
	quazaaSettings.ProfileGUID = m_qSettings.value("GUID", QUuid::createUuid().toString().remove(QRegExp("[{}]"))).toString();
	quazaaSettings.ProfileICQuin = m_qSettings.value("ICQuin", "").toString();
	quazaaSettings.ProfileInterests = m_qSettings.value("Interests", QStringList()).toStringList();
	quazaaSettings.ProfileIrcAlternateNickname = m_qSettings.value("IrcAlternateNickname", "").toString();
	quazaaSettings.ProfileIrcNickname = m_qSettings.value("IrcNickname", tr("Quazaa User")).toString();
	quazaaSettings.ProfileIrcUserName = m_qSettings.value("IrcUserName", tr("QuazaaUser")).toString();
	quazaaSettings.ProfileJabberID = m_qSettings.value("JabberID", "").toString();
	quazaaSettings.ProfileLatitude = m_qSettings.value("Latitude", "").toString();
	quazaaSettings.ProfileLongitude = m_qSettings.value("Longitude", "").toString();
	quazaaSettings.ProfileMSNPassport = m_qSettings.value("MSNPassport", "").toString();
	quazaaSettings.ProfileMyspaceProfile = m_qSettings.value("MyspaceProfile", "").toString();
	quazaaSettings.ProfileRealName = m_qSettings.value("RealName", "").toString();
	quazaaSettings.ProfileStateProvince = m_qSettings.value("StateProvince", "").toString();
	quazaaSettings.ProfileYahooID = m_qSettings.value("YahooID", "").toString();
	m_qSettings.endGroup();
}

void QuazaaSettings::saveSkinWindowSettings(QSkinDialog *skinDialog)
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	m_qSettings.setValue( "NormalPositionAndSize", skinDialog->dialogNormalGeometry() );
	m_qSettings.setValue( "WindowMinimized", skinDialog->isMinimized() );
	m_qSettings.setValue( "WindowMaximized", skinDialog->isMaximized() );
	m_qSettings.setValue( "WindowVisible", skinDialog->isVisible() );
}

void QuazaaSettings::loadSkinWindowSettings(QSkinDialog *skinDialog)
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	skinDialog->setGeometry(m_qSettings.value( "NormalPositionAndSize", QRect( QPoint(200,200), QSize(535,104) ) ).toRect());
	skinDialog->setMaximized(m_qSettings.value("WindowMaximized", false).toBool());
	skinDialog->setMinimized(m_qSettings.value("WindowMinimized", false).toBool());
	quazaaSettings.MainWindowVisible = m_qSettings.value( "WindowVisible", true ).toBool();
}

void QuazaaSettings::saveWindowSettings(QMainWindow *window)
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	m_qSettings.setValue( "ActiveTab", quazaaSettings.MainWindowActiveTab );
	m_qSettings.setValue( "ChatRoomsTaskVisible", quazaaSettings.MainWindowChatRoomsTaskVisible );
	m_qSettings.setValue( "ChatFriendsTaskVisible", quazaaSettings.MainWindowChatFriendsTaskVisible );
	m_qSettings.setValue( "HomeConnectionTaskVisible", quazaaSettings.MainWindowHomeConnectionTaskVisible );
	m_qSettings.setValue( "HomeDownloadsTaskVisible", quazaaSettings.MainWindowHomeDownloadsTaskVisible );
	m_qSettings.setValue( "HomeLibraryTaskVisible", quazaaSettings.MainWindowHomeLibraryTaskVisible );
	m_qSettings.setValue( "HomeSearchString", quazaaSettings.MainWindowHomeSearchString );
	m_qSettings.setValue( "HomeTorrentsTaskVisible", quazaaSettings.MainWindowHomeTorrentsTaskVisible );
	m_qSettings.setValue( "HomeUploadsTaskVisible", quazaaSettings.MainWindowHomeUploadsTaskVisible );
	m_qSettings.setValue( "LibraryDetailsSplitter", quazaaSettings.MainWindowLibraryDetailsSplitter );
	m_qSettings.setValue( "LibraryDetailsVisible", quazaaSettings.MainWindowLibraryDetailsVisible );
	m_qSettings.setValue( "LibraryNavigatorTab", quazaaSettings.MainWindowLibraryNavigatorTab );
	m_qSettings.setValue( "LibrarySplitter", quazaaSettings.MainWindowLibrarySplitter );
	m_qSettings.setValue( "MediaPlaylistVisible", quazaaSettings.MainWindowMediaPlaylistVisible );
	m_qSettings.setValue( "MediaSplitter", quazaaSettings.MainWindowMediaSplitter );
	m_qSettings.setValue( "NetworkSplitter", quazaaSettings.MainWindowNetworkSplitter );
	m_qSettings.setValue( "SearchDetailsSplitter", quazaaSettings.MainWindowSearchDetailsSplitter );
	m_qSettings.setValue( "SearchDetailsVisible", quazaaSettings.MainWindowSearchDetailsVisible );
	m_qSettings.setValue( "SearchFileTypeTaskVisible", quazaaSettings.MainWindowSearchFileTypeTaskVisible );
	m_qSettings.setValue( "SearchNetworksTaskVisible", quazaaSettings.MainWindowSearchNetworksTaskVisible );
	m_qSettings.setValue( "SearchResultsTaskVisible", quazaaSettings.MainWindowSearchResultsTaskVisible );
	m_qSettings.setValue( "SearchSidebarVisible", quazaaSettings.MainWindowSearchSidebarVisible );
	m_qSettings.setValue( "SearchTaskVisible", quazaaSettings.MainWindowSearchTaskVisible );
	m_qSettings.setValue( "TransfersSplitter", quazaaSettings.MainWindowTransfersSplitter );
}

void QuazaaSettings::loadWindowSettings(QMainWindow *window)
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	quazaaSettings.MainWindowActiveTab = m_qSettings.value( "ActiveTab", 0 ).toInt();
	quazaaSettings.MainWindowChatRoomsTaskVisible = m_qSettings.value( "ChatRoomsTaskVisible", true ).toBool();
	quazaaSettings.MainWindowChatFriendsTaskVisible = m_qSettings.value( "ChatFriendsTaskVisible", true ).toBool();
	quazaaSettings.MainWindowHomeLibraryTaskVisible = m_qSettings.value( "HomeLibraryTaskVisible", true ).toBool();
	quazaaSettings.MainWindowHomeDownloadsTaskVisible = m_qSettings.value( "HomeDownloadsTaskVisible", true ).toBool();
	quazaaSettings.MainWindowHomeUploadsTaskVisible = m_qSettings.value( "HomeUploadsTaskVisible", true ).toBool();
	quazaaSettings.MainWindowHomeConnectionTaskVisible = m_qSettings.value( "HomeConnectionTaskVisible", true ).toBool();
	quazaaSettings.MainWindowHomeTorrentsTaskVisible = m_qSettings.value( "HomeTorrentsTaskVisible", true ).toBool();
	quazaaSettings.MainWindowHomeSearchString = m_qSettings.value( "HomeSearchString", "" ).toString();
	quazaaSettings.MainWindowLibraryDetailsSplitter = m_qSettings.value( "LibraryDetailsSplitter", QByteArray() ).toByteArray();
	quazaaSettings.MainWindowLibrarySplitter = m_qSettings.value( "LibrarySplitter", QByteArray() ).toByteArray();
	quazaaSettings.MainWindowLibraryNavigatorTab = m_qSettings.value( "LibraryNavigatorTab", 0 ).toInt();
	quazaaSettings.MainWindowLibraryDetailsVisible = m_qSettings.value( "LibraryDetailsVisible", true ).toBool();
	quazaaSettings.MainWindowMediaSplitter = m_qSettings.value( "MediaSplitter", QByteArray() ).toByteArray();
	quazaaSettings.MainWindowNetworkSplitter = m_qSettings.value( "NetworkSplitter", QByteArray() ).toByteArray();
	quazaaSettings.MainWindowSearchDetailsSplitter = m_qSettings.value( "SearchDetailsSplitter", QByteArray() ).toByteArray();
	quazaaSettings.MainWindowSearchTaskVisible = m_qSettings.value( "SearchTaskVisible", true ).toBool();
	quazaaSettings.MainWindowSearchNetworksTaskVisible = m_qSettings.value( "SearchNetworksTaskVisible", true ).toBool();
	quazaaSettings.MainWindowSearchFileTypeTaskVisible = m_qSettings.value( "SearchFileTypeTaskVisible", true ).toBool();
	quazaaSettings.MainWindowSearchResultsTaskVisible = m_qSettings.value( "SearchResultsTaskVisible", true ).toBool();
	quazaaSettings.MainWindowMediaPlaylistVisible = m_qSettings.value( "MediaPlaylistVisible", true ).toBool();
	quazaaSettings.MainWindowSearchSidebarVisible = m_qSettings.value( "SearchSidebarVisible", true ).toBool();
	quazaaSettings.MainWindowSearchDetailsVisible = m_qSettings.value( "SearchDetailsVisible", true ).toBool();
	quazaaSettings.MainWindowTransfersSplitter = m_qSettings.value( "TransfersSplitter", QByteArray() ).toByteArray();
}

void QuazaaSettings::saveLanguageSettings()
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	m_qSettings.beginGroup("Language");
	m_qSettings.setValue("LanguageFile", quazaaSettings.LanguageFile);
	m_qSettings.endGroup();
}

void QuazaaSettings::loadLanguageSettings()
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );

	m_qSettings.beginGroup("Language");
	quazaaSettings.LanguageFile = m_qSettings.value("LanguageFile", ("quazaa_default_en")).toString();
	m_qSettings.endGroup();
}

void QuazaaSettings::saveFirstRun(bool firstRun)
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );
	m_qSettings.setValue("FirstRun", firstRun);
}

bool QuazaaSettings::FirstRun()
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );
	return m_qSettings.value("FirstRun", true).toBool();
}

void QuazaaSettings::saveSkinSettings()
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );
	m_qSettings.setValue("SkinFile", SkinFile);
}

void QuazaaSettings::loadSkinSettings()
{
	QSettings m_qSettings( quazaaGlobals.ApplicationOrganizationName(), quazaaGlobals.ApplicationName() );
	SkinFile = m_qSettings.value("SkinFile", qApp->applicationDirPath() + "/Skin/Greenery/Greenery.qsk").toString();
}
