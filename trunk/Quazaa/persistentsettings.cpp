//
// persistentsettings.cpp
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

#include "persistentsettings.h"
#include "quazaaglobals.h"
#include <QUuid>
#include <QSettings>
#include <QDesktopServices>
#include <QApplication>

PersistentSettings Settings;

PersistentSettings::PersistentSettings()
{
}

void PersistentSettings::saveMainWindowState(QMainWindow *mainWindow)
{
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);

	m_qSettings.beginGroup("MainWindow");
	m_qSettings.setValue( "NormalPositionAndSize", mainWindow->normalGeometry() );
	m_qSettings.setValue( "IsMinimized", mainWindow->isMinimized() );
	m_qSettings.setValue( "IsMaximized", mainWindow->isMaximized() );
	m_qSettings.setValue( "IsVisible", mainWindow->isVisible() );
	m_qSettings.setValue( "ActiveTab", Settings.MainWindowState.ActiveTab );
	m_qSettings.setValue( "LibraryTab", Settings.MainWindowState.LibraryTab );
	m_qSettings.setValue( "SearchType", Settings.MainWindowState.SearchType );
	m_qSettings.setValue( "HomeSearch", Settings.MainWindowState.HomeSearch );
	m_qSettings.setValue( "HomeSearchType", Settings.MainWindowState.HomeSearchType );
	m_qSettings.setValue( "PlaylistVisible", Settings.MainWindowState.PlaylistVisible );
	m_qSettings.setValue( "ShowSearch", Settings.MainWindowState.ShowSearch );
	m_qSettings.setValue( "ShowSearchDetails", Settings.MainWindowState.ShowSearchDetails );
	m_qSettings.sync();
	m_qSettings.endGroup();
}

void PersistentSettings::loadMainWindowState(QMainWindow *mainWindow)
{
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);
	QRect m_rWindowSizeAndPosition;

	m_qSettings.beginGroup("MainWindow");
	m_rWindowSizeAndPosition = m_qSettings.value("NormalPositionAndSize", QRect(QPoint(200,200), QSize(535,104))).toRect();
	mainWindow->move( m_rWindowSizeAndPosition.topLeft() );
	mainWindow->resize( m_rWindowSizeAndPosition.size() );
	if (m_qSettings.value("IsMaximized", false).toBool())
		mainWindow->setWindowState(Qt::WindowMaximized);
	if (m_qSettings.value("IsMinimized", false).toBool())
		mainWindow->setWindowState(Qt::WindowMinimized);
	Settings.MainWindowState.ActiveTab = m_qSettings.value( "ActiveTab", 0 ).toInt();
	Settings.MainWindowState.LibraryTab = m_qSettings.value( "LibraryTab", 0).toInt();
	Settings.MainWindowState.SearchType = m_qSettings.value( "SearchType", 0 ).toInt();
	Settings.MainWindowState.HomeSearch = m_qSettings.value("HomeSearch", "").toString();
	Settings.MainWindowState.HomeSearchType = m_qSettings.value( "HomeSearchType", 0).toInt();
	Settings.MainWindowState.PlaylistVisible = m_qSettings.value( "PlaylistVisible", true).toBool();
	Settings.MainWindowState.ShowSearch = m_qSettings.value( "ShowSearch", true).toBool();
	Settings.MainWindowState.ShowSearchDetails = m_qSettings.value( "ShowSearchDetails", true).toBool();
	m_qSettings.endGroup();
}

void PersistentSettings::loadSettings()
{
	// Load an instance of QSettings
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);

	m_qSettings.beginGroup("Live");
	Settings.Live.AdultWarning = m_qSettings.value("AdultWarning", false).toBool();
	Settings.Live.AutoClose = m_qSettings.value("AutoClose", false).toBool();
	Settings.Live.BandwidthScale = m_qSettings.value("BandwidthScale", 100).toInt();
	Settings.Live.DefaultED2KServersLoaded = m_qSettings.value("DefaultED2KServersLoaded", false).toBool();
	Settings.Live.DiskSpaceStop = m_qSettings.value("DiskSpaceStop", false).toBool();
	Settings.Live.DiskSpaceWarning = m_qSettings.value("DiskSpaceWarning", false).toBool();
	Settings.Live.DiskWriteWarning = m_qSettings.value("DiskWriteWarning", false).toBool();
	Settings.Live.DonkeyServerWarning = m_qSettings.value("DonkeyServerWarning", false).toBool();
	Settings.Live.LastDuplicateHash = m_qSettings.value("LastDuplicateHash", "").toString();
	Settings.Live.MaliciousWarning = m_qSettings.value("MaliciousWarning", false).toBool();
	Settings.Live.QueueLimitWarning = m_qSettings.value("QueueLimitWarning", false).toBool();
	Settings.Live.UploadLimitWarning = m_qSettings.value("UploadLimitWarning", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Basic");
	Settings.Basic.ChatTips = m_qSettings.value("ChatTips", true).toBool();
	Settings.Basic.CloseMode = m_qSettings.value("CloseMode", 0).toInt();
	Settings.Basic.ConnectOnStartup = m_qSettings.value("ConnectOnStartup", true).toBool();
	Settings.Basic.DefaultLanguage = m_qSettings.value("DefaultLanguage", true).toBool();
	Settings.Basic.DiskSpaceStop = m_qSettings.value("DiskSpaceStop", 25).toInt();
	Settings.Basic.DiskSpaceWarning = m_qSettings.value("DiskSpaceWarning", 500).toInt();
	Settings.Basic.DownloadsTips = m_qSettings.value("DownloadsTips", true).toBool();
	Settings.Basic.FirstRun = m_qSettings.value("FirstRun", true).toBool();
	Settings.Basic.GUIMode = m_qSettings.value("GUIMode", true).toBool();
	Settings.Basic.LanguageFile = m_qSettings.value("LanguageFile", "").toString();
	Settings.Basic.LibraryTips = m_qSettings.value("LibraryTips", true).toBool();
	Settings.Basic.MediaTips = m_qSettings.value("MediaTips", true).toBool();
	Settings.Basic.NeighboursTips = m_qSettings.value("NeighboursTips", true).toBool();
	Settings.Basic.SearchTips = m_qSettings.value("SearchTips", true).toBool();
	Settings.Basic.SkinFile = m_qSettings.value("SkinFile", "").toString();
	Settings.Basic.StartWithSystem = m_qSettings.value("StartWithSystem", false).toBool();
	Settings.Basic.TipDelay = m_qSettings.value("TipDelay", 600).toInt();
	Settings.Basic.TipLowResMode = m_qSettings.value("TipLowResMode", true).toBool();
	Settings.Basic.TipTransparency = m_qSettings.value("TipTransparency", 255).toInt();
	Settings.Basic.TrayMinimise = m_qSettings.value("TrayMinimise", false).toBool();
	Settings.Basic.UploadsTips = m_qSettings.value("UploadsTips", true).toBool();
	Settings.Basic.UsingSkin = m_qSettings.value("UsingSkin", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Parental");
	Settings.Parental.AdultFilter = m_qSettings.value("AdultFilter",
														 QStringList() << "shit" << "fuck" << "damn" << "bitch"
														 << "asshole" << "ass" << "pussy" << "cock" << "cunt" << "dick"
														 << "whore" << "slut" << "clit").toStringList();
	Settings.Parental.ChatAdultCensor = m_qSettings.value("ChatAdultCensor", true).toBool();
	Settings.Parental.FilterAdultSearchResults = m_qSettings.value("FilterAdultSearchResults", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Logging");
	Settings.Logging.DebugLog = m_qSettings.value("DebugLog", false).toBool();
	Settings.Logging.LogLevel = m_qSettings.value("LogLevel", 3).toInt();
	Settings.Logging.LogShowTimestamp = m_qSettings.value("LogShowTimestamp", true).toBool();
	Settings.Logging.MaxDebugLogSize = m_qSettings.value("MaxDebugLogSize", 10).toInt();
	Settings.Logging.SearchLog = m_qSettings.value("SearchLog", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Library");
	Settings.Library.FilterURI = m_qSettings.value("FilterURI", "").toString();
	Settings.Library.GhostFiles = m_qSettings.value("GhostFiles", true).toBool();
	Settings.Library.HashWindow = m_qSettings.value("HashWindow", true).toBool();
	Settings.Library.HighPriorityHashing = m_qSettings.value("HighPriorityHashing", false).toInt();
	Settings.Library.HighPriorityHashingSpeed = m_qSettings.value("HighPriorityHashingSpeed", 20).toBool();
	Settings.Library.HistoryDays = m_qSettings.value("HistoryDays", 3).toInt();
	Settings.Library.HistoryTotal = m_qSettings.value("HistoryTotal", 32).toInt();
	Settings.Library.LowPriorityHashingSpeed = m_qSettings.value("LowPriorityHashingSpeed", 2).toInt();
	Settings.Library.NeverShareTypes = m_qSettings.value("NeverShareTypes",
														 QStringList() << "vbs" << "js" << "jc!" << "fb!" << "bc!" << "!ut"
														 << "dbx" << "part" << "partial" << "pst" << "reget" << "getright"
														 << "pif" << "lnk" << "sd" << "url" << "wab" << "m4p" << "infodb"
														 << "racestats" << "chk" << "tmp" << "temp" << "ini" << "inf" << "log"
														 << "old" << "manifest" << "met" << "bak" << "$$$" << "---" << "~~~"
														 << "###" << "__incomplete___" << "wma" << "wmv").toStringList();
	Settings.Library.PartialMatch = m_qSettings.value("PartialMatch", true).toBool();
	Settings.Library.PreferAPETags = m_qSettings.value("PreferAPETags", true).toBool();
	Settings.Library.QueryRouteSize = m_qSettings.value("QueryRouteSize", 20).toInt();
	Settings.Library.RememberViews = m_qSettings.value("RememberViews", true).toBool();
	Settings.Library.SafeExecuteTypes = m_qSettings.value("SafeExecuteTypes",
														  QStringList() << "3gp" << "7z" << "aac" << "ace" << "ape" << "avi"
														  << "bmp" << "co" << "collection" << "flv" << "gif" << "iso"
														  << "jpg" << "jpeg" << "lit" << "mid" << "mov" << "m1v" << "m2v"
														  << "m3u" << "m4a" << "mkv" << "mp2" << "mp3" << "mp4" << "mpa"
														  << "mpe" << "mpg" << "mpeg" << "ogg" << "ogm" << "pdf" << "png"
														  << "qt" << "rar" << "rm" << "sks" << "tar" << "tgz" << "torrent"
														  << "txt" << "wav" << "zip").toStringList();
	Settings.Library.ScanAPE = m_qSettings.value("ScanAPE", true).toBool();
	Settings.Library.ScanASF = m_qSettings.value("ScanASF", true).toBool();
	Settings.Library.ScanAVI = m_qSettings.value("ScanAVI", true).toBool();
	Settings.Library.ScanCHM = m_qSettings.value("ScanCHM", true).toBool();
	Settings.Library.ScanEXE = m_qSettings.value("ScanEXE", true).toBool();
	Settings.Library.ScanImage = m_qSettings.value("ScanImage", true).toBool();
	Settings.Library.ScanMP3 = m_qSettings.value("ScanMP3", true).toBool();
	Settings.Library.ScanMPC = m_qSettings.value("ScanMPC", true).toBool();
	Settings.Library.ScanMPEG = m_qSettings.value("ScanMPEG", true).toBool();
	Settings.Library.ScanMSI = m_qSettings.value("ScanMSI", true).toBool();
	Settings.Library.ScanOGG = m_qSettings.value("ScanOGG", true).toBool();
	Settings.Library.ScanPDF = m_qSettings.value("ScanPDF", true).toBool();
	Settings.Library.SchemaURI = m_qSettings.value("SchemaURI", "http://www.limewire.com/schemas/audio.xsd").toString();
	Settings.Library.Shares = m_qSettings.value("Shares", QStringList() << QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).replace(QString("\\"), QString("/")) + "/Quazaa Downloads"
												<< QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/Quazaa/Torrents").toStringList();
	Settings.Library.ShowCoverArt = m_qSettings.value("ShowCoverArt", true).toBool();
	Settings.Library.SmartSeriesDetection = m_qSettings.value("SmartSeriesDetection", false).toBool();
	Settings.Library.SourceExpire = m_qSettings.value("SourceExpire", 86400).toInt();
	Settings.Library.SourceMesh = m_qSettings.value("SourceMesh", true).toBool();
	Settings.Library.ThumbSize = m_qSettings.value("ThumbSize", 96).toInt();
	Settings.Library.TigerHeight = m_qSettings.value("TigerHeight", 9).toInt();
	Settings.Library.TreeSize = m_qSettings.value("TreeSize", 200).toInt();
	Settings.Library.WatchFolders = m_qSettings.value("WatchFolders", true).toBool();
	Settings.Library.WatchFoldersTimeout = m_qSettings.value("WatchFoldersTimeout", 5).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("MediaPlayer");
	Settings.MediaPlayer.Aspect = m_qSettings.value("Aspect", 0).toInt();
	Settings.MediaPlayer.AudioVisualPlugin = m_qSettings.value("AudioVisualPlugin", "").toString();
	Settings.MediaPlayer.CustomPlayerPath = m_qSettings.value("CustomPlayerPath", "").toString();
	Settings.MediaPlayer.MediaHandler = m_qSettings.value("MediaHandler", 0).toInt();
	Settings.MediaPlayer.FileTypes = m_qSettings.value("FileTypes", QStringList() << "asx" << "wax" << "m3u" << "wvx" << "wmx"
													   << "asf" << "wav" << "snd" << "au" << "aif" << "aifc" << "aiff" << "mp3"
													   << "cda" << "mid" << "rmi" << "midi" << "avi" << "mpeg" << "mpg"
													   << "m1v" << "mp2" << "mpa" << "mpe").toStringList();
	Settings.MediaPlayer.ListVisible = m_qSettings.value("ListVisible", true).toBool();
	Settings.MediaPlayer.Mute = m_qSettings.value("Mute", false).toBool();
	Settings.MediaPlayer.Playlist = m_qSettings.value("Playlist", QStringList()).toStringList();
	Settings.MediaPlayer.Random = m_qSettings.value("Random", false).toBool();
	Settings.MediaPlayer.Repeat = m_qSettings.value("Repeat", false).toBool();
	Settings.MediaPlayer.StatusVisible = m_qSettings.value("StatusVisible", true).toBool();
	Settings.MediaPlayer.Volume = m_qSettings.value("Volume", 100).toInt();
	Settings.MediaPlayer.Zoom = m_qSettings.value("Zoom", 0).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Search");
	Settings.Search.BlankSchemaURI = m_qSettings.value("BlankSchemaURI", "").toString();
	Settings.Search.BrowseTreeSize = m_qSettings.value("BrowseTreeSize", 180).toInt();
	Settings.Search.ClearPrevious = m_qSettings.value("ClearPrevious", 0).toInt();
	Settings.Search.ExpandSearchMatches = m_qSettings.value("ExpandSearchMatches", false).toBool();
	Settings.Search.FilterMask = m_qSettings.value("FilterMask", 360).toInt();
	Settings.Search.HighlightNew = m_qSettings.value("HighlightNew", true).toBool();
	Settings.Search.LastSchemaURI = m_qSettings.value("LastSchemaURI", "").toString();
	Settings.Search.MaxPreviewLength = m_qSettings.value("MaxPreviewLength", 20480).toInt();
	Settings.Search.MonitorFilter = m_qSettings.value("MonitorFilter", "").toString();
	Settings.Search.MonitorQueue = m_qSettings.value("MonitorQueue", 128).toInt();
	Settings.Search.MonitorSchemaURI = m_qSettings.value("MonitorSchemaURI", "http://www.limewire.com/schemas/audio.xsd").toString();
	Settings.Search.SchemaTypes = m_qSettings.value("SchemaTypes", true).toBool();
	Settings.Search.SearchThrottle = m_qSettings.value("SearchThrottle", 200).toInt();
	Settings.Search.ShareMonkeyBaseURL = m_qSettings.value("ShareMonkeyBaseURL", "http://tools.sharemonkey.com/xml/").toString();
	Settings.Search.ShowNames = m_qSettings.value("ShowNames", true).toBool();
	Settings.Search.SwitchOnDownload = m_qSettings.value("SwitchOnDownload", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Chat");
	Settings.Chat.AwayMessageIdleTime = m_qSettings.value("AwayMessageIdleTime", 30).toInt();
	Settings.Chat.Background = m_qSettings.value("Background", "").toString();
	Settings.Chat.ConnectOnStartup = m_qSettings.value("ConnectOnStartup", false).toBool();
	Settings.Chat.ColorChatBackground = m_qSettings.value("ColorChatBackground", QColor(qRgb(255,255,255))).value<QColor>();
	Settings.Chat.ColorNormalText = m_qSettings.value("ColorNormalText", QColor(qRgb(0,0,0))).value<QColor>();
	Settings.Chat.ColorNoticesText = m_qSettings.value("ColorNoticesText", QColor(qRgb(255,0,0))).value<QColor>();
	Settings.Chat.ColorRoomActionsText = m_qSettings.value("ColorRoomActionsText", QColor(qRgb(0,170,0))).value<QColor>();
	Settings.Chat.ColorServerMessagesText = m_qSettings.value("ColorServerMessagesText", QColor(qRgb(0,0,255))).value<QColor>();
	Settings.Chat.ColorTopicsText = m_qSettings.value("ColorTopicsText", QColor(qRgb(170,85,127))).value<QColor>();
	Settings.Chat.EnableChatAllNetworks = m_qSettings.value("EnableChatAllNetworks", true).toBool();
	Settings.Chat.EnableFileTransfers = m_qSettings.value("EnableFileTransfers", true).toBool();
	Settings.Chat.GnutellaChatEnable = m_qSettings.value("GnutellaChatEnable", true).toBool();
	Settings.Chat.IrcServerName = m_qSettings.value("IrcServerName", "irc.p2pchat.net").toString();
	Settings.Chat.IrcServerPort = m_qSettings.value("IrcServerPort", 6667).toInt();
	Settings.Chat.ScreenFont = m_qSettings.value("ScreenFont", QFont()).value<QFont>();
	Settings.Chat.ShowTimestamp = m_qSettings.value("ShowTimestamp", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Profile");
	Settings.Profile.Age = m_qSettings.value("Age", 18).toInt();
	Settings.Profile.AolScreenName = m_qSettings.value("AolScreenName", "").toString();
	Settings.Profile.AvatarPath = m_qSettings.value("AvatarPath", ":/Chat/Graphics/Chat/DefaultAvatar.png").toString();
	Settings.Profile.Biography = m_qSettings.value("Biography", "").toString();
	Settings.Profile.City = m_qSettings.value("City", "").toString();
	Settings.Profile.Country = m_qSettings.value("Country", "").toString();
	Settings.Profile.Email = m_qSettings.value("Email", "").toString();
	Settings.Profile.Favorites = m_qSettings.value("Favorites", QStringList()).toStringList();
	Settings.Profile.FavoritesURL = m_qSettings.value("FavoritesURL", QStringList()).toStringList();
	Settings.Profile.Gender = m_qSettings.value("Gender", 0).toInt();
	Settings.Profile.GnutellaScreenName = m_qSettings.value("GnutellaScreenName", tr("Quazaa User")).toString();
	Settings.Profile.GUID = m_qSettings.value("GUID", QUuid::createUuid().toString().remove(QRegExp("[{}]"))).toString();
	Settings.Profile.ICQuin = m_qSettings.value("ICQuin", "").toString();
	Settings.Profile.Interests = m_qSettings.value("Interests", QStringList()).toStringList();
	Settings.Profile.IrcAlternateNickname = m_qSettings.value("IrcAlternateNickname", "").toString();
	Settings.Profile.IrcNickname = m_qSettings.value("IrcNickname", tr("Quazaa User")).toString();
	Settings.Profile.IrcUserName = m_qSettings.value("IrcUserName", tr("QuazaaUser")).toString();
	Settings.Profile.JabberID = m_qSettings.value("JabberID", "").toString();
	Settings.Profile.Latitude = m_qSettings.value("Latitude", "").toString();
	Settings.Profile.Longitude = m_qSettings.value("Longitude", "").toString();
	Settings.Profile.MSNPassport = m_qSettings.value("MSNPassport", "").toString();
	Settings.Profile.MyspaceProfile = m_qSettings.value("MyspaceProfile", "").toString();
	Settings.Profile.RealName = m_qSettings.value("RealName", "").toString();
	Settings.Profile.StateProvince = m_qSettings.value("StateProvince", "").toString();
	Settings.Profile.YahooID = m_qSettings.value("YahooID", "").toString();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Connection");
	Settings.Connection.CanAcceptIncoming = m_qSettings.value("CanAcceptIncoming", 0).toInt();
	Settings.Connection.ConnectThrottle = m_qSettings.value("ConnectThrottle", 250).toInt();
	Settings.Connection.DetectConnectionLoss = m_qSettings.value("DetectConnectionLoss", true).toBool();
	Settings.Connection.DetectConnectionReset = m_qSettings.value("DetectConnectionReset", false).toBool();
	Settings.Connection.FailureLimit = m_qSettings.value("FailureLimit", 3).toInt();
	Settings.Connection.FailurePenalty = m_qSettings.value("FailurePenalty", 300).toInt();
	Settings.Connection.InAddress = m_qSettings.value("InAddress", "").toString();
	Settings.Connection.InBind = m_qSettings.value("InBind", false).toBool();
	Settings.Connection.InSpeed = m_qSettings.value("InSpeed", 1050.00).toDouble();
	Settings.Connection.OutAddress = m_qSettings.value("OutAddress", "").toString();
	Settings.Connection.OutSpeed = m_qSettings.value("OutSpeed", 32.00).toDouble();
	Settings.Connection.Port = m_qSettings.value("Port", 6350).toInt();
	Settings.Connection.RandomPort = m_qSettings.value("RandomPort", false).toBool();
	Settings.Connection.SendBuffer = m_qSettings.value("SendBuffer", 2048).toInt();
	Settings.Connection.TimeoutConnect = m_qSettings.value("TimeoutConnect", 16).toInt();
	Settings.Connection.TimeoutHandshake = m_qSettings.value("TimeoutHandshake", 45).toInt();
	Settings.Connection.TimeoutTraffic = m_qSettings.value("TimeoutTraffic", 140).toInt();
	Settings.Connection.XPsp2PatchedConnect = m_qSettings.value("XPsp2PatchedConnect", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Web");
	Settings.Web.BrowserIntegration = m_qSettings.value("BrowserIntegration", false).toBool();
	Settings.Web.Ares = m_qSettings.value("Ares", true).toBool();
	Settings.Web.ED2K = m_qSettings.value("ED2K", true).toBool();
	Settings.Web.Foxy = m_qSettings.value("Foxy", true).toBool();
	Settings.Web.Gnutella = m_qSettings.value("Gnutella", true).toBool();
	Settings.Web.Magnet = m_qSettings.value("Magnet", true).toBool();
	Settings.Web.ManageDownloadTypes = m_qSettings.value("ManageDownloadTypes", QStringList() << "7z" << "ace" << "arj"
														 << "bin" << "exe" << "fml" << "grs" << "gz" << "hqx" << "iso"
														 << "lzh" << "mp3" << "msi" << "r0" << "rar" << "sit" << "tar"
														 << "tgz" << "z" << "zip").toStringList();
	Settings.Web.Piolet = m_qSettings.value("Piolet", true).toBool();
	Settings.Web.Torrent = m_qSettings.value("Torrent", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("WebServices");
	Settings.WebServices.BitziAgent = m_qSettings.value("BitziAgent", ".").toString();
	Settings.WebServices.BitziOkay = m_qSettings.value("BitziOkay", false).toBool();
	Settings.WebServices.BitziWebSubmit = m_qSettings.value("BitziWebSubmit", "http://bitzi.com/lookup/(SHA1).(TTH)?fl=(SIZE)&ff=(FIRST20)&fn=(NAME)&tag.ed2k.ed2khash=(ED2K)&(INFO)&a=(AGENT)&v=Q0.4&ref=quazaa").toString();
	Settings.WebServices.BitziWebView = m_qSettings.value("BitziWebView", "http://bitzi.com/lookup/(URN)?v=detail&ref=quazaa").toString();
	Settings.WebServices.BitziXML = m_qSettings.value("BitziXML", "http://ticket.bitzi.com/rdf/(SHA1).(TTH)").toString();
	Settings.WebServices.ShareMonkeyCid = m_qSettings.value("ShareMonkeyCid", "197506").toString();
	Settings.WebServices.ShareMonkeyOkay = m_qSettings.value("ShareMonkeyOkay", false).toBool();
	Settings.WebServices.ShareMonkeySaveThumbnail = m_qSettings.value("ShareMonkeySaveThumbnail", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Transfers");
	Settings.Transfers.BandwidthDownloads = m_qSettings.value("Downloads", 1500.0).toDouble();
	Settings.Transfers.BandwidthHubIn = m_qSettings.value("HubIn", 0).toInt();
	Settings.Transfers.BandwidthHubOut = m_qSettings.value("HubOut", 0).toInt();
	Settings.Transfers.BandwidthHubUploads = m_qSettings.value("HubUploads", 40).toInt();
	Settings.Transfers.BandwidthLeafIn = m_qSettings.value("LeafIn", 0).toInt();
	Settings.Transfers.BandwidthLeafOut = m_qSettings.value("LeafOut", 0).toInt();
	Settings.Transfers.BandwidthPeerIn = m_qSettings.value("PeerIn", 0).toInt();
	Settings.Transfers.BandwidthPeerOut = m_qSettings.value("PeerOut", 0).toInt();
	Settings.Transfers.BandwidthRequest = m_qSettings.value("Request", 32).toInt();
	Settings.Transfers.BandwidthUdpOut = m_qSettings.value("UdpOut", 0).toInt();
	Settings.Transfers.BandwidthUploads = m_qSettings.value("Uploads", 384.0).toDouble();
	Settings.Downloads.QueueLimit = m_qSettings.value("QueueLimit", 0).toInt();
	Settings.Transfers.MinTransfersRest = m_qSettings.value("MinTransfersRest", 15).toInt();
	Settings.Transfers.RatesUnit = m_qSettings.value("RatesUnit", 1).toInt();
	Settings.Transfers.RequireConnectedNetwork = m_qSettings.value("RequireConnectedNetwork", true).toBool();
	Settings.Transfers.SimpleProgressBar = m_qSettings.value("SimpleProgressBar", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Downloads");
	Settings.Downloads.AllowBackwards = m_qSettings.value("AllowBackwards", true).toBool();
	Settings.Downloads.AutoClear = m_qSettings.value("AutoClear", false).toBool();
	Settings.Downloads.BufferSize = m_qSettings.value("BufferSize", 81920).toInt();
	Settings.Downloads.ChunkSize = m_qSettings.value("ChunkSize", 524288).toInt();
	Settings.Downloads.ChunkStrap = m_qSettings.value("ChunkStrap", 131072).toInt();
	Settings.Downloads.ClearDelay = m_qSettings.value("ClearDelay", 30000).toInt();
	Settings.Downloads.CompletePath = m_qSettings.value("CompletePath", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).replace(QString("\\"), QString("/")) + "/Quazaa Downloads").toString();
	Settings.Downloads.ConnectThrottle = m_qSettings.value("ConnectThrottle", 800).toInt();
	Settings.Downloads.DropFailedSourcesThreshold = m_qSettings.value("DropFailedSourcesThreshold", 20).toInt();
	Settings.Downloads.ExpandDownloads = m_qSettings.value("ExpandDownloads", false).toBool();
	Settings.Downloads.FlushSD = m_qSettings.value("FlushSD", true).toBool();
	Settings.Downloads.IncompletePath = m_qSettings.value("IncompletePath", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/Quazaa/Incomplete").toString();
	Settings.Downloads.MaxAllowedFailures = m_qSettings.value("MaxAllowedFailures", 10).toInt();
	Settings.Downloads.MaxConnectingSources = m_qSettings.value("MaxConnectingSources", 8).toInt();
	Settings.Downloads.MaxFiles = m_qSettings.value("MaxFiles", 26).toInt();
	Settings.Downloads.MaxFileSearches = m_qSettings.value("MaxFileSearches", 2).toInt();
	Settings.Downloads.MaxReviews = m_qSettings.value("MaxReviews", 64).toInt();
	Settings.Downloads.MaxTransfers = m_qSettings.value("MaxTransfers", 100).toInt();
	Settings.Downloads.MaxTransfersPerFile = m_qSettings.value("MaxTransfersPerFile", 10).toInt();
	Settings.Downloads.Metadata = m_qSettings.value("Metadata", true).toBool();
	Settings.Downloads.MinSources = m_qSettings.value("MinSources", 1).toInt();
	Settings.Downloads.NeverDrop = m_qSettings.value("NeverDrop", false).toBool();
	Settings.Downloads.PushTimeout = m_qSettings.value("PushTimeout", 45000).toInt();
	Settings.Downloads.QueueLimit = m_qSettings.value("QueueLimit", 3).toInt();
	Settings.Downloads.RequestHash = m_qSettings.value("RequestHash", true).toBool();
	Settings.Downloads.RequestHTTP11 = m_qSettings.value("RequestHTTP11", true).toBool();
	Settings.Downloads.RequestURLENC = m_qSettings.value("RequestURLENC", true).toBool();
	Settings.Downloads.RetryDelay = m_qSettings.value("RetryDelay", 600000).toInt();
	Settings.Downloads.SaveInterval = m_qSettings.value("SaveInterval", 60000).toInt();
	Settings.Downloads.SearchPeriod = m_qSettings.value("SearchPeriod", 120000).toInt();
	Settings.Downloads.ShowPercent = m_qSettings.value("ShowPercent", false).toBool();
	Settings.Downloads.ShowSources = m_qSettings.value("ShowSources", false).toBool();
	Settings.Downloads.SortColumns = m_qSettings.value("SortColumns", true).toBool();
	Settings.Downloads.SortSources = m_qSettings.value("SortSources", true).toBool();
	Settings.Downloads.SourcesWanted = m_qSettings.value("SourcesWanted", 500).toInt();
	Settings.Downloads.StaggardStart = m_qSettings.value("StaggardStart", false).toBool();
	Settings.Downloads.StarveGiveUp = m_qSettings.value("StarveGiveUp", 3).toInt();
	Settings.Downloads.StarveTimeout = m_qSettings.value("StarveTimeout", 2700).toInt();
	Settings.Downloads.URIPrompt = m_qSettings.value("URIPrompt", true).toBool();
	Settings.Downloads.VerifyED2K = m_qSettings.value("VerifyED2K", true).toBool();
	Settings.Downloads.VerifyFiles = m_qSettings.value("VerifyFiles", true).toBool();
	Settings.Downloads.VerifyTiger = m_qSettings.value("VerifyTiger", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Uploads");
	Settings.Uploads.AllowBackwards = m_qSettings.value("AllowBackwards", true).toBool();
	Settings.Uploads.AutoClear = m_qSettings.value("AutoClear", false).toBool();
	Settings.Uploads.ClampdownFactor = m_qSettings.value("ClampdownFactor", 20).toInt();
	Settings.Uploads.ClampdownFloor = m_qSettings.value("SharePartials", 8).toInt();
	Settings.Uploads.ClearDelay = m_qSettings.value("ClearDelay", 30).toInt();
	Settings.Uploads.DynamicPreviews = m_qSettings.value("DynamicPreviews", true).toBool();
	Settings.Uploads.FreeBandwidthFactor = m_qSettings.value("FreeBandwidthFactor", 15).toInt();
	Settings.Uploads.FreeBandwidthValue = m_qSettings.value("FreeBandwidthValue", 20).toInt();
	Settings.Uploads.HubShareLimiting = m_qSettings.value("HubShareLimiting", true).toBool();
	Settings.Uploads.MaxPerHost = m_qSettings.value("MaxPerHost", 2).toInt();
	Settings.Uploads.PreviewQuality = m_qSettings.value("PreviewQuality", 70).toInt();
	Settings.Uploads.PreviewTransfers = m_qSettings.value("PreviewTransfers", 3).toInt();
	Settings.Uploads.QueuePollMax = m_qSettings.value("QueuePollMax", 120000).toInt();
	Settings.Uploads.QueuePollMin = m_qSettings.value("QueuePollMin", 45000).toInt();
	Settings.Uploads.RewardQueuePercentage = m_qSettings.value("RewardQueuePercentage", 10).toInt();
	Settings.Uploads.RotateChunkLimit = m_qSettings.value("RotateChunkLimit", 1024).toInt();
	Settings.Uploads.ShareHashset = m_qSettings.value("ShareHashset", true).toBool();
	Settings.Uploads.ShareMetadata = m_qSettings.value("ShareMetadata", true).toBool();
	Settings.Uploads.SharePartials = m_qSettings.value("SharePartials", true).toBool();
	Settings.Uploads.SharePreviews = m_qSettings.value("SharePreviews", true).toBool();
	Settings.Uploads.ShareTiger = m_qSettings.value("ShareTiger", true).toBool();
	Settings.Uploads.ThrottleMode = m_qSettings.value("ThrottleMode", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Security");
	Settings.Security.AllowProfileBrowse = m_qSettings.value("AllowProfileBrowse", true).toBool();
	Settings.Security.AllowSharesBrowse = m_qSettings.value("AllowSharesBrowse", true).toBool();
	Settings.Security.BlockedAgentUploadFilter = m_qSettings.value("BlockedAgentUploadFilter", QStringList() << "Mozilla").toStringList();
	Settings.Security.DeleteFirewallException = m_qSettings.value("DeleteFirewallException", true).toBool();
	Settings.Security.DeleteUPnPPorts = m_qSettings.value("DeleteUPnPPorts", true).toBool();
	Settings.Security.ED2kChatFilter = m_qSettings.value("ED2kChatFilter", true).toBool();
	Settings.Security.EnableFirewallException = m_qSettings.value("EnableFirewallException", true).toBool();
	Settings.Security.EnableUPnP = m_qSettings.value("EnableUPnP", true).toBool();
	Settings.Security.FirewallState = m_qSettings.value("FirewallState", 0).toInt();
	Settings.Security.ChatFilter = m_qSettings.value("ChatFilter", true).toBool();
	Settings.Security.IrcFloodLimit = m_qSettings.value("IrcFloodLimit", 24).toInt();
	Settings.Security.IrcFloodProtection = m_qSettings.value("IrcFloodProtection", true).toBool();
	Settings.Security.RemoteEnable = m_qSettings.value("RemoteEnable", false).toBool();
	Settings.Security.RemotePassword = m_qSettings.value("RemotePassword", "").toString();
	Settings.Security.RemoteUsername = m_qSettings.value("RemoteUsername", "").toString();
	Settings.Security.SearchIgnoreLocalIP = m_qSettings.value("SearchIgnoreLocalIP", true).toBool();
	Settings.Security.SearchIgnoreOwnIP = m_qSettings.value("SearchIgnoreOwnIP", true).toBool();
	Settings.Security.SearchSpamFilterThreshold = m_qSettings.value("SearchSpamFilterThreshold", 20).toInt();
	Settings.Security.UPnPSkipWANIPSetup = m_qSettings.value("UPnPSkipWANIPSetup", false).toBool();
	Settings.Security.UPnPSkipWANPPPSetup = m_qSettings.value("UPnPSkipWANPPPSetup", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella");
	Settings.Gnutella.CompressHub2Hub = m_qSettings.value("CompressHub2Hub", true).toBool();
	Settings.Gnutella.CompressHub2Leaf = m_qSettings.value("CompressHub2Leaf", true).toBool();
	Settings.Gnutella.CompressLeaf2Hub = m_qSettings.value("CompressLeaf2Hub", false).toBool();
	Settings.Gnutella.ConnectFactor = m_qSettings.value("ConnectFactor", 3).toInt();
	Settings.Gnutella.ConnectThrottle = m_qSettings.value("ConnectThrottle", 120).toInt();
	Settings.Gnutella.HitsPerPacket = m_qSettings.value("HitsPerPacket", 64).toInt();
	Settings.Gnutella.HostCacheSize = m_qSettings.value("HostCacheSize", 1024).toInt();
	Settings.Gnutella.HostCacheView = m_qSettings.value("HostCacheView", 3).toInt();
	Settings.Gnutella.MaxHits = m_qSettings.value("MaxHits", 64).toInt();
	Settings.Gnutella.MaxResults = m_qSettings.value("MaxResults", 150).toInt();
	Settings.Gnutella.RouteCache = m_qSettings.value("RouteCache", 10).toInt();
	Settings.Gnutella.SpecifyProtocol = m_qSettings.value("SpecifyProtocol", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella2");
	Settings.Gnutella2.ClientMode = m_qSettings.value("ClientMode", 0).toInt();
	Settings.Gnutella2.Enable = m_qSettings.value("Enable", true).toBool();
	Settings.Gnutella2.HAWPeriod = m_qSettings.value("HAWPeriod", 300000).toInt();
	Settings.Gnutella2.HostCount = m_qSettings.value("HostCount", 15).toInt();
	Settings.Gnutella2.HostCurrent = m_qSettings.value("HostCurrent", 600).toInt();
	Settings.Gnutella2.HostExpire = m_qSettings.value("HostExpire", 172800).toInt();
	Settings.Gnutella2.HubHorizonSize = m_qSettings.value("HubHorizonSize", 128).toInt();
	Settings.Gnutella2.HubVerified = m_qSettings.value("HubVerified", false).toBool();
	Settings.Gnutella2.KHLHubCount = m_qSettings.value("KHLHubCount", 50).toInt();
	Settings.Gnutella2.KHLPeriod = m_qSettings.value("KHLPeriod", 60000).toInt();
	Settings.Gnutella2.LNIPeriod = m_qSettings.value("HubHorizonSize", 60000).toInt();
	Settings.Gnutella2.NumHubs = m_qSettings.value("NumHubs", 2).toInt();
	Settings.Gnutella2.NumLeafs = m_qSettings.value("NumLeafs", 300).toInt();
	Settings.Gnutella2.NumPeers = m_qSettings.value("NumPeers", 6).toInt();
	Settings.Gnutella2.PingRate = m_qSettings.value("PingRate", 15000).toInt();
	Settings.Gnutella2.PingRelayLimit = m_qSettings.value("PingRelayLimit", 10).toInt();
	Settings.Gnutella2.QueryGlobalThrottle = m_qSettings.value("QueryGlobalThrottle", 125).toInt();
	Settings.Gnutella2.QueryHostDeadline = m_qSettings.value("QueryHostDeadline", 600).toInt();
	Settings.Gnutella2.QueryHostThrottle = m_qSettings.value("QueryHostThrottle", 120).toInt();
	Settings.Gnutella2.QueryLimit = m_qSettings.value("QueryLimit", 2400).toInt();
	Settings.Gnutella2.RequeryDelay = m_qSettings.value("RequeryDelay", 14400).toInt();
	Settings.Gnutella2.UdpBuffers = m_qSettings.value("UdpBuffers", 512).toInt();
	Settings.Gnutella2.UdpGlobalThrottle = m_qSettings.value("UdpGlobalThrottle", 1).toInt();
	Settings.Gnutella2.UdpInExpire = m_qSettings.value("UdpInExpire", 30000).toInt();
	Settings.Gnutella2.UdpInFrames = m_qSettings.value("UdpInFrames", 256).toInt();
	Settings.Gnutella2.UdpMTU = m_qSettings.value("UdpMTU", 500).toInt();
	Settings.Gnutella2.UdpOutExpire = m_qSettings.value("UdpOutExpire", 26000).toInt();
	Settings.Gnutella2.UdpOutFrames = m_qSettings.value("UdpOutFrames", 256).toInt();
	Settings.Gnutella2.UdpOutResend = m_qSettings.value("UdpOutResend", 6000).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella1");
	Settings.Gnutella1.ClientMode = m_qSettings.value("ClientMode", 1).toInt();
	Settings.Gnutella1.DefaultTTL = m_qSettings.value("DefaultTTL", 3).toInt();
	Settings.Gnutella1.Enable = m_qSettings.value("Enable", false).toBool();
	Settings.Gnutella1.EnableDIPPSupport = m_qSettings.value("EnableDIPPSupport", true).toBool();
	Settings.Gnutella1.EnableGGEP = m_qSettings.value("EnableGGEP", true).toBool();
	Settings.Gnutella1.HitQueueLimit = m_qSettings.value("HitQueueLimit", 50).toInt();
	Settings.Gnutella1.HostCount = m_qSettings.value("HostCount", 15).toInt();
	Settings.Gnutella1.HostExpire = m_qSettings.value("HostExpire", 10).toInt();
	Settings.Gnutella1.MaxHostsInPongs = m_qSettings.value("MaxHostsInPongs", 10).toInt();
	Settings.Gnutella1.MaximumPacket = m_qSettings.value("MaximumPacket", 65535).toInt();
	Settings.Gnutella1.MaximumQuery = m_qSettings.value("MaximumQuery", 256).toInt();
	Settings.Gnutella1.MaximumTTL = m_qSettings.value("MaximumTTL", 10).toInt();
	Settings.Gnutella1.NumHubs = m_qSettings.value("NumHubs", 3).toInt();
	Settings.Gnutella1.NumLeafs = m_qSettings.value("NumLeafs", 50).toInt();
	Settings.Gnutella1.NumPeers = m_qSettings.value("NumPeers", 32).toInt();
	Settings.Gnutella1.PacketBufferSize = m_qSettings.value("PacketBufferSize", 64).toInt();
	Settings.Gnutella1.PacketBufferTime = m_qSettings.value("PacketBufferTime", 60).toInt();
	Settings.Gnutella1.PingFlood = m_qSettings.value("PingFlood", 0).toInt();
	Settings.Gnutella1.PingRate = m_qSettings.value("PingRate", 15).toInt();
	Settings.Gnutella1.PongCache = m_qSettings.value("PongCache", 1).toInt();
	Settings.Gnutella1.PongCount = m_qSettings.value("PongCount", 1).toInt();
	Settings.Gnutella1.QueryHitUTF8 = m_qSettings.value("QueryHitUTF8", true).toBool();
	Settings.Gnutella1.QuerySearchUTF8 = m_qSettings.value("QuerySearchUTF8", true).toBool();
	Settings.Gnutella1.QueryThrottle = m_qSettings.value("QueryThrottle", 30).toInt();
	Settings.Gnutella1.RequeryDelay = m_qSettings.value("RequeryDelay", 30).toInt();
	Settings.Gnutella1.SearchTTL = m_qSettings.value("SearchTTL", 3).toInt();
	Settings.Gnutella1.StrictPackets = m_qSettings.value("StrictPackets", false).toBool();
	Settings.Gnutella1.TranslateTTL = m_qSettings.value("TranslateTTL", 2).toInt();
	Settings.Gnutella1.VendorMsg = m_qSettings.value("VendorMsg", true).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Ares");
	Settings.Ares.Enable = m_qSettings.value("Enable", true).toBool();
	Settings.Ares.MaximumDownloads = m_qSettings.value("MaximumDownloads", 10).toInt();
	Settings.Ares.MaximumUploads = m_qSettings.value("MaximumUploads", 6).toInt();
	Settings.Ares.MaximumUploadsPerUser = m_qSettings.value("MaximumUploadsPerUser", 3).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("eDonkey2k");
	Settings.eDonkey2k.DefaultServerFlags = m_qSettings.value("DefaultServerFlags", "ffffffff").toString();
	Settings.eDonkey2k.DequeueTime = m_qSettings.value("DequeueTime", 60).toInt();
	Settings.eDonkey2k.Enable = m_qSettings.value("Enable", false).toBool();
	Settings.eDonkey2k.EnableKad = m_qSettings.value("EnableKad", false).toBool();
	Settings.eDonkey2k.Endgame = m_qSettings.value("Endgame", true).toBool();
	Settings.eDonkey2k.ExtendedRequest = m_qSettings.value("ExtendedRequest", 2).toInt();
	Settings.eDonkey2k.FastConnect = m_qSettings.value("FastConnect", false).toBool();
	Settings.eDonkey2k.ForceHighID = m_qSettings.value("ForceHighID", true).toBool();
	Settings.eDonkey2k.FrameSize = m_qSettings.value("FrameSize", 10).toInt();
	Settings.eDonkey2k.GetSourcesThrottle = m_qSettings.value("GetSourcesThrottle", 8).toInt();
	Settings.eDonkey2k.LargeFileSupport = m_qSettings.value("LargeFileSupport", false).toBool();
	Settings.eDonkey2k.LearnNewServers = m_qSettings.value("LearnNewServers", false).toBool();
	Settings.eDonkey2k.LearnNewServersClient = m_qSettings.value("LearnNewServersClient", false).toBool();
	Settings.eDonkey2k.MagnetSearch = m_qSettings.value("MagnetSearch", true).toBool();
	Settings.eDonkey2k.MaxClients = m_qSettings.value("MaxClients", 35).toInt();
	Settings.eDonkey2k.MaxResults = m_qSettings.value("MaxResults", 100).toInt();
	Settings.eDonkey2k.MaxShareCount = m_qSettings.value("MaxShareCount", 1000).toInt();
	Settings.eDonkey2k.MetAutoQuery = m_qSettings.value("MetAutoQuery", true).toBool();
	Settings.eDonkey2k.MinServerFileSize = m_qSettings.value("MinServerFileSize", 0).toInt();
	Settings.eDonkey2k.NumServers = m_qSettings.value("NumServers", 1).toInt();
	Settings.eDonkey2k.PacketThrottle = m_qSettings.value("PacketThrottle", 500).toInt();
	Settings.eDonkey2k.QueryFileThrottle = m_qSettings.value("QueryFileThrottle", 60).toInt();
	Settings.eDonkey2k.QueryGlobalThrottle = m_qSettings.value("QueryGlobalThrottle", 1000).toInt();
	Settings.eDonkey2k.QueueRankThrottle = m_qSettings.value("QueueRankThrottle", 120).toInt();
	Settings.eDonkey2k.QueryServerThrottle = m_qSettings.value("QueryServerThrottle", 120).toInt();
	Settings.eDonkey2k.ReAskTime = m_qSettings.value("ReAskTime", 29).toInt();
	Settings.eDonkey2k.RequestPipe = m_qSettings.value("RequestPipe", 3).toInt();
	Settings.eDonkey2k.RequestSize = m_qSettings.value("RequestSize", 90).toInt();
	Settings.eDonkey2k.SearchCachedServers = m_qSettings.value("SearchCachedServers", true).toBool();
	Settings.eDonkey2k.SendPortServer = m_qSettings.value("SendPortServer", false).toBool();
	Settings.eDonkey2k.ServerListURL = m_qSettings.value("ServerListURL", "http://ocbmaurice.dyndns.org/pl/slist.pl/server.met?download/server-best.met").toString();
	Settings.eDonkey2k.ServerWalk = m_qSettings.value("ServerWalk", true).toBool();
	Settings.eDonkey2k.SourceThrottle = m_qSettings.value("SourceThrottle", 1000).toInt();
	Settings.eDonkey2k.StatsGlobalThrottle = m_qSettings.value("StatsGlobalThrottle", 30).toInt();
	Settings.eDonkey2k.StatsServerThrottle = m_qSettings.value("StatsServerThrottle", 7).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Bittorrent");
	Settings.Bittorrent.AutoClear = m_qSettings.value("AutoClear", false).toBool();
	Settings.Bittorrent.AutoSeed = m_qSettings.value("AutoSeed", true).toBool();
	Settings.Bittorrent.BandwidthPercentage = m_qSettings.value("BandwidthPercentage", 80).toInt();
	Settings.Bittorrent.ClearRatio = m_qSettings.value("ClearRatio", 120).toInt();
	Settings.Bittorrent.CodePage = m_qSettings.value("CodePage", 0).toInt();
	Settings.Bittorrent.DefaultTracker = m_qSettings.value("DefaultTracker", "").toString();
	Settings.Bittorrent.DefaultTrackerPeriod = m_qSettings.value("DefaultTrackerPeriod", 5).toInt();
	Settings.Bittorrent.DhtPruneTime = m_qSettings.value("DhtPruneTime", 30).toInt();
	Settings.Bittorrent.DownloadConnections = m_qSettings.value("DownloadConnections", 40).toInt();
	Settings.Bittorrent.DownloadTorrents = m_qSettings.value("DownloadTorrents", 4).toInt();
	Settings.Bittorrent.Endgame = m_qSettings.value("Endgame", true).toBool();
	Settings.Bittorrent.ExtraKeys = m_qSettings.value("ExtraKeys", true).toBool();
	Settings.Bittorrent.LinkPing = m_qSettings.value("LinkPing", 120).toInt();
	Settings.Bittorrent.LinkTimeout = m_qSettings.value("LinkTimeout", 180).toInt();
	Settings.Bittorrent.Managed = m_qSettings.value("Managed", true).toBool();
	Settings.Bittorrent.PreferBTSources = m_qSettings.value("PreferBTSources", true).toBool();
	Settings.Bittorrent.RandomPeriod = m_qSettings.value("RandomPeriod", 30).toInt();
	Settings.Bittorrent.RequestLimit = m_qSettings.value("RequestLimit", 128).toInt();
	Settings.Bittorrent.RequestPipe = m_qSettings.value("RequestPipe", 4).toInt();
	Settings.Bittorrent.RequestSize = m_qSettings.value("RequestSize", 16).toInt();
	Settings.Bittorrent.ShowFilesInDownload = m_qSettings.value("ShowFilesInDownload", true).toBool();
	Settings.Bittorrent.SourceExchangePeriod = m_qSettings.value("SourceExchangePeriod", 10).toInt();
	Settings.Bittorrent.StartPaused = m_qSettings.value("StartPaused", false).toBool();
	Settings.Bittorrent.TestPartials = m_qSettings.value("TestPartials", true).toBool();
	Settings.Bittorrent.TorrentPath = m_qSettings.value("TorrentPath", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/Quazaa/Torrents").toString();
	Settings.Bittorrent.TrackerKey = m_qSettings.value("TrackerKey", true).toBool();
	Settings.Bittorrent.UploadCount = m_qSettings.value("UploadCount", 4).toInt();
	Settings.Bittorrent.UseKademlia = m_qSettings.value("UseKademlia", true).toBool();
	Settings.Bittorrent.UseSaveDialog = m_qSettings.value("UseSaveDialog", false).toBool();
	Settings.Bittorrent.UseTemp = m_qSettings.value("UseTemp", false).toBool();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Discovery");
	Settings.Discovery.AccessThrottle = m_qSettings.value("AccessThrottle", 60).toInt();
	Settings.Discovery.BootstrapCount = m_qSettings.value("BootstrapCount", 10).toInt();
	Settings.Discovery.CacheCount = m_qSettings.value("CacheCount", 50).toInt();
	Settings.Discovery.DefaultUpdate = m_qSettings.value("DefaultUpdate", 60).toInt();
	Settings.Discovery.EnableG1GWC = m_qSettings.value("EnableG1GWC", false).toBool();
	Settings.Discovery.FailureLimit = m_qSettings.value("FailureLimit", 2).toInt();
	Settings.Discovery.Lowpoint = m_qSettings.value("Lowpoint", 10).toInt();
	Settings.Discovery.UpdatePeriod = m_qSettings.value("UpdatePeriod", 30).toInt();
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Scheduler");

	m_qSettings.endGroup();
	return;
}

void PersistentSettings::saveSettings()
{
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);

	m_qSettings.beginGroup("Live");
	m_qSettings.setValue("AdultWarning", Settings.Live.AdultWarning);
	m_qSettings.setValue("AutoClose", Settings.Live.AutoClose);
	m_qSettings.setValue("BandwidthScale", Settings.Live.BandwidthScale);
	m_qSettings.setValue("DefaultED2KServersLoaded", Settings.Live.DefaultED2KServersLoaded);
	m_qSettings.setValue("DiskSpaceStop", Settings.Live.DiskSpaceStop);
	m_qSettings.setValue("DiskSpaceWarning", Settings.Live.DiskSpaceWarning);
	m_qSettings.setValue("DiskWriteWarning", Settings.Live.DiskWriteWarning);
	m_qSettings.setValue("DonkeyServerWarning", Settings.Live.DonkeyServerWarning);
	m_qSettings.setValue("LastDuplicateHash", Settings.Live.LastDuplicateHash);
	m_qSettings.setValue("MaliciousWarning", Settings.Live.MaliciousWarning);
	m_qSettings.setValue("QueueLimitWarning", Settings.Live.QueueLimitWarning);
	m_qSettings.setValue("UploadLimitWarning", Settings.Live.UploadLimitWarning);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Basic");
	m_qSettings.setValue("ChatTips", Settings.Basic.ChatTips);
	m_qSettings.setValue("CloseMode", Settings.Basic.CloseMode);
	m_qSettings.setValue("ConnectOnStartup", Settings.Basic.ConnectOnStartup);
	m_qSettings.setValue("DefaultLanguage", Settings.Basic.DefaultLanguage);
	m_qSettings.setValue("DiskSpaceStop", Settings.Basic.DiskSpaceStop);
	m_qSettings.setValue("DiskSpaceWarning", Settings.Basic.DiskSpaceWarning);
	m_qSettings.setValue("DownloadsTips", Settings.Basic.DownloadsTips);
	m_qSettings.setValue("FirstRun", Settings.Basic.FirstRun);
	m_qSettings.setValue("GUIMode", Settings.Basic.GUIMode);
	m_qSettings.setValue("LanguageFile", Settings.Basic.LanguageFile);
	m_qSettings.setValue("LibraryTips", Settings.Basic.LibraryTips);
	m_qSettings.setValue("MediaTips", Settings.Basic.MediaTips);
	m_qSettings.setValue("NeighboursTips", Settings.Basic.NeighboursTips);
	m_qSettings.setValue("SearchTips", Settings.Basic.SearchTips);
	m_qSettings.setValue("SkinFile", Settings.Basic.SkinFile);
	m_qSettings.setValue("StartWithSystem", Settings.Basic.StartWithSystem);
	m_qSettings.setValue("TipDelay", Settings.Basic.TipDelay);
	m_qSettings.setValue("TipLowResMode", Settings.Basic.TipLowResMode);
	m_qSettings.setValue("TipTransparency", Settings.Basic.TipTransparency);
	m_qSettings.setValue("TrayMinimise", Settings.Basic.TrayMinimise);
	m_qSettings.setValue("UploadsTips", Settings.Basic.UploadsTips);
	m_qSettings.setValue("UsingSkin", Settings.Basic.UsingSkin);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Parental");
	m_qSettings.setValue("AdultFilter", Settings.Parental.AdultFilter);
	m_qSettings.setValue("ChatAdultCensor", Settings.Parental.ChatAdultCensor);
	m_qSettings.setValue("FilterAdultSearchResults", Settings.Parental.FilterAdultSearchResults);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Logging");
	m_qSettings.setValue("DebugLog", Settings.Logging.DebugLog);
	m_qSettings.setValue("LogLevel", Settings.Logging.LogLevel);
	m_qSettings.setValue("LogShowTimestamp", Settings.Logging.LogShowTimestamp);
	m_qSettings.setValue("MaxDebugLogSize", Settings.Logging.MaxDebugLogSize);
	m_qSettings.setValue("SearchLog", Settings.Logging.SearchLog);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Library");
	m_qSettings.setValue("FilterURI", Settings.Library.FilterURI);
	m_qSettings.setValue("GhostFiles", Settings.Library.GhostFiles);
	m_qSettings.setValue("HashWindow", Settings.Library.HashWindow);
	m_qSettings.setValue("HighPriorityHashing", Settings.Library.HighPriorityHashing);
	m_qSettings.setValue("HighPriorityHashingSpeed", Settings.Library.HighPriorityHashingSpeed);
	m_qSettings.setValue("HistoryDays", Settings.Library.HistoryDays);
	m_qSettings.setValue("HistoryTotal", Settings.Library.HistoryTotal);
	m_qSettings.setValue("LowPriorityHashingSpeed", Settings.Library.LowPriorityHashingSpeed);
	m_qSettings.setValue("NeverShareTypes", Settings.Library.NeverShareTypes);
	m_qSettings.setValue("PartialMatch", Settings.Library.PartialMatch);
	m_qSettings.setValue("PreferAPETags", Settings.Library.PreferAPETags);
	m_qSettings.setValue("QueryRouteSize", Settings.Library.QueryRouteSize);
	m_qSettings.setValue("RememberViews", Settings.Library.RememberViews);
	m_qSettings.setValue("SafeExecuteTypes", Settings.Library.SafeExecuteTypes);
	m_qSettings.setValue("ScanAPE", Settings.Library.ScanAPE);
	m_qSettings.setValue("ScanASF", Settings.Library.ScanASF);
	m_qSettings.setValue("ScanAVI", Settings.Library.ScanAVI);
	m_qSettings.setValue("ScanCHM", Settings.Library.ScanCHM);
	m_qSettings.setValue("ScanEXE", Settings.Library.ScanEXE);
	m_qSettings.setValue("ScanImage", Settings.Library.ScanImage);
	m_qSettings.setValue("ScanMP3", Settings.Library.ScanMP3);
	m_qSettings.setValue("ScanMPC", Settings.Library.ScanMPC);
	m_qSettings.setValue("ScanMPEG", Settings.Library.ScanMPEG);
	m_qSettings.setValue("ScanMSI", Settings.Library.ScanMSI);
	m_qSettings.setValue("ScanOGG", Settings.Library.ScanOGG);
	m_qSettings.setValue("ScanPDF", Settings.Library.ScanPDF);
	m_qSettings.setValue("SchemaURI", Settings.Library.SchemaURI);
	m_qSettings.setValue("Shares", Settings.Library.Shares);
	m_qSettings.setValue("ShowCoverArt", Settings.Library.ShowCoverArt);
	m_qSettings.setValue("SmartSeriesDetection", Settings.Library.SmartSeriesDetection);
	m_qSettings.setValue("SourceExpire", Settings.Library.SourceExpire);
	m_qSettings.setValue("SourceMesh", Settings.Library.SourceMesh);
	m_qSettings.setValue("ThumbSize", Settings.Library.ThumbSize);
	m_qSettings.setValue("TigerHeight", Settings.Library.TigerHeight);
	m_qSettings.setValue("TreeSize", Settings.Library.TreeSize);
	m_qSettings.setValue("WatchFolders", Settings.Library.WatchFolders);
	m_qSettings.setValue("WatchFoldersTimeout", Settings.Library.WatchFoldersTimeout);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("MediaPlayer");
	m_qSettings.setValue("Aspect", Settings.MediaPlayer.Aspect);
	m_qSettings.setValue("AudioVisualPlugin", Settings.MediaPlayer.AudioVisualPlugin);
	m_qSettings.setValue("CustomPlayerPath", Settings.MediaPlayer.CustomPlayerPath);
	m_qSettings.setValue("MediaHandler", Settings.MediaPlayer.MediaHandler);
	m_qSettings.setValue("FileTypes", Settings.MediaPlayer.FileTypes);
	m_qSettings.setValue("ListVisible", Settings.MediaPlayer.ListVisible);
	m_qSettings.setValue("Mute", Settings.MediaPlayer.Mute);
	m_qSettings.setValue("Playlist", Settings.MediaPlayer.Playlist);
	m_qSettings.setValue("Random", Settings.MediaPlayer.Random);
	m_qSettings.setValue("Repeat", Settings.MediaPlayer.Repeat);
	m_qSettings.setValue("StatusVisible", Settings.MediaPlayer.StatusVisible);
	m_qSettings.setValue("Volume", Settings.MediaPlayer.Volume);
	m_qSettings.setValue("Zoom", Settings.MediaPlayer.Zoom);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Search");
	m_qSettings.setValue("BlankSchemaURI", Settings.Search.BlankSchemaURI);
	m_qSettings.setValue("BrowseTreeSize", Settings.Search.BrowseTreeSize);
	m_qSettings.setValue("ClearPrevious", Settings.Search.ClearPrevious);
	m_qSettings.setValue("ExpandSearchMatches", Settings.Search.ExpandSearchMatches);
	m_qSettings.setValue("FilterMask", Settings.Search.FilterMask);
	m_qSettings.setValue("HighlightNew", Settings.Search.HighlightNew);
	m_qSettings.setValue("LastSchemaURI", Settings.Search.LastSchemaURI);
	m_qSettings.setValue("MaxPreviewLength", Settings.Search.MaxPreviewLength);
	m_qSettings.setValue("MonitorFilter", Settings.Search.MonitorFilter);
	m_qSettings.setValue("MonitorQueue", Settings.Search.MonitorQueue);
	m_qSettings.setValue("MonitorSchemaURI", Settings.Search.MonitorSchemaURI);
	m_qSettings.setValue("SchemaTypes", Settings.Search.SchemaTypes);
	m_qSettings.setValue("SearchThrottle", Settings.Search.SearchThrottle);
	m_qSettings.setValue("ShareMonkeyBaseURL", Settings.Search.ShareMonkeyBaseURL);
	m_qSettings.setValue("ShowNames", Settings.Search.ShowNames);
	m_qSettings.setValue("SwitchOnDownload", Settings.Search.SwitchOnDownload);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Chat");
	m_qSettings.setValue("AwayMessageIdleTime", Settings.Chat.AwayMessageIdleTime);
	m_qSettings.setValue("Background", Settings.Chat.Background);
	m_qSettings.setValue("ConnectOnStartup", Settings.Chat.ConnectOnStartup);
	m_qSettings.setValue("ColorChatBackground", Settings.Chat.ColorChatBackground);
	m_qSettings.setValue("ColorNormalText", Settings.Chat.ColorNormalText);
	m_qSettings.setValue("ColorNoticesText", Settings.Chat.ColorNoticesText);
	m_qSettings.setValue("ColorRoomActionsText", Settings.Chat.ColorRoomActionsText);
	m_qSettings.setValue("ColorServerMessagesText", Settings.Chat.ColorServerMessagesText);
	m_qSettings.setValue("ColorTopicsText", Settings.Chat.ColorTopicsText);
	m_qSettings.setValue("EnableChatAllNetworks", Settings.Chat.EnableChatAllNetworks);
	m_qSettings.setValue("EnableFileTransfers", Settings.Chat.EnableFileTransfers);
	m_qSettings.setValue("GnutellaChatEnable", Settings.Chat.GnutellaChatEnable);
	m_qSettings.setValue("IrcServerName", Settings.Chat.IrcServerName);
	m_qSettings.setValue("IrcServerPort", Settings.Chat.IrcServerPort);
	m_qSettings.setValue("ScreenFont", Settings.Chat.ScreenFont);
	m_qSettings.setValue("ShowTimestamp", Settings.Chat.ShowTimestamp);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Connection");
	m_qSettings.setValue("CanAcceptIncoming", Settings.Connection.CanAcceptIncoming);
	m_qSettings.setValue("ConnectThrottle", Settings.Connection.ConnectThrottle);
	m_qSettings.setValue("DetectConnectionLoss", Settings.Connection.DetectConnectionLoss);
	m_qSettings.setValue("DetectConnectionReset", Settings.Connection.DetectConnectionReset);
	m_qSettings.setValue("FailureLimit", Settings.Connection.FailureLimit);
	m_qSettings.setValue("FailurePenalty", Settings.Connection.FailurePenalty);
	m_qSettings.setValue("InAddress", Settings.Connection.InAddress);
	m_qSettings.setValue("InBind", Settings.Connection.InBind);
	m_qSettings.setValue("InSpeed", Settings.Connection.InSpeed);
	m_qSettings.setValue("OutAddress", Settings.Connection.OutAddress);
	m_qSettings.setValue("OutSpeed", Settings.Connection.OutSpeed);
	m_qSettings.setValue("Port", Settings.Connection.Port);
	m_qSettings.setValue("RandomPort", Settings.Connection.RandomPort);
	m_qSettings.setValue("SendBuffer", Settings.Connection.SendBuffer);
	m_qSettings.setValue("TimeoutConnect", Settings.Connection.TimeoutConnect);
	m_qSettings.setValue("TimeoutHandshake", Settings.Connection.TimeoutHandshake);
	m_qSettings.setValue("TimeoutTraffic", Settings.Connection.TimeoutTraffic);
	m_qSettings.setValue("XPsp2PatchedConnect", Settings.Connection.XPsp2PatchedConnect);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Web");
	m_qSettings.setValue("Ares", Settings.Web.Ares);
	m_qSettings.setValue("BrowserIntegration", Settings.Web.BrowserIntegration);
	m_qSettings.setValue("ED2K", Settings.Web.ED2K);
	m_qSettings.setValue("Foxy", Settings.Web.Foxy);
	m_qSettings.setValue("Gnutella", Settings.Web.Gnutella);
	m_qSettings.setValue("Magnet", Settings.Web.Magnet);
	m_qSettings.setValue("ManageDownloadTypes", Settings.Web.ManageDownloadTypes);
	m_qSettings.setValue("Piolet", Settings.Web.Piolet);
	m_qSettings.setValue("Torrent", Settings.Web.Torrent);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("WebServices");
	m_qSettings.setValue("BitziAgent", Settings.WebServices.BitziAgent);
	m_qSettings.setValue("BitziOkay", Settings.WebServices.BitziOkay);
	m_qSettings.setValue("BitziWebSubmit", Settings.WebServices.BitziWebSubmit);
	m_qSettings.setValue("BitziWebView", Settings.WebServices.BitziWebView);
	m_qSettings.setValue("BitziXML", Settings.WebServices.BitziXML);
	m_qSettings.setValue("ShareMonkeyCid", Settings.WebServices.ShareMonkeyCid);
	m_qSettings.setValue("ShareMonkeyOkay", Settings.WebServices.ShareMonkeyOkay);
	m_qSettings.setValue("ShareMonkeySaveThumbnail", Settings.WebServices.ShareMonkeySaveThumbnail);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Transfers");
	m_qSettings.setValue("BandwidthDownloads", Settings.Transfers.BandwidthDownloads);
	m_qSettings.setValue("BandwidthHubIn", Settings.Transfers.BandwidthHubIn);
	m_qSettings.setValue("BandwidthHubOut", Settings.Transfers.BandwidthHubOut);
	m_qSettings.setValue("BandwidthHubUploads", Settings.Transfers.BandwidthHubUploads);
	m_qSettings.setValue("BandwidthLeafIn", Settings.Transfers.BandwidthLeafIn);
	m_qSettings.setValue("BandwidthLeafOut", Settings.Transfers.BandwidthLeafOut);
	m_qSettings.setValue("BandwidthPeerIn", Settings.Transfers.BandwidthPeerIn);
	m_qSettings.setValue("BandwidthPeerOut", Settings.Transfers.BandwidthPeerOut);
	m_qSettings.setValue("BandwidthRequest", Settings.Transfers.BandwidthRequest);
	m_qSettings.setValue("BandwidthUdpOut", Settings.Transfers.BandwidthUdpOut);
	m_qSettings.setValue("BandwidthUploads", Settings.Transfers.BandwidthUploads);
	m_qSettings.setValue("MinTransfersRest", Settings.Transfers.MinTransfersRest);
	m_qSettings.setValue("RatesUnit", Settings.Transfers.RatesUnit);
	m_qSettings.setValue("RequireConnectedNetwork", Settings.Transfers.RequireConnectedNetwork);
	m_qSettings.setValue("SimpleProgressBar", Settings.Transfers.SimpleProgressBar);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Downloads");
	m_qSettings.setValue("AllowBackwards", Settings.Downloads.AllowBackwards);
	m_qSettings.setValue("AutoClear", Settings.Downloads.AutoClear);
	m_qSettings.setValue("BufferSize", Settings.Downloads.BufferSize);
	m_qSettings.setValue("ChunkSize", Settings.Downloads.ChunkSize);
	m_qSettings.setValue("ChunkStrap", Settings.Downloads.ChunkStrap);
	m_qSettings.setValue("ClearDelay", Settings.Downloads.ClearDelay);
	m_qSettings.setValue("CompletePath", Settings.Downloads.CompletePath);
	m_qSettings.setValue("ConnectThrottle", Settings.Downloads.ConnectThrottle);
	m_qSettings.setValue("DropFailedSourcesThreshold", Settings.Downloads.DropFailedSourcesThreshold);
	m_qSettings.setValue("ExpandDownloads", Settings.Downloads.ExpandDownloads);
	m_qSettings.setValue("FlushSD", Settings.Downloads.FlushSD);
	m_qSettings.setValue("IncompletePath", Settings.Downloads.IncompletePath);
	m_qSettings.setValue("MaxAllowedFailures", Settings.Downloads.MaxAllowedFailures);
	m_qSettings.setValue("MaxConnectingSources", Settings.Downloads.MaxConnectingSources);
	m_qSettings.setValue("MaxFiles", Settings.Downloads.MaxFiles);
	m_qSettings.setValue("MaxFileSearches", Settings.Downloads.MaxFileSearches);
	m_qSettings.setValue("MaxReviews", Settings.Downloads.MaxReviews);
	m_qSettings.setValue("MaxTransfers", Settings.Downloads.MaxTransfers);
	m_qSettings.setValue("MaxTransfersPerFile", Settings.Downloads.MaxTransfersPerFile);
	m_qSettings.setValue("Metadata", Settings.Downloads.Metadata);
	m_qSettings.setValue("MinSources", Settings.Downloads.MinSources);
	m_qSettings.setValue("NeverDrop", Settings.Downloads.NeverDrop);
	m_qSettings.setValue("PushTimeout", Settings.Downloads.PushTimeout);
	m_qSettings.setValue("QueueLimit", Settings.Downloads.QueueLimit);
	m_qSettings.setValue("RequestHash", Settings.Downloads.RequestHash);
	m_qSettings.setValue("RequestHTTP11", Settings.Downloads.RequestHTTP11);
	m_qSettings.setValue("RequestURLENC", Settings.Downloads.RequestURLENC);
	m_qSettings.setValue("RetryDelay", Settings.Downloads.RetryDelay);
	m_qSettings.setValue("SaveInterval", Settings.Downloads.SaveInterval);
	m_qSettings.setValue("SearchPeriod", Settings.Downloads.SearchPeriod);
	m_qSettings.setValue("ShowGroups", Settings.Downloads.ShowGroups);
	m_qSettings.setValue("ShowMonitorURLs", Settings.Downloads.ShowMonitorURLs);
	m_qSettings.setValue("ShowPercent", Settings.Downloads.ShowPercent);
	m_qSettings.setValue("ShowSources", Settings.Downloads.ShowSources);
	m_qSettings.setValue("SortColumns", Settings.Downloads.SortColumns);
	m_qSettings.setValue("SortSources", Settings.Downloads.SortSources);
	m_qSettings.setValue("SourcesWanted", Settings.Downloads.SourcesWanted);
	m_qSettings.setValue("StaggardStart", Settings.Downloads.StaggardStart);
	m_qSettings.setValue("StarveGiveUp", Settings.Downloads.StarveGiveUp);
	m_qSettings.setValue("StarveTimeout", Settings.Downloads.StarveTimeout);
	m_qSettings.setValue("URIPrompt", Settings.Downloads.URIPrompt);
	m_qSettings.setValue("VerifyED2K", Settings.Downloads.VerifyED2K);
	m_qSettings.setValue("VerifyFiles", Settings.Downloads.VerifyFiles);
	m_qSettings.setValue("VerifyTiger", Settings.Downloads.VerifyTiger);
	m_qSettings.setValue("WebHookEnable", Settings.Downloads.WebHookEnable);
	m_qSettings.setValue("WebHookExtensions", Settings.Downloads.WebHookExtensions);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Uploads");
	m_qSettings.setValue("AllowBackwards", Settings.Uploads.AllowBackwards);
	m_qSettings.setValue("AutoClear", Settings.Uploads.AutoClear);
	m_qSettings.setValue("ClampdownFactor", Settings.Uploads.ClampdownFactor);
	m_qSettings.setValue("ClampdownFloor", Settings.Uploads.ClampdownFloor);
	m_qSettings.setValue("ClearDelay", Settings.Uploads.ClearDelay);
	m_qSettings.setValue("DynamicPreviews", Settings.Uploads.DynamicPreviews);
	m_qSettings.setValue("FreeBandwidthFactor", Settings.Uploads.FreeBandwidthFactor);
	m_qSettings.setValue("FreeBandwidthValue", Settings.Uploads.FreeBandwidthValue);
	m_qSettings.setValue("HubShareLimiting", Settings.Uploads.HubShareLimiting);
	m_qSettings.setValue("MaxPerHost", Settings.Uploads.MaxPerHost);
	m_qSettings.setValue("PreviewQuality", Settings.Uploads.PreviewQuality);
	m_qSettings.setValue("PreviewTransfers", Settings.Uploads.PreviewTransfers);
	m_qSettings.setValue("QueuePollMax", Settings.Uploads.QueuePollMax);
	m_qSettings.setValue("QueuePollMin", Settings.Uploads.QueuePollMin);
	m_qSettings.setValue("RewardQueuePercentage", Settings.Uploads.RewardQueuePercentage);
	m_qSettings.setValue("RotateChunkLimit", Settings.Uploads.RotateChunkLimit);
	m_qSettings.setValue("ShareHashset", Settings.Uploads.ShareHashset);
	m_qSettings.setValue("ShareMetadata", Settings.Uploads.ShareMetadata);
	m_qSettings.setValue("SharePartials", Settings.Uploads.SharePartials);
	m_qSettings.setValue("SharePreviews", Settings.Uploads.SharePreviews);
	m_qSettings.setValue("ShareTiger", Settings.Uploads.ShareTiger);
	m_qSettings.setValue("ThrottleMode", Settings.Uploads.ThrottleMode);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Security");
	m_qSettings.setValue("AllowProfileBrowse", Settings.Security.AllowProfileBrowse);
	m_qSettings.setValue("AllowSharesBrowse", Settings.Security.AllowSharesBrowse);
	m_qSettings.setValue("BlockedAgentUploadFilter", Settings.Security.BlockedAgentUploadFilter);
	m_qSettings.setValue("DeleteFirewallException", Settings.Security.DeleteFirewallException);
	m_qSettings.setValue("DeleteUPnPPorts", Settings.Security.DeleteUPnPPorts);
	m_qSettings.setValue("ED2kChatFilter", Settings.Security.ED2kChatFilter);
	m_qSettings.setValue("EnableFirewallException", Settings.Security.EnableFirewallException);
	m_qSettings.setValue("EnableUPnP", Settings.Security.EnableUPnP);
	m_qSettings.setValue("FirewallState", Settings.Security.FirewallState);
	m_qSettings.setValue("ChatFilter", Settings.Security.ChatFilter);
	m_qSettings.setValue("IrcFloodLimit", Settings.Security.IrcFloodLimit);
	m_qSettings.setValue("IrcFloodProtection", Settings.Security.IrcFloodProtection);
	m_qSettings.setValue("MaxMaliciousFileSize", Settings.Security.MaxMaliciousFileSize);
	m_qSettings.setValue("RemoteEnable", Settings.Security.RemoteEnable);
	m_qSettings.setValue("RemotePassword", Settings.Security.RemotePassword);
	m_qSettings.setValue("RemoteUsername", Settings.Security.RemoteUsername);
	m_qSettings.setValue("SearchIgnoreLocalIP", Settings.Security.SearchIgnoreLocalIP);
	m_qSettings.setValue("SearchIgnoreOwnIP", Settings.Security.SearchIgnoreOwnIP);
	m_qSettings.setValue("SearchSpamFilterThreshold", Settings.Security.SearchSpamFilterThreshold);
	m_qSettings.setValue("UPnPSkipWANIPSetup", Settings.Security.UPnPSkipWANIPSetup);
	m_qSettings.setValue("UPnPSkipWANPPPSetup", Settings.Security.UPnPSkipWANPPPSetup);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella");
	m_qSettings.setValue("CompressHub2Hub", Settings.Gnutella.CompressHub2Hub);
	m_qSettings.setValue("CompressHub2Leaf", Settings.Gnutella.CompressHub2Leaf);
	m_qSettings.setValue("CompressLeaf2Hub", Settings.Gnutella.CompressLeaf2Hub);
	m_qSettings.setValue("ConnectFactor", Settings.Gnutella.ConnectFactor);
	m_qSettings.setValue("ConnectThrottle", Settings.Gnutella.ConnectThrottle);
	m_qSettings.setValue("HitsPerPacket", Settings.Gnutella.HitsPerPacket);
	m_qSettings.setValue("HostCacheSize", Settings.Gnutella.HostCacheSize);
	m_qSettings.setValue("HostCacheView", Settings.Gnutella.HostCacheView);
	m_qSettings.setValue("MaxHits", Settings.Gnutella.MaxHits);
	m_qSettings.setValue("MaxResults", Settings.Gnutella.MaxResults);
	m_qSettings.setValue("RouteCache", Settings.Gnutella.RouteCache);
	m_qSettings.setValue("SpecifyProtocol", Settings.Gnutella.SpecifyProtocol);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella2");
	m_qSettings.setValue("ClientMode", Settings.Gnutella2.ClientMode);
	m_qSettings.setValue("Enable", Settings.Gnutella2.Enable);
	m_qSettings.setValue("HAWPeriod", Settings.Gnutella2.HAWPeriod);
	m_qSettings.setValue("HostCount", Settings.Gnutella2.HostCount);
	m_qSettings.setValue("HostCurrent", Settings.Gnutella2.HostCurrent);
	m_qSettings.setValue("HostExpire", Settings.Gnutella2.HostExpire);
	m_qSettings.setValue("HubHorizonSize", Settings.Gnutella2.HubHorizonSize);
	m_qSettings.setValue("HubVerified", Settings.Gnutella2.HubVerified);
	m_qSettings.setValue("KHLHubCount", Settings.Gnutella2.KHLHubCount);
	m_qSettings.setValue("KHLPeriod", Settings.Gnutella2.KHLPeriod);
	m_qSettings.setValue("LNIPeriod", Settings.Gnutella2.LNIPeriod);
	m_qSettings.setValue("NumHubs", Settings.Gnutella2.NumHubs);
	m_qSettings.setValue("NumLeafs", Settings.Gnutella2.NumLeafs);
	m_qSettings.setValue("NumPeers", Settings.Gnutella2.NumPeers);
	m_qSettings.setValue("PingRate", Settings.Gnutella2.PingRate);
	m_qSettings.setValue("PingRelayLimit", Settings.Gnutella2.PingRelayLimit);
	m_qSettings.setValue("QueryGlobalThrottle", Settings.Gnutella2.QueryGlobalThrottle);
	m_qSettings.setValue("QueryHostDeadline", Settings.Gnutella2.QueryHostDeadline);
	m_qSettings.setValue("QueryHostThrottle", Settings.Gnutella2.QueryHostThrottle);
	m_qSettings.setValue("QueryLimit", Settings.Gnutella2.QueryLimit);
	m_qSettings.setValue("RequeryDelay", Settings.Gnutella2.RequeryDelay);
	m_qSettings.setValue("UdpBuffers", Settings.Gnutella2.UdpBuffers);
	m_qSettings.setValue("UdpGlobalThrottle", Settings.Gnutella2.UdpGlobalThrottle);
	m_qSettings.setValue("UdpInExpire", Settings.Gnutella2.UdpInExpire);
	m_qSettings.setValue("UdpInFrames", Settings.Gnutella2.UdpInFrames);
	m_qSettings.setValue("UdpMTU", Settings.Gnutella2.UdpMTU);
	m_qSettings.setValue("UdpOutExpire", Settings.Gnutella2.UdpOutExpire);
	m_qSettings.setValue("UdpOutFrames", Settings.Gnutella2.UdpOutFrames);
	m_qSettings.setValue("UdpOutResend", Settings.Gnutella2.UdpOutResend);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Gnutella1");
	m_qSettings.setValue("ClientMode", Settings.Gnutella1.ClientMode);
	m_qSettings.setValue("DefaultTTL", Settings.Gnutella1.DefaultTTL);
	m_qSettings.setValue("Enable", Settings.Gnutella1.Enable);
	m_qSettings.setValue("EnableDIPPSupport", Settings.Gnutella1.EnableDIPPSupport);
	m_qSettings.setValue("EnableGGEP", Settings.Gnutella1.EnableGGEP);
	m_qSettings.setValue("HitQueueLimit", Settings.Gnutella1.HitQueueLimit);
	m_qSettings.setValue("HostCount", Settings.Gnutella1.HostCount);
	m_qSettings.setValue("HostExpire", Settings.Gnutella1.HostExpire);
	m_qSettings.setValue("MaxHostsInPongs", Settings.Gnutella1.MaxHostsInPongs);
	m_qSettings.setValue("MaximumPacket", Settings.Gnutella1.MaximumPacket);
	m_qSettings.setValue("MaximumQuery", Settings.Gnutella1.MaximumQuery);
	m_qSettings.setValue("MaximumTTL", Settings.Gnutella1.MaximumTTL);
	m_qSettings.setValue("NumHubs", Settings.Gnutella1.NumHubs);
	m_qSettings.setValue("NumLeafs", Settings.Gnutella1.NumLeafs);
	m_qSettings.setValue("NumPeers", Settings.Gnutella1.NumPeers);
	m_qSettings.setValue("PacketBufferSize", Settings.Gnutella1.PacketBufferSize);
	m_qSettings.setValue("PacketBufferTime", Settings.Gnutella1.PacketBufferTime);
	m_qSettings.setValue("PingFlood", Settings.Gnutella1.PingFlood);
	m_qSettings.setValue("PingRate", Settings.Gnutella1.PingRate);
	m_qSettings.setValue("PongCache", Settings.Gnutella1.PongCache);
	m_qSettings.setValue("PongCount", Settings.Gnutella1.PongCount);
	m_qSettings.setValue("QueryHitUTF8", Settings.Gnutella1.QueryHitUTF8);
	m_qSettings.setValue("QuerySearchUTF8", Settings.Gnutella1.QuerySearchUTF8);
	m_qSettings.setValue("QueryThrottle", Settings.Gnutella1.QueryThrottle);
	m_qSettings.setValue("RequeryDelay", Settings.Gnutella1.RequeryDelay);
	m_qSettings.setValue("SearchTTL", Settings.Gnutella1.SearchTTL);
	m_qSettings.setValue("StrictPackets", Settings.Gnutella1.StrictPackets);
	m_qSettings.setValue("TranslateTTL", Settings.Gnutella1.TranslateTTL);
	m_qSettings.setValue("VendorMsg", Settings.Gnutella1.VendorMsg);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Ares");
	m_qSettings.setValue("Enable", Settings.Ares.Enable);
	m_qSettings.setValue("MaximumDownloads", Settings.Ares.MaximumDownloads);
	m_qSettings.setValue("MaximumUploads", Settings.Ares.MaximumUploads);
	m_qSettings.setValue("MaximumUploadsPerUser", Settings.Ares.MaximumUploadsPerUser);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("eDonkey2k");
	m_qSettings.setValue("DefaultServerFlags", Settings.eDonkey2k.DefaultServerFlags);
	m_qSettings.setValue("DequeueTime", Settings.eDonkey2k.DequeueTime);
	m_qSettings.setValue("Enable", Settings.eDonkey2k.Enable);
	m_qSettings.setValue("EnableKad", Settings.eDonkey2k.EnableKad);
	m_qSettings.setValue("Endgame", Settings.eDonkey2k.Endgame);
	m_qSettings.setValue("ExtendedRequest", Settings.eDonkey2k.ExtendedRequest);
	m_qSettings.setValue("FastConnect", Settings.eDonkey2k.FastConnect);
	m_qSettings.setValue("ForceHighID", Settings.eDonkey2k.ForceHighID);
	m_qSettings.setValue("FrameSize", Settings.eDonkey2k.FrameSize);
	m_qSettings.setValue("GetSourcesThrottle", Settings.eDonkey2k.GetSourcesThrottle);
	m_qSettings.setValue("LargeFileSupport", Settings.eDonkey2k.LargeFileSupport);
	m_qSettings.setValue("LearnNewServers", Settings.eDonkey2k.LearnNewServers);
	m_qSettings.setValue("LearnNewServersClient", Settings.eDonkey2k.LearnNewServersClient);
	m_qSettings.setValue("MagnetSearch", Settings.eDonkey2k.MagnetSearch);
	m_qSettings.setValue("MaxClients", Settings.eDonkey2k.MaxClients);
	m_qSettings.setValue("MaxResults", Settings.eDonkey2k.MaxResults);
	m_qSettings.setValue("MaxShareCount", Settings.eDonkey2k.MaxShareCount);
	m_qSettings.setValue("MetAutoQuery", Settings.eDonkey2k.MetAutoQuery);
	m_qSettings.setValue("MinServerFileSize", Settings.eDonkey2k.MinServerFileSize);
	m_qSettings.setValue("NumServers", Settings.eDonkey2k.NumServers);
	m_qSettings.setValue("PacketThrottle", Settings.eDonkey2k.PacketThrottle);
	m_qSettings.setValue("QueryFileThrottle", Settings.eDonkey2k.QueryFileThrottle);
	m_qSettings.setValue("QueryGlobalThrottle", Settings.eDonkey2k.QueryGlobalThrottle);
	m_qSettings.setValue("QueueRankThrottle", Settings.eDonkey2k.QueueRankThrottle);
	m_qSettings.setValue("QueryServerThrottle", Settings.eDonkey2k.QueryServerThrottle);
	m_qSettings.setValue("ReAskTime", Settings.eDonkey2k.ReAskTime);
	m_qSettings.setValue("RequestPipe", Settings.eDonkey2k.RequestPipe);
	m_qSettings.setValue("RequestSize", Settings.eDonkey2k.RequestSize);
	m_qSettings.setValue("SearchCachedServers", Settings.eDonkey2k.SearchCachedServers);
	m_qSettings.setValue("SendPortServer", Settings.eDonkey2k.SendPortServer);
	m_qSettings.setValue("ServerListURL", Settings.eDonkey2k.ServerListURL);
	m_qSettings.setValue("ServerWalk", Settings.eDonkey2k.ServerWalk);
	m_qSettings.setValue("SourceThrottle", Settings.eDonkey2k.SourceThrottle);
	m_qSettings.setValue("StatsGlobalThrottle", Settings.eDonkey2k.StatsGlobalThrottle);
	m_qSettings.setValue("StatsServerThrottle", Settings.eDonkey2k.StatsServerThrottle);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Bittorrent");
	m_qSettings.setValue("AutoClear", Settings.Bittorrent.AutoClear);
	m_qSettings.setValue("AutoSeed", Settings.Bittorrent.AutoSeed);
	m_qSettings.setValue("BandwidthPercentage", Settings.Bittorrent.BandwidthPercentage);
	m_qSettings.setValue("ClearRatio", Settings.Bittorrent.ClearRatio);
	m_qSettings.setValue("CodePage", Settings.Bittorrent.CodePage);
	m_qSettings.setValue("DefaultTracker", Settings.Bittorrent.DefaultTracker);
	m_qSettings.setValue("DefaultTrackerPeriod", Settings.Bittorrent.DefaultTrackerPeriod);
	m_qSettings.setValue("DhtPruneTime", Settings.Bittorrent.DhtPruneTime);
	m_qSettings.setValue("DownloadConnections", Settings.Bittorrent.DownloadConnections);
	m_qSettings.setValue("DownloadTorrents", Settings.Bittorrent.DownloadTorrents);
	m_qSettings.setValue("Endgame", Settings.Bittorrent.Endgame);
	m_qSettings.setValue("ExtraKeys", Settings.Bittorrent.ExtraKeys);
	m_qSettings.setValue("LinkPing", Settings.Bittorrent.LinkPing);
	m_qSettings.setValue("LinkTimeout", Settings.Bittorrent.LinkTimeout);
	m_qSettings.setValue("Managed", Settings.Bittorrent.Managed);
	m_qSettings.setValue("PreferBTSources", Settings.Bittorrent.PreferBTSources);
	m_qSettings.setValue("RandomPeriod", Settings.Bittorrent.RandomPeriod);
	m_qSettings.setValue("RequestLimit", Settings.Bittorrent.RequestLimit);
	m_qSettings.setValue("RequestPipe", Settings.Bittorrent.RequestPipe);
	m_qSettings.setValue("RequestSize", Settings.Bittorrent.RequestSize);
	m_qSettings.setValue("ShowFilesInDownload", Settings.Bittorrent.ShowFilesInDownload);
	m_qSettings.setValue("SourceExchangePeriod", Settings.Bittorrent.SourceExchangePeriod);
	m_qSettings.setValue("StartPaused", Settings.Bittorrent.StartPaused);
	m_qSettings.setValue("TestPartials", Settings.Bittorrent.TestPartials);
	m_qSettings.setValue("TorrentPath", Settings.Bittorrent.TorrentPath);
	m_qSettings.setValue("TrackerKey", Settings.Bittorrent.TrackerKey);
	m_qSettings.setValue("UploadCount", Settings.Bittorrent.UploadCount);
	m_qSettings.setValue("UseKademlia", Settings.Bittorrent.UseKademlia);
	m_qSettings.setValue("UseSaveDialog", Settings.Bittorrent.UseSaveDialog);
	m_qSettings.setValue("UseTemp", Settings.Bittorrent.UseTemp);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Discovery");
	m_qSettings.setValue("AccessThrottle", Settings.Discovery.AccessThrottle);
	m_qSettings.setValue("BootstrapCount", Settings.Discovery.BootstrapCount);
	m_qSettings.setValue("CacheCount", Settings.Discovery.CacheCount);
	m_qSettings.setValue("DefaultUpdate", Settings.Discovery.DefaultUpdate);
	m_qSettings.setValue("EnableG1GWC", Settings.Discovery.EnableG1GWC);
	m_qSettings.setValue("FailureLimit", Settings.Discovery.FailureLimit);
	m_qSettings.setValue("Lowpoint", Settings.Discovery.Lowpoint);
	m_qSettings.setValue("UpdatePeriod", Settings.Discovery.UpdatePeriod);
	m_qSettings.endGroup();

	m_qSettings.beginGroup("Scheduler");

	m_qSettings.endGroup();
	return;
}

void PersistentSettings::saveProfile()
{
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);

	m_qSettings.beginGroup("Profile");
	m_qSettings.setValue("Age", Settings.Profile.Age);
	m_qSettings.setValue("AolScreenName", Settings.Profile.AolScreenName);
	m_qSettings.setValue("AvatarPath", Settings.Profile.AvatarPath);
	m_qSettings.setValue("Biography", Settings.Profile.Biography);
	m_qSettings.setValue("City", Settings.Profile.City);
	m_qSettings.setValue("Country", Settings.Profile.Country);
	m_qSettings.setValue("Email", Settings.Profile.Email);
	m_qSettings.setValue("Favorites", Settings.Profile.Favorites);
	m_qSettings.setValue("FavoritesURL", Settings.Profile.FavoritesURL);
	m_qSettings.setValue("Gender", Settings.Profile.Gender);
	m_qSettings.setValue("GnutellaScreenName", Settings.Profile.GnutellaScreenName);
	m_qSettings.setValue("GUID", Settings.Profile.GUID);
	m_qSettings.setValue("ICQuin", Settings.Profile.ICQuin);
	m_qSettings.setValue("Interests", Settings.Profile.Interests);
	m_qSettings.setValue("IrcAlternateNickname", Settings.Profile.IrcAlternateNickname);
	m_qSettings.setValue("IrcNickname", Settings.Profile.IrcNickname);
	m_qSettings.setValue("IrcUserName", Settings.Profile.IrcUserName);
	m_qSettings.setValue("JabberID", Settings.Profile.JabberID);
	m_qSettings.setValue("Latitude", Settings.Profile.Latitude);
	m_qSettings.setValue("Longitude", Settings.Profile.Longitude);
	m_qSettings.setValue("MSNPassport", Settings.Profile.MSNPassport);
	m_qSettings.setValue("MyspaceProfile", Settings.Profile.MyspaceProfile);
	m_qSettings.setValue("RealName", Settings.Profile.RealName);
	m_qSettings.setValue("StateProvince", Settings.Profile.StateProvince);
	m_qSettings.setValue("YahooID", Settings.Profile.YahooID);
	m_qSettings.endGroup();
	return;
}

void PersistentSettings::saveWizard()
{
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);

	m_qSettings.beginGroup("Transfers");
	m_qSettings.setValue("BandwidthDownloads", Settings.Transfers.BandwidthDownloads);
	m_qSettings.setValue("BandwidthUploads", Settings.Transfers.BandwidthUploads);
	m_qSettings.setValue("SimpleProgressBar", Settings.Transfers.SimpleProgressBar);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Security");
	m_qSettings.setValue("EnableUPnP", Settings.Security.EnableUPnP);
	m_qSettings.setValue("AllowSharesBrowse", Settings.Security.AllowSharesBrowse);
	m_qSettings.setValue("AllowProfileBrowse", Settings.Security.AllowProfileBrowse);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Profile");
	m_qSettings.setValue("IrcNickname", Settings.Profile.IrcNickname);
	m_qSettings.setValue("IrcAlternateNickname", Settings.Profile.IrcAlternateNickname);
	m_qSettings.setValue("GnutellaScreenName", Settings.Profile.GnutellaScreenName);
	m_qSettings.setValue("Gender", Settings.Profile.Gender);
	m_qSettings.setValue("Age", Settings.Profile.Age);
	m_qSettings.setValue("Country", Settings.Profile.Country);
	m_qSettings.setValue("StateProvince", Settings.Profile.StateProvince);
	m_qSettings.setValue("City", Settings.Profile.City);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Gnutella2");
	m_qSettings.setValue("Enable", Settings.Gnutella2.Enable);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Gnutella1");
	m_qSettings.setValue("Enable", Settings.Gnutella1.Enable);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Ares");
	m_qSettings.setValue("Enable", Settings.Ares.Enable);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("eDonkey2k");
	m_qSettings.setValue("Enable", Settings.eDonkey2k.Enable);
	m_qSettings.setValue("EnableKad", Settings.eDonkey2k.EnableKad);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Web");
	m_qSettings.setValue("Torrent", Settings.Web.Torrent);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Bittorrent");
	m_qSettings.setValue("UseKademlia", Settings.Bittorrent.UseKademlia);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Basic");
	m_qSettings.setValue("ConnectOnStartup", Settings.Basic.ConnectOnStartup);
	m_qSettings.setValue("StartWithSystem", Settings.Basic.StartWithSystem);
	m_qSettings.setValue("FirstRun", Settings.Basic.FirstRun);
	m_qSettings.endGroup();
	m_qSettings.beginGroup("Library");
	m_qSettings.setValue("HighPriorityHashing", Settings.Library.HighPriorityHashing);
	m_qSettings.endGroup();
}

void PersistentSettings::saveLanguage()
{
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);

	m_qSettings.beginGroup("Basic");
	m_qSettings.setValue("DefaultLanguage", Settings.Basic.DefaultLanguage);
	m_qSettings.setValue("LanguageFile", Settings.Basic.LanguageFile);
	m_qSettings.endGroup();
}

void PersistentSettings::loadLanguage()
{
	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);

	m_qSettings.beginGroup("Basic");
	Settings.Basic.DefaultLanguage = m_qSettings.value("DefaultLanguage", true).toBool();
	Settings.Basic.LanguageFile = m_qSettings.value("LanguageFile", "").toString();
	m_qSettings.endGroup();
}
