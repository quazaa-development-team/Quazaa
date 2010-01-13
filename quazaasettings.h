//
// quazaasettings.h
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

#ifndef QUAZAASETTINGS_H
#define QUAZAASETTINGS_H

#include <QObject>
#include <QMainWindow>
#include "QSkinDialog/qskindialog.h"

class QuazaaSettings : public QObject
{
public:
	QuazaaSettings();
	void saveSettings();
	void loadSettings();
	void saveProfile();
	void loadProfile();
	void saveSkinWindowSettings(QSkinDialog *skinDialog);
	void loadSkinWindowSettings(QSkinDialog *skinDialog);
	void saveSkinSettings();
	void loadSkinSettings();
	void saveWindowSettings(QMainWindow *window);
	void loadWindowSettings(QMainWindow *window);
	void saveLanguageSettings();
	void loadLanguageSettings();
	void saveFirstRun(bool firstRun);
	bool FirstRun();

public:

	//Main Window settings including splitters, the active tab, sidebar task visible states, etc.
	int			MainWindowActiveTab;					// The active tab (Home, Library, Media, etc.)
	bool		MainWindowChatRoomsTaskVisible;			// The Rooms task in the Chat sidebar
	bool		MainWindowChatFriendsTaskVisible;		// The Friends task in the Chat sidebar
	QByteArray	MainWindowGraphSplitter;				// Graph splitter position
	bool		MainWindowHomeConnectionTaskVisible;	// The Connection task in the Home sidebar
	bool		MainWindowHomeDownloadsTaskVisible;		// The Downloads task in the Home sidebar
	bool		MainWindowHomeLibraryTaskVisible;		// The Library task in the Home sidebar
	QString		MainWindowHomeSearchString;				// The text typed into the search text box on the home tab
	bool		MainWindowHomeTorrentsTaskVisible;		// The BitTorrent task in the Home sidebar
	bool		MainWindowHomeUploadsTaskVisible;		// The Uploads task in the Home sidebar
	QByteArray	MainWindowHostCacheSplitter;			// Host Cache splitter position
	QByteArray	MainWindowLibraryDetailsSplitter;		// Library details splitter position
	bool		MainWindowLibraryDetailsVisible;		// The details for the selected Library file
	int			MainWindowLibraryNavigatorTab;			// Shared Folders or Organiser
	QByteArray	MainWindowLibrarySplitter;				// Library splitter position
	bool		MainWindowMediaPlaylistVisible;			// Media tab's playlist
	QByteArray	MainWindowMediaSplitter;				// Media splitter position
	QByteArray	MainWindowNetworkSplitter;				// Network splitter position
	QByteArray	MainWindowSearchDetailsSplitter;		// Search details splitter position
	bool		MainWindowSearchDetailsVisible;			// The details for the selected Search item
	bool		MainWindowSearchFileTypeTaskVisible;	// The File Type task in the Search sidebar
	bool		MainWindowSearchNetworksTaskVisible;	// The Networks task in the Search sidebar
	bool		MainWindowSearchResultsTaskVisible;		// The Results task in the Search sidebar
	bool		MainWindowSearchSidebarVisible;			// Search sidebar
	bool		MainWindowSearchTaskVisible;			// The Search task in the Search sidebar
	QByteArray	MainWindowTransfersSplitter;			// Transfers splitter position
	bool		MainWindowVisible;						// Is the main window visible

	// Live settings
	bool		LiveAdultWarning;							// Has the user been warned about the adult filter?
	bool		LiveAutoClose;
	int			LiveBandwidthScale;							// Monitor slider settings
	bool		LiveDefaultED2KServersLoaded;				// Has Quazaa already loaded default ED2K servers?
	bool		LiveDiskSpaceStop;							// Has Quazaa paused all downloads due to critical disk space?
	bool		LiveDiskSpaceWarning;						// Has the user been warned of low disk space?
	bool		LiveDiskWriteWarning;						// Has the user been warned of write problems?
	bool		LiveDonkeyServerWarning;					// Has the user been warned about having an empty server list?
	QString		LiveLastDuplicateHash;						// Stores the hash of the file about which the warning was shown
	bool		LiveMaliciousWarning;						// Is the warning dialog opened?
	bool		LiveQueueLimitWarning;						// Has the user been warned about limiting the max Q position accepted?
	bool		LiveUploadLimitWarning;						// Has the user been warned about the ed2k/BT ratio?

	//Basic settings
	int			BasicCloseMode;							// How does application react the close button it is pressed
	bool		BasicMinimizeToTray;					// How does application react the minimize button it is pressed
	bool		BasicChatTips;							// Show tooltips on chat view?
	bool		BasicConnectOnStartup;					// Connect to networks when starting Quazaa
	int			BasicDiskSpaceStop;						// Value at which to pause all downloads due to low disk space
	int			BasicDiskSpaceWarning;					// Value at which to warn the user about low disk space
	bool		BasicDownloadsTips;						// Show tooltips on downloads view?
	bool		BasicLibraryTips;						// Show tooltips on library view?
	bool		BasicMediaTips;							// Show tooltips on media view?
	bool		BasicNeighboursTips;					// Show tooltips on neigbours view?
	bool		BasicSearchTips;						// Show tooltips on searches view?
	bool		BasicStartWithSystem;					// Start with operating system
	int			BasicTipDelay;							// Amount of time the mouse is hovered before showing a tooltip
	bool		BasicTipLowResMode;						// Low resolution tooltips for performance issues.
	int			BasicTipTransparency;					// Alpha transarency level for tooltips
	bool		BasicUploadsTips;						// Show tooltips on uploads view?

	//Skin settings
	QString		SkinFile;								// Full path to the skin file

	//Language settings
	QString		LanguageFile;							// What language does the user prefer
	bool		LanguageDefault;						// Is the default language being used

	// Parental settings
	QStringList	ParentalAdultFilter;							// List of words to censor
	bool		ParentalChatAdultCensor;						// Censor adlt language from chat
	bool		ParentalFilterAdultSearchResults;				// Filter adult content from searches

	// Logging settings
	bool		LoggingDebugLog;								// Create a log file
	int			LoggingLogLevel;								// Log severity (0 - MSG_ERROR .. 4 - MSG_DEBUG)
	bool		LoggingLogShowTimestamp;						// Show timestamps in the system log?
	int			LoggingMaxDebugLogSize;						// Max size of the log file
	bool		LoggingSearchLog;								// Display search facility log information

	// Library settings
	QString		LibraryFilterURI;
	bool		LibraryGhostFiles;								// Create ghost files specifying why files were deleted on deletion
	bool		LibraryHashWindow;								// Display hashing progress window
	bool		LibraryHighPriorityHashing;					// Use high priority hashing
	int			LibraryHighPriorityHashingSpeed;				// desired speed in MB/s when hashing with hi priority
	int			LibraryHistoryDays;							// Remember files for this many days
	int			LibraryHistoryTotal;							// Remember this many total files in Library history
	int			LibraryLowPriorityHashingSpeed;				// desired speed in MB/s when hashing with low priority
	QStringList	LibraryNeverShareTypes;						// Never share these file types
	bool		LibraryPartialMatch;
	bool		LibraryPreferAPETags;							// Read APE tags first and only then ID3 tags from audio files
	int			LibraryQueryRouteSize;
	bool		LibraryRememberViews;							// Remember custom folder views
	QStringList	LibrarySafeExecuteTypes;						// These file types are safe to open
	bool		LibraryScanAPE;								// Enable .ape,.mac,.apl metadata extraction by internals
	bool		LibraryScanASF;								// Enable .asf,.wma,.wmv metadata extraction by internals
	bool		LibraryScanAVI;								// Enable .avi metadata extraction by internals
	bool		LibraryScanCHM;								// Enable .chm metadata extraction by internals
	bool		LibraryScanEXE;								// Enable .exe,.dll metadata extraction by internals
	bool		LibraryScanImage;								// Enable .jpg,.jpeg,.gif,.png,.bmp metadata extraction by internals
	bool		LibraryScanMP3;								// Enable .mp3 metadata extraction by internals
	bool		LibraryScanMPC;								// Enable .mpc,.mpp,.mp+ metadata extraction by internals
	bool		LibraryScanMPEG;								// Enable .mpeg,.mpg metadata extraction by internals
	bool		LibraryScanMSI;								// Enable .msi metadata extraction by internals
	bool		LibraryScanOGG;								// Enable .ogg metadata extraction by internals
	bool		LibraryScanPDF;								// Enable .pdf metadata extraction by internals
	QString		LibrarySchemaURI;
	QStringList	LibraryShares;									// A list of your shared folders
	bool		LibraryShowCoverArt;
	bool		LibrarySmartSeriesDetection;					// Organize video files in Library by using predefined patterns
	int			LibrarySourceExpire;
	bool		LibrarySourceMesh;
	int			LibraryThumbSize;
	int			LibraryTigerHeight;
	int			LibraryTreeSize;
	bool		LibraryWatchFolders;							// Watch shared folders and update hashes when changed
	int			LibraryWatchFoldersTimeout;					// Quit watching folder after time

	/*
	bool		ShowVirtual;
	int			PanelSize;
	bool		ShowPanel;
	bool		UseFolderGUID;							// Save/Load folder GUID using NTFS stream
	bool		MarkFileAsDownload;						// Mark downloaded file using NTFS stream as Internet Explorer
	bool		UseCustomFolders;						// Use desktop.ini
	*/

	// Media settings
	int			MediaAspect;									// Size video is displayed in
	QString		MediaAudioVisualPlugin;						// Path to visual plugin for audio. Null if none is used.
	QString		MediaCustomPlayerPath;						// Path to the custom player executable
	int			MediaHandler;							// Media Player (0 Built In, 1 Shell Open, 2 Custom)
	QStringList	MediaFileTypes;								// Try to open these file types as media files
	bool		MediaListVisible;							// Is playlist visible
	bool		MediaMute;									// Is sound muted
	QString		MediaOpenPath;								// The last folder where a media file was opened
	QStringList	MediaPlaylist;								// Save playlist so it can be reopened when closing/opening Quazaa
	bool		MediaShuffle;									// Is random turned on for media files?
	bool		MediaRepeat;									// Is repeat turned on for media files?
	bool		MediaStatusVisible;							// Display time remaining, etc
	qreal		MediaVolume;									// Sound volume
	int			MediaZoom;									// The zoom level for video
	/*
	QString		MediaServicesCLSID;
	QString		Mpeg1PreviewCLSID;
	QString		Mp3PreviewCLSID;
	QString		AviPreviewCLSID;
	QString		VisWrapperCLSID;
	QString		VisSoniqueCLSID;
	QString		VisCLSID;
	QString		VisPath;
	int			VisSize;
	QString		ServicePath;
	bool		ShortPaths;								// Some players differently handle unicode paths but they can launch files using 8.3 paths
	*/

	// Search settings
	QString		SearchBlankSchemaURI;
	int			SearchBrowseTreeSize;
	int			SearchClearPrevious;							// Clear previous search results? 0 - ask user; 1 - no; 2 - yes.
	bool		SearchExpandSearchMatches;					// Expand multi-source search matches
	int			SearchFilterMask;
	bool		SearchHighlightNew;
	QString		SearchLastSchemaURI;
	int			SearchMaxPreviewLength;						// Max size of search previews
	QString		SearchMonitorFilter;
	int			SearchMonitorQueue;
	QString		SearchMonitorSchemaURI;
	bool		SearchSchemaTypes;
	int			SearchThrottle;							// How often each individual search may run. Low values may cause source finding to get overlooked.
	QString		SearchShareMonkeyBaseURL;
	bool		SearchShowNames;
	bool		SearchSwitchOnDownload;						// Switch to transfers view when starting downloads
	//bool		SanityCheck;							// Drop hits of banned hosts

	// Chat settings
	int			ChatAwayMessageIdleTime;					// Time in secs of idle system time before showing away message
	QString		ChatBackground;								// Path to an image for the background of the chat window
	bool		ChatConnectOnStartup;						// Connect to the chat server and enter rooms on startup
	QColor		ChatColorChatBackground;					// Color for the background of the chat window
	QColor		ChatColorNormalText;						// Color for normal text in the chat window
	QColor		ChatColorNoticesText;						// Color for notices in the chat window
	QColor		ChatColorRoomActionsText;					// Color for room actions in the chat window
	QColor		ChatColorServerMessagesText;				// Color for server messages in the chat window
	QColor		ChatColorTopicsText;						// Color for topics in the chat window
	bool		ChatEnableChatAllNetworks;					// Is chat allowed over other protocols? (ed2k, etc)
	bool		ChatEnableFileTransfers;					// Enable IRC File Transfers
	bool		ChatGnutellaChatEnable;						// Is Gnutella chat enabled with compatible clients?
	QString		ChatIrcServerName;							// Web address of the Irc chat server
	int			ChatIrcServerPort;							// Port to connect to the chat server on
	QFont		ChatScreenFont;								// Font for the chat screen
	bool		ChatShowTimestamp;							// Show timestamps at the beginning of messages

	// Profile settings
	int			ProfileAge;									// Age..
	QString		ProfileAolScreenName;							// Aol screen name
	QString		ProfileAvatarPath;								// Path to an avatar image file. Quazaa logo used if blank
	QString		ProfileBiography;								// Your life story..
	QString		ProfileCity;									// Your city
	QString		ProfileCountry;								// Your country..
	QString		ProfileEmail;									// EMail address
	QStringList	ProfileFavorites;								// Favorite websites
	QStringList	ProfileFavoritesURL;							// Favorite Websites URLs
	int			ProfileGender;									// Gender..
	QString		ProfileGnutellaScreenName;						// The name displayed in searches, uploads and downloads and when users browse your system
	QString		ProfileGUID;									// Unique ID for each client. Can be regenerated
	QString		ProfileICQuin;									// Identification number in ICQ
	QStringList	ProfileInterests;								// What do you like to do other than use Quazaa
	QString		ProfileIrcAlternateNickname;					// Alternate nickname in Irc chat if first one is already used
	QString		ProfileIrcNickname;							// Nickname used in Irc chat
	QString		ProfileIrcUserName;							// User name for Irc chat
	QString		ProfileJabberID;								// Jabber ID
	QString		ProfileLatitude;								// Location for a disgruntled P2P user to aim a missile
	QString		ProfileLongitude;								// Location for a disgruntled P2P user to aim a missile
	QString		ProfileMSNPassport;							// Microsoft's Messenger ID
	QString		ProfileMyspaceProfile;							// Myspace profile excluding http://profile.myspace.com/
	QString		ProfileRealName;								// User's real name
	QString		ProfileStateProvince;							// Your state or province
	QString		ProfileYahooID;								// Yahoo Messenger ID

	// Connection settings
	int			ConnectionCanAcceptIncoming;						// Can we accept incoming connections or are they being blocked
	bool		ConnectionDetectLoss;					// Detect loss of internet connection
	bool		ConnectionDetectReset;					// Detect regaining of internet connection
	int			ConnectionThrottle;						// Delay between connection attempts. (Neighbour connections)
	int			ConnectionFailureLimit;							// Max allowed connection failures (default = 3) (Neighbour connections)
	int			ConnectionFailurePenalty;							// Delay after connection failure (seconds, default = 300) (Neighbour connections)
	QString		ConnectionInAddress;								// Inbound IP address
	bool		ConnectionInBind;									// Force binding to selected address
	double		ConnectionInSpeed;								// Inbound internet connection speed in Kilobits/second
	QString		ConnectionOutAddress;								// Outbound IP address
	double		ConnectionOutSpeed;								// Outbound internet connection speed in Kilobits/second
	int			ConnectionPort;									// Incoming port
	bool		ConnectionRandomPort;								// Select a random incoming port
	int			ConnectionSendBuffer;								// Size of data send blocks
	int			ConnectionTimeout;							// Time to wait for a connection before dropping the connection
	int			ConnectionTimeoutHandshake;						// Time to wait on a handshake before dropping a connection
	int			ConnectionTimeoutTraffic;							// Time to wait for general network communications before dropping a connection
	bool		ConnectionXPsp2PatchedConnect;					// Connect to one network at a time. Don't download while connecting. (XPsp2)

	// Web settings
	bool		WebAres;									// Use Quazaa to open Ares links (arlnk:)
	bool		WebBrowserIntegration;						// Use Quazaa to manage browser downloads?
	bool		WebED2K;									// Use Quazaa to open eDonkey 2k links (eD2k:)
	bool		WebFoxy;									// Use Quazaa to open Foxy links (slightly altered magnet)
	bool		WebGnutella;								// Use Quazaa to open Gnutella links (gnutella:)
	bool		WebMagnet;									// Use Quazaa to open Magnet links (magnet:)
	QStringList	WebManageDownloadTypes;					// Use Quazaa to manage these download types
	bool		WebPiolet;									// Use Quazaa to open Piolet links (mp2p:)
	bool		WebTorrent;								// Use Quazaa to open torrent links (.torrent files and torrent:)

	// Web Services settings
	QString		WebServicesBitziAgent;
	bool		WebServicesBitziOkay;
	QString		WebServicesBitziWebSubmit;
	QString		WebServicesBitziWebView;
	QString		WebServicesBitziXML;
	QString		WebServicesShareMonkeyCid;							// Affiliate ID
	bool		WebServicesShareMonkeyOkay;
	bool		WebServicesShareMonkeySaveThumbnail;

	// Transfers settings
	double		TransfersBandwidthDownloads;								// Inbound speed limit in Bytes/seconds
	int			TransfersBandwidthHubIn;
	int			TransfersBandwidthHubOut;
	int			TransfersBandwidthHubUploads;
	int			TransfersBandwidthLeafIn;
	int			TransfersBandwidthLeafOut;
	int			TransfersBandwidthPeerIn;
	int			TransfersBandwidthPeerOut;
	int			TransfersBandwidthRequest;
	int			TransfersBandwidthUdpOut;
	double		TransfersBandwidthUploads;								// Outbound speed limit in Bytes/seconds
	int			TransfersMinTransfersRest;						// For how long at least to suspend Transfers each round
	int			TransfersRatesUnit;								// Units that the transfer rates are to be displayed in
	bool		TransfersRequireConnectedNetwork;				// Only upload/download to connected networks
	bool		TransfersSimpleProgressBar;						// Displays a simplified progress bar (lower CPU use)

	// Downloads settings
	bool		DownloadsAllowBackwards;							// Permit download to run in reverse when appropriate
	bool		DownloadsAutoClear;								// Automatically clear finished downloads
	int			DownloadsBufferSize;								// Size of download buffer
	int			DownloadsChunkSize;								// Size of download chunks
	int			DownloadsChunkStrap;								// Size of first download chunk
	int			DownloadsClearDelay;								// Time a download should be finished before clearing
	QString		DownloadsCompletePath;							// Where downloads are moved when they complete
	int			DownloadsConnectThrottle;						// Delay between download attempts. (Very important for routers)
	int			DownloadsDropFailedSourcesThreshold;				// The number of sources where Quazaa start dropping failed sources after only one attempt
	bool		DownloadsExpandDownloads;						// Expand new downloads by default
	bool		DownloadsFlushSD;
	QString		DownloadsIncompletePath;							// Where incomplete downloads are stored
	int			DownloadsMaxAllowedFailures;						// Max download failures before dropping a download source
	int			DownloadsMaxConnectingSources;					// The maximum number of sources that can be in the 'connecting' state. (Important for XPsp2)
	int			DownloadsMaxFiles;								// How many files download at once
	int			DownloadsMaxFileSearches;						// Number of files over the download limit that can search for sources for when they start. (Search, etc)
	int			DownloadsMaxReviews;								// Maximum number of reviews to store per download
	int			DownloadsMaxTransfers;							// How many total tranfers take place
	int			DownloadsMaxTransfersPerFile;					// How many transfers are allowed per file
	bool		DownloadsMetadata;								// Download metadata (ID3 tags, etc.)
	int			DownloadsMinSources;								// The minimum number of sources a download has before Quazaa regards it as having a problem
	bool		DownloadsNeverDrop;								// Do not drop bad sources (may pollute source list with many dead sources)
	int			DownloadsPushTimeout;							// Lower transfer timeout for push sources
	int			DownloadsQueueLimit;								// Longest queue to wait in. (0 to disable. This should be >800 or 0 to get good performance from ed2k)
	bool		DownloadsRequestHash;							// Request unknown hashes from active download sources
	bool		DownloadsRequestHTTP11;
	bool		DownloadsRequestURLENC;
	int			DownloadsRetryDelay;								// Time to wait between connection connection attempts to a particular download source
	int			DownloadsSaveInterval;							// Time between writes to disc of downloads
	int			DownloadsSearchPeriod;							// Time to send out search requests
	bool		DownloadsShowGroups;
	bool		DownloadsShowMonitorURLs;
	bool		DownloadsShowPercent;							// Display small green % complete bar on progress graphic
	bool		DownloadsShowSources;							// Display individual download sources in Downloads tree
	bool		DownloadsSortColumns;							// Allow user to sort downloads by clicking column headers
	bool		DownloadsSortSources;							// Automatically sort sources (Status, protocol, queue)
	int			DownloadsSourcesWanted;							// Number of sources Quazaa 'wants'. (Will not request more than this number of sources from ed2k)
	bool		DownloadsStaggardStart;							// Start file connections at different time intervals instead of all at the same time
	int			DownloadsStarveGiveUp;							// How long (in hours) before Quazaa will give up on a download and try another if it gets no data. (+ 0-9 h, depending on sources)
	int			DownloadsStarveTimeout;							// How long before Quazaa tries drops a connection for a particular file source
	bool		DownloadsURIPrompt;								// Show prompt before opening URIs
	bool		DownloadsVerifyED2K;								// Verify file integrity on eDonkey 2k downloads
	bool		DownloadsVerifyFiles;							// Verify file integrity
	bool		DownloadsVerifyTiger;							// Verify file integrity using Tiger hash
	bool		DownloadsWebHookEnable;
	QStringList	DownloadsWebHookExtensions;

	// Uploads settings
	bool		UploadsAllowBackwards;							// Allow data to be sent from end of range to begining where supported
	bool		UploadsAutoClear;								// Automatically clear completed uploads ('Completed' queue)
	int			UploadsClampdownFactor;						// % threshold where additional uploads are denied
	int			UploadsClampdownFloor;							// Floor to base clampdown factor off of
	int			UploadsClearDelay;								// Delay between auto-clears
	bool		UploadsDynamicPreviews;						// Dynamically create previews on request
	int			UploadsFreeBandwidthFactor;					// Amount of bandwidth allocated for each upload
	int			UploadsFreeBandwidthValue;						// Amount of bandwidth remaining for uploads
	bool		UploadsHubShareLimiting;						// Limit sharing in hub mode
	int			UploadsMaxPerHost;								// Max simultaneous uploads to one remote client
	int			UploadsPreviewQuality;							// Quality of dynamically created previews
	int			UploadsPreviewTransfers;						// Max simultaneous uploads of previews
	int			UploadsQueuePollMax;
	int			UploadsQueuePollMin;
	int			UploadsRewardQueuePercentage;					// The percentage of each reward queue reserved for uploaders
	int			UploadsRotateChunkLimit;						// Limit on the size of rotating chunks
	bool		UploadsShareHashset;							// Share the hashset for a particular file
	bool		UploadsShareMetadata;							// Share metadata (ID3 tags, etc.) for files
	bool		UploadsSharePartials;							// Share new partial downloads
	bool		UploadsSharePreviews;							// Share previews
	bool		UploadsShareTiger;								// Share tiger tree hashes
	bool		UploadsThrottleMode;							// Are we throttling upload bandwidth

	//Security settings
	bool		SecurityAllowProfileBrowse;						// Allow others to browse your user profile
	bool		SecurityAllowSharesBrowse;						// Allow others to browse your shares
	QStringList	SecurityBlockedAgentUploadFilter;				// List of bad user agents to block
	bool		SecurityDeleteFirewallException;				// Delete Firewall exception on shutdown
	bool		SecurityDeleteUPnPPorts;						// Delete forwarded ports on shutdown (UPnP)
	bool		SecurityED2kChatFilter;							// Filter known ed2k spam. (pretty bad- always on)
	bool		SecurityEnableFirewallException;				// Create Firewall exception at startup
	bool		SecurityEnableUPnP;								// Use UPnP to automatically set up firewalls/routers
	int			SecurityFirewallState;							// Is a firewall blocking incloming connections?
	bool		SecurityChatFilter;								// Filter out chat spam
	int			SecurityIrcFloodLimit;							// Number of messages sent by a user in 1 second before ignoring them
	bool		SecurityIrcFloodProtection;						// Automatically ignore users that flood chat rooms with text
	int			SecurityMaxMaliciousFileSize;					// Size for which to trigger malicious software search
	bool		SecurityRemoteEnable;							// Enable remote access?
	QString		SecurityRemotePassword;							// Remote access password
	QString		SecurityRemoteUsername;							// Remote access user name
	bool		SecuritySearchIgnoreLocalIP;					// Ingnore all 'local' (LAN) IPs
	bool		SecuritySearchIgnoreOwnIP;						// Do not accept any ports on your external IP as a source
	int			SecuritySearchSpamFilterThreshold;				// Percentage of spam hits which triggers file sources to be treated as spam
	bool		SecurityUPnPSkipWANIPSetup;						// Skip WANIPConn1 device setup (UPnP)
	bool		SecurityUPnPSkipWANPPPSetup;					// Skip WANPPPConn1 device setup (UPnP)

	// Gnutella settings
	bool		GnutellaCompressHub2Hub;						// Hub to Hub Link compression
	bool		GnutellaCompressHub2Leaf;						// Hub to Leaf Link Compression
	bool		GnutellaCompressLeaf2Hub;						// Leaf to Hub Link Compression
	int			GnutellaConnectFactor;
	int			GnutellaConnectThrottle;						// Delay between connection attempts (seconds)
	int			GnutellaHitsPerPacket;							// Max hits allowed for a single packet
	int			GnutellaHostCacheSize;							// Size of host cache
	int			GnutellaHostCacheView;
	int			GnutellaMaxHits;								// Max hits allowed for a single query
	int			GnutellaMaxResults;								// Maximum results to return to a single query
	int			GnutellaRouteCache;								// Cache for route to peer
	bool		GnutellaSpecifyProtocol;						// Specify G1 or G2 when initiating a connection

	// Gnutella2 settings
	int			Gnutella2ClientMode;								// Desired mode of operation: MODE_AUTO, MODE_LEAF, MODE_HUB
	bool		Gnutella2Enable;									// Connect to G2
	int			Gnutella2HAWPeriod;
	int			Gnutella2HostCount;								// Number of hosts in X-Try-Hubs
	int			Gnutella2HostCurrent;							// Index of current host
	int			Gnutella2HostExpire;								// Inactive time before a host expires
	int			Gnutella2HubHorizonSize;
	bool		Gnutella2HubVerified;							// Verified we are operating as a hub
	int			Gnutella2KHLHubCount;
	int			Gnutella2KHLPeriod;
	int			Gnutella2LNIPeriod;
	int			Gnutella2NumHubs;								// Number of hubs a leaf has (Leaf to Hub)
	int			Gnutella2NumLeafs;								// Number of leafs a hub has (Hub to Leaf)
	int			Gnutella2NumPeers;								// Number of peers a hub has (Hub to Hub)
	int			Gnutella2PingRate;								// Time in milliseconds between pings
	int			Gnutella2PingRelayLimit;							// Number of other leafs to forward a /PI/UDP to: 10 - 30
	int			Gnutella2QueryGlobalThrottle;					// Max G2 query rate (Cannot exceed 8/sec)
	int			Gnutella2QueryHostDeadline;						// Time before ending queries
	int			Gnutella2QueryHostThrottle;						// Bandwidth throttling for queries
	int			Gnutella2QueryLimit;								// Maximum amount of concurrent queries
	int			Gnutella2RequeryDelay;							// Time before sending another query
	int			Gnutella2UdpBuffers;								// UDP protocol buffer size
	int			Gnutella2UdpGlobalThrottle;						// Throttle UDP protocol bandwidth
	int			Gnutella2UdpInExpire;							// Time before incoming an incloming UDP connection
	int			Gnutella2UdpInFrames;							// UDP protocol in frame size
	int			Gnutella2UdpMTU;									// UDP protocol maximum transmission units
	int			Gnutella2UdpOutExpire;							// Time before dropping a UDP connection
	int			Gnutella2UdpOutFrames;							// UDP protocol out frame size
	int			Gnutella2UdpOutResend;							// Time before resending a UDP protocol packet

	// Gnutella1 settings
	int			Gnutella1ClientMode;								// Desired mode of operation: MODE_AUTO, MODE_LEAF, MODE_ULTRAPEER
	int			Gnutella1DefaultTTL;								// Default time to live
	bool		Gnutella1Enable;									// Connect to G1
	bool		Gnutella1EnableDIPPSupport;						// Handle GDNA host cache exchange
	bool		Gnutella1EnableGGEP;
	int			Gnutella1HitQueueLimit;							// Protect G1 clients from badly configured queues
	int			Gnutella1HostCount;								// Number of hosts in X-Try-Ultrapeers
	int			Gnutella1HostExpire;
	int			Gnutella1MaxHostsInPongs;						// The number of hosts included in the response of pings having SCP GGEP block
	int			Gnutella1MaximumPacket;
	int			Gnutella1MaximumQuery;
	int			Gnutella1MaximumTTL;								// Maximum time to live
	int			Gnutella1NumHubs;								// Number of ultrapeers a leaf has (Leaf to Ultrapeer)
	int			Gnutella1NumLeafs;								// Number of leafs an ultrapeer has (Ultrapeer to Leaf)
	int			Gnutella1NumPeers;								// Number of peers an ultrapeer has (Ultrapeer to Ultrapper)
	int			Gnutella1PacketBufferSize;						// Size of the packet buffer
	int			Gnutella1PacketBufferTime;
	int			Gnutella1PingFlood;
	int			Gnutella1PingRate;
	int			Gnutella1PongCache;
	int			Gnutella1PongCount;
	bool		Gnutella1QueryHitUTF8;							// Use UTF-8 encoding to read Gnutella1 QueryHit packets
	bool		Gnutella1QuerySearchUTF8;						// Use UTF-8 encoding to create Gnutella1 Query packets
	int			Gnutella1QueryThrottle;
	int			Gnutella1RequeryDelay;
	int			Gnutella1SearchTTL;								// Search time to live
	bool		Gnutella1StrictPackets;
	int			Gnutella1TranslateTTL;							// Translate time to live
	bool		Gnutella1VendorMsg;

	// Ares settings
	bool		AresEnable;									// Connect to Ares
	int			AresMaximumDownloads;						// Max concurrent downloads
	int			AresMaximumUploads;							// Max concurrent uploads
	int			AresMaximumUploadsPerUser;					// Max uploads per user

	// EDonkey settings
	QString		EDonkeyDefaultServerFlags;						// Default server flags (for UDP searches)
	int			EDonkeyDequeueTime;
	bool		EDonkeyEnable;									// Connect to eDonkey 2k
	bool		EDonkeyEnableKad;								// Connect to the Kad discovery service
	bool		EDonkeyEndgame;								// Allow endgame mode when completing downloads. (Download same chunk from multiple sources)
	int			EDonkeyExtendedRequest;
	bool		EDonkeyFastConnect;							// Try connecting to 2 servers to get online faster
	bool		EDonkeyForceHighID;							// Reconnect if low-id. (once only)
	int			EDonkeyFrameSize;
	int			EDonkeyGetSourcesThrottle;						// Max rate a general GetSources can done
	bool		EDonkeyLargeFileSupport;						// Allow 64 bit file sizes
	bool		EDonkeyLearnNewServers;						// Get new servers from servers
	bool		EDonkeyLearnNewServersClient;					// Get new servers from clients
	bool		EDonkeyMagnetSearch;							// Search for magnets over ed2k (lower server load)
	int			EDonkeyMaxClients;								// Max ed2k client links
	int			EDonkeyMaxResults;								// Stop searching after this many search results
	int			EDonkeyMaxShareCount;							// Hard limit on file list sent to server
	bool		EDonkeyMetAutoQuery;							// Auto query for a new server list
	int			EDonkeyMinServerFileSize;						// Minimum size a file in the library must be in order to be included in the server file list. (In KB)
	int			EDonkeyNumServers;								// 1
	int			EDonkeyPacketThrottle;							// ED2K packet rate limiter
	int			EDonkeyQueryFileThrottle;						// Max rate a file can have GetSources done
	int			EDonkeyQueryGlobalThrottle;					// Global throttle for all ed2k searches (TCP, UDP, manual and auto)
	int			EDonkeyQueueRankThrottle;						// How frequently queue ranks are sent
	int			EDonkeyQueryServerThrottle;					// Max rate at which an individual server can be queried
	int			EDonkeyReAskTime;
	int			EDonkeyRequestPipe;
	int			EDonkeyRequestSize;
	bool		EDonkeySearchCachedServers;					// Search on known remote servers in the server cache
	bool		EDonkeySendPortServer;							// Send port in tag to ed2k servers. (not needed for newer servers)
	QString		EDonkeyServerListURL;							// URL address to auto query for a new server list
	bool		EDonkeyServerWalk;								// Enable global UDP walk of servers
	int			EDonkeySourceThrottle;							// ED2K source rate limiter
	int			EDonkeyStatsGlobalThrottle;					// Global throttle for server UDP stats requests
	int			EDonkeyStatsServerThrottle;					// Max rate at which an individual server can be asked for stats

	// BitTorrent settings
	bool		BitTorrentAutoClear;								// Clear completed torrents when they meet the required share ratio
	bool		BitTorrentAutoSeed;								// Automatically re-seed most recently completed torrent on start-up
	int			BitTorrentBandwidthPercentage;					// Percentage of bandwidth to use when BT active.
	int			BitTorrentClearRatio;								// Share ratio a torrent must reach to be cleared. (Minimum 100%)
	int			BitTorrentCodePage;								// The code page to assume for a .torrent file if it isn't UTF-8
	QString		BitTorrentDefaultTracker;							// Default tracker to use if trackers listed in torrent are down
	int			BitTorrentDefaultTrackerPeriod;					// Delay between tracker contact attempts if one is not specified by tracker
	int			BitTorrentDhtPruneTime;
	int			BitTorrentDownloadConnections;					// Number of active torrent connections allowed
	int			BitTorrentDownloadTorrents;						// Number of torrents to download at once
	bool		BitTorrentEndgame;								// Allow endgame mode when completing torrents. (Download same chunk from multiple sources)
	bool		BitTorrentExtraKeys;								// Check for '.utf8' keys if there is an encoding error
	int			BitTorrentLinkPing;
	int			BitTorrentLinkTimeout;
	bool		BitTorrentManaged;								// Managed Torrents
	bool		BitTorrentPreferBTSources;						// Prefer downloading from BT sources when appropriate
	int			BitTorrentRandomPeriod;
	int			BitTorrentRequestLimit;
	int			BitTorrentRequestPipe;
	int			BitTorrentRequestSize;
	bool		BitTorrentShowFilesInDownload;					// Display Individual Files In The Download Tree
	int			BitTorrentSourceExchangePeriod;					// Time to exchange sources with new peers
	bool		BitTorrentStartPaused;							// Start torrents in a paused state
	bool		BitTorrentTestPartials;							// Do on integrity check of Bittorrent partials
	QString		BitTorrentTorrentPath;							// Where .torrent files are stored
	bool		BitTorrentTrackerKey;								// Send a key (random value) to trackers
	int			BitTorrentUploadCount;							// Number of active torrent uploads allowed
	bool		BitTorrentUseKademlia;							// Use Kademlia with bittorrent
	bool		BitTorrentUseSaveDialog;							// Use torrent save dialog or use default settings
	bool		BitTorrentUseTemp;								// Store incomplete torrent downloads in the temporary folder

	//Discovery settings
	int			DiscoveryAccessThrottle;
	int			DiscoveryBootstrapCount;
	int			DiscoveryCacheCount;								// Limit ability to learn new caches
	int			DiscoveryDefaultUpdate;
	bool		DiscoveryEnableG1GWC;
	int			DiscoveryFailureLimit;
	int			DiscoveryLowpoint;
	int			DiscoveryUpdatePeriod;

	// Scheduler settings
};

extern QuazaaSettings quazaaSettings;

#endif // QUAZAASETTINGS_H
