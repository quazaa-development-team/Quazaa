//
// persistentsettings.h
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

#ifndef PERSISTENTSETTINGS_H
#define PERSISTENTSETTINGS_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QColor>
#include <QtGui/QMainWindow>

class PersistentSettings : public QObject
{
public:
	PersistentSettings();
	void saveMainWindowState(QMainWindow *mainWindow);
	void loadMainWindowState(QMainWindow *mainWindow);
	void loadSettings();
	void saveSettings();
	void saveProfile();
	void saveWizard();
	void saveLanguage();
	void loadLanguage();

	// Attributes
public:
	struct sMainWindowState
	{
		int			ActiveTab;								// The active tab (Home, Library, Media, etc.)
		int			LibraryTab;								// Organizer or Shared Folders
		int			SearchType;								// What type of file is currently being searched for
		QString		HomeSearch;								// The text typed into the search text box on the home tab
		int			HomeSearchType;							// The type of search in the home tab
		bool		PlaylistVisible;						// Is the media player visible or not
		bool		ShowSearch;								// Should the search sidebar be shown
		bool		ShowSearchDetails;						// Should details be shown for selected search items
	} MainWindowState;

	struct sLive
	{
		bool		AdultWarning;							// Has the user been warned about the adult filter?
		bool		AutoClose;
		int			BandwidthScale;							// Monitor slider settings
		bool		DefaultED2KServersLoaded;				// Has Quazaa already loaded default ED2K servers?
		bool		DiskSpaceStop;							// Has Quazaa paused all downloads due to critical disk space?
		bool		DiskSpaceWarning;						// Has the user been warned of low disk space?
		bool		DiskWriteWarning;						// Has the user been warned of write problems?
		bool		DonkeyServerWarning;					// Has the user been warned about having an empty server list?
		QString		LastDuplicateHash;						// Stores the hash of the file about which the warning was shown
		bool		MaliciousWarning;						// Is the warning dialog opened?
		bool		QueueLimitWarning;						// Has the user been warned about limiting the max Q position accepted?
		bool		UploadLimitWarning;						// Has the user been warned about the ed2k/BT ratio?
	} Live;

	struct sBasic
	{
		bool		ChatTips;								// Show tooltips on chat view?
		int			CloseMode;								// What happens when the close button is pressed? (0 - Ask, 1 - Close, 2 - Minimise)
		bool		ConnectOnStartup;						// Connect to networks when starting Quazaa
		bool		DefaultLanguage;						// Is the default language being used
		int			DiskSpaceStop;							// Value at which to pause all downloads due to low disk space
		int			DiskSpaceWarning;						// Value at which to warn the user about low disk space
		bool		DownloadsTips;							// Show tooltips on downloads view?
		bool		FirstRun;								// Is this the first time Quazaa was run?
		bool		GUIMode;								// Start in GUI or terminal/command prompt?
		QString		LanguageFile;							// What language does the user prefer
		bool		LibraryTips;							// Show tooltips on library view?
		bool		MediaTips;								// Show tooltips on media view?
		bool		NeighboursTips;							// Show tooltips on neigbours view?
		bool		SearchTips;								// Show tooltips on searches view?
		QString		SkinFile;								// Full path to the skin file
		bool		StartWithSystem;						// Start with operating system
		int			TipDelay;								// Amount of time the mouse is hovered before showing a tooltip
		bool		TipLowResMode;							// Low resolution tooltips for performance issues.
		int			TipTransparency;						// Alpha transarency level for tooltips
		bool		TrayMinimise;							// Minimise to tray or task panel.
		bool		UploadsTips;							// Show tooltips on uploads view?
		bool		UsingSkin;								// Is the user using a skin instead of the default interface?

		/*Possibly use these later as things start coming together. Some won't be needed. TODO: Delete any useless variables.
		QString		Path;									// Installation path for Quazaa
		QString		UserPath;								// Path for user data. (May be the same as above for single user installs)
		bool		MultiUser;								// Multiuser installation		
		bool		SizeLists;
		bool		HashIntegrity;
		bool		AlwaysOpenURLs;
		bool		LanguageRTL;							// Right-to-Left GUI (2000, XP only)
		bool		IgnoreXPsp2;							// Ignore the presence of Windows XP SP2 limits
		bool		DebugBTSources;							// Display received sources for BT download when seeding
		bool		ItWasLimited;							// If the user patched the half-open connection limit we change the settings back to gain full speed
		int			SmartVersion;							// Settings version
		bool		Running;*/
	} Basic;

	struct sParental
	{
		QStringList	AdultFilter;							// List of words to censor
		bool		ChatAdultCensor;						// Censor adlt language from chat
		bool		FilterAdultSearchResults;				// Filter adult content from searches
	} Parental;
	
	struct sLogging
	{
		bool		DebugLog;								// Create a log file
		int			LogLevel;								// Log severity (0 - MSG_ERROR .. 4 - MSG_DEBUG)
		bool		LogShowTimestamp;						// Show timestamps in the system log?
		int			MaxDebugLogSize;						// Max size of the log file
		bool		SearchLog;								// Display search facility log information
	} Logging;

	struct sLibrary
	{
		QString		FilterURI;
		bool		GhostFiles;								// Create ghost files specifying why files were deleted on deletion
		bool		HashWindow;								// Display hashing progress window
		bool		HighPriorityHashing;					// Use high priority hashing
		int			HighPriorityHashingSpeed;				// desired speed in MB/s when hashing with hi priority
		int			HistoryDays;							// Remember files for this many days
		int			HistoryTotal;							// Remember this many total files in Library history
		int			LowPriorityHashingSpeed;				// desired speed in MB/s when hashing with low priority
		QStringList	NeverShareTypes;						// Never share these file types
		bool		PartialMatch;
		bool		PreferAPETags;							// Read APE tags first and only then ID3 tags from audio files
		int			QueryRouteSize;
		bool		RememberViews;							// Remember custom folder views
		QStringList	SafeExecuteTypes;						// These file types are safe to open
		bool		ScanAPE;								// Enable .ape,.mac,.apl metadata extraction by internals
		bool		ScanASF;								// Enable .asf,.wma,.wmv metadata extraction by internals
		bool		ScanAVI;								// Enable .avi metadata extraction by internals
		bool		ScanCHM;								// Enable .chm metadata extraction by internals
		bool		ScanEXE;								// Enable .exe,.dll metadata extraction by internals
		bool		ScanImage;								// Enable .jpg,.jpeg,.gif,.png,.bmp metadata extraction by internals
		bool		ScanMP3;								// Enable .mp3 metadata extraction by internals
		bool		ScanMPC;								// Enable .mpc,.mpp,.mp+ metadata extraction by internals
		bool		ScanMPEG;								// Enable .mpeg,.mpg metadata extraction by internals
		bool		ScanMSI;								// Enable .msi metadata extraction by internals
		bool		ScanOGG;								// Enable .ogg metadata extraction by internals
		bool		ScanPDF;								// Enable .pdf metadata extraction by internals
		QString		SchemaURI;
		QStringList	Shares;									// A list of your shared folders
		bool		ShowCoverArt;
		bool		SmartSeriesDetection;					// Organize video files in Library by using predefined patterns
		int			SourceExpire;
		bool		SourceMesh;
		int			ThumbSize;
		int			TigerHeight;
		int			TreeSize;
		bool		WatchFolders;							// Watch shared folders and update hashes when changed
		int			WatchFoldersTimeout;					// Quit watching folder after time

		/*
		bool		ShowVirtual;
		int			PanelSize;
		bool		ShowPanel;
		bool		UseFolderGUID;							// Save/Load folder GUID using NTFS stream
		bool		MarkFileAsDownload;						// Mark downloaded file using NTFS stream as Internet Explorer
		bool		UseCustomFolders;						// Use desktop.ini
		*/
	} Library;

	struct sMediaPlayer
	{
		int			Aspect;									// Size video is displayed in
		QString		AudioVisualPlugin;						// Path to visual plugin for audio. Null if none is used.
		QString		CustomPlayerPath;						// Path to the custom player executable
		int			MediaHandler;							// Media Player (0 Built In, 1 Shell Open, 2 Custom)
		QStringList	FileTypes;								// Try to open these file types as media files
		bool		ListVisible;							// Is playlist visible
		bool		Mute;									// Is sound muted
		QStringList	Playlist;								// Save playlist so it can be reopened when closing/opening Quazaa
		bool		Random;									// Is random turned on for media files?
		bool		Repeat;									// Is repeat turned on for media files?
		bool		StatusVisible;							// Display time remaining, etc
		int			Volume;									// Sound volume
		int			Zoom;									// The zoom level for video

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
	} MediaPlayer;

	struct sSearch
	{
		QString		BlankSchemaURI;
		int			BrowseTreeSize;
		int			ClearPrevious;							// Clear previous search results? 0 - ask user; 1 - no; 2 - yes.
		bool		EnableFileTransfers;					// Enable file transfers through IRC
		bool		ExpandSearchMatches;					// Expand multi-source search matches
		int			FilterMask;
		bool		HighlightNew;
		QString		LastSchemaURI;
		int			MaxPreviewLength;						// Max size of search previews
		QString		MonitorFilter;
		int			MonitorQueue;
		QString		MonitorSchemaURI;
		bool		SchemaTypes;
		int			SearchThrottle;							// How often each individual search may run. Low values may cause source finding to get overlooked.
		QString		ShareMonkeyBaseURL;
		bool		ShowNames;
		bool		SwitchOnDownload;						// Switch to transfers view when starting downloads
		//bool		SanityCheck;							// Drop hits of banned hosts
	} Search;

	struct sChat
	{
		int			AwayMessageIdleTime;					// Time in secs of idle system time before showing away message
		QString		Background;								// Path to an image for the background of the chat window
		bool		ConnectOnStartup;						// Connect to the chat server and enter rooms on startup
		QColor		ColorChatBackground;					// Color for the background of the chat window
		QColor		ColorNormalText;						// Color for normal text in the chat window
		QColor		ColorNoticesText;						// Color for notices in the chat window
		QColor		ColorRoomActionsText;					// Color for room actions in the chat window
		QColor		ColorServerMessagesText;				// Color for server messages in the chat window
		QColor		ColorTopicsText;						// Color for topics in the chat window
		bool		EnableChatAllNetworks;					// Is chat allowed over other protocols? (ed2k, etc)
		bool		EnableFileTransfers;					// Enable IRC File Transfers
		bool		GnutellaChatEnable;						// Is Gnutella chat enabled with compatible clients?
		QString		IrcServerName;							// Web address of the Irc chat server
		int			IrcServerPort;							// Port to connect to the chat server on
		QFont		ScreenFont;								// Font for the chat screen
		bool		ShowTimestamp;							// Show timestamps at the beginning of messages
	} Chat;

	struct sProfile
	{
		int			Age;									// Age..
		QString		AolScreenName;							// Aol screen name
		QString		AvatarPath;								// Path to an avatar image file. Quazaa logo used if blank
		QString		Biography;								// Your life story..
		QString		City;									// Your city
		QString		Country;								// Your country..
		QString		Email;									// EMail address
		QStringList	Favorites;								// Favorite websites
		QStringList	FavoritesURL;							// Favorite Websites URLs
		int			Gender;									// Gender..
		QString		GnutellaScreenName;						// The name displayed in searches, uploads and downloads and when users browse your system
		QString		GUID;									// Unique ID for each client. Can be regenerated
		QString		ICQuin;									// Identification number in ICQ
		QStringList	Interests;								// What do you like to do other than use Quazaa
		QString		IrcAlternateNickname;					// Alternate nickname in Irc chat if first one is already used
		QString		IrcNickname;							// Nickname used in Irc chat
		QString		IrcUserName;							// User name for Irc chat
		QString		JabberID;								// Jabber ID
		QString		Latitude;								// Location for a disgruntled P2P user to aim a missile
		QString		Longitude;								// Location for a disgruntled P2P user to aim a missile
		QString		MSNPassport;							// Microsoft's Messenger ID
		QString		MyspaceProfile;							// Myspace profile excluding http://profile.myspace.com/
		QString		RealName;								// User's real name
		QString		StateProvince;							// Your state or province
		QString		YahooID;								// Yahoo Messenger ID
	} Profile;

	struct sConnection
	{
		int			CanAcceptIncoming;						// Can we accept incoming connections or are they being blocked
		int			ConnectThrottle;						// Delay between connection attempts. (Neighbour connections)
		bool		DetectConnectionLoss;					// Detect loss of internet connection
		bool		DetectConnectionReset;					// Detect regaining of internet connection
		int			FailureLimit;							// Max allowed connection failures (default = 3) (Neighbour connections)
		int			FailurePenalty;							// Delay after connection failure (seconds, default = 300) (Neighbour connections)
		QString		InAddress;								// Inbound IP address
		bool		InBind;									// Force binding to selected address
		double		InSpeed;								// Inbound internet connection speed in Kilobits/second
		QString		OutAddress;								// Outbound IP address
		double		OutSpeed;								// Outbound internet connection speed in Kilobits/second
		int			Port;									// Incoming port
		bool		RandomPort;								// Select a random incoming port
		int			SendBuffer;								// Size of data send blocks
		int			TimeoutConnect;							// Time to wait for a connection before dropping the connection
		int			TimeoutHandshake;						// Time to wait on a handshake before dropping a connection
		int			TimeoutTraffic;							// Time to wait for general network communications before dropping a connection
		bool		XPsp2PatchedConnect;					// Connect to one network at a time. Don't download while connecting. (XPsp2)
	} Connection;

	struct sWeb
	{
		bool		Ares;									// Use Quazaa to open Ares links (arlnk:)
		bool		BrowserIntegration;						// Use Quazaa to manage browser downloads?
		bool		ED2K;									// Use Quazaa to open eDonkey 2k links (eD2k:)
		bool		Foxy;									// Use Quazaa to open Foxy links (slightly altered magnet)
		bool		Gnutella;								// Use Quazaa to open Gnutella links (gnutella:)
		bool		Magnet;									// Use Quazaa to open Magnet links (magnet:)
		QStringList	ManageDownloadTypes;					// Use Quazaa to manage these download types
		bool		Piolet;									// Use Quazaa to open Piolet links (mp2p:)
		bool		Torrent;								// Use Quazaa to open torrent links (.torrent files and torrent:)
	} Web;

	struct sWebServices
	{
		QString		BitziAgent;
		bool		BitziOkay;
		QString		BitziWebSubmit;
		QString		BitziWebView;
		QString		BitziXML;
		QString		ShareMonkeyCid;							// Affiliate ID
		bool		ShareMonkeyOkay;
		bool		ShareMonkeySaveThumbnail;
	} WebServices;

	struct sTransfers
	{
		double		BandwidthDownloads;								// Inbound speed limit in Bytes/seconds
		int			BandwidthHubIn;
		int			BandwidthHubOut;
		int			BandwidthHubUploads;
		int			BandwidthLeafIn;
		int			BandwidthLeafOut;
		int			BandwidthPeerIn;
		int			BandwidthPeerOut;
		int			BandwidthRequest;
		int			BandwidthUdpOut;
		double		BandwidthUploads;								// Outbound speed limit in Bytes/seconds
		int			MinTransfersRest;						// For how long at least to suspend Transfers each round
		int			RatesUnit;								// Units that the transfer rates are to be displayed in
		bool		RequireConnectedNetwork;				// Only upload/download to connected networks
		bool		SimpleProgressBar;						// Displays a simplified progress bar (lower CPU use)
	} Transfers;

	struct sDownloads
	{
		bool		AllowBackwards;							// Permit download to run in reverse when appropriate
		bool		AutoClear;								// Automatically clear finished downloads
		int			BufferSize;								// Size of download buffer
		int			ChunkSize;								// Size of download chunks
		int			ChunkStrap;								// Size of first download chunk
		int			ClearDelay;								// Time a download should be finished before clearing
		QString		CompletePath;							// Where downloads are moved when they complete
		int			ConnectThrottle;						// Delay between download attempts. (Very important for routers)
		int			DropFailedSourcesThreshold;				// The number of sources where Quazaa start dropping failed sources after only one attempt
		bool		ExpandDownloads;						// Expand new downloads by default
		bool		FlushSD;
		QString		IncompletePath;							// Where incomplete downloads are stored
		int			MaxAllowedFailures;						// Max download failures before dropping a download source
		int			MaxConnectingSources;					// The maximum number of sources that can be in the 'connecting' state. (Important for XPsp2)
		int			MaxFiles;								// How many files download at once
		int			MaxFileSearches;						// Number of files over the download limit that can search for sources for when they start. (Search, etc)
		int			MaxReviews;								// Maximum number of reviews to store per download
		int			MaxTransfers;							// How many total tranfers take place
		int			MaxTransfersPerFile;					// How many transfers are allowed per file
		bool		Metadata;								// Download metadata (ID3 tags, etc.)
		int			MinSources;								// The minimum number of sources a download has before Quazaa regards it as having a problem
		bool		NeverDrop;								// Do not drop bad sources (may pollute source list with many dead sources)
		int			PushTimeout;							// Lower transfer timeout for push sources
		int			QueueLimit;								// Longest queue to wait in. (0 to disable. This should be >800 or 0 to get good performance from ed2k)
		bool		RequestHash;							// Request unknown hashes from active download sources
		bool		RequestHTTP11;
		bool		RequestURLENC;
		int			RetryDelay;								// Time to wait between connection connection attempts to a particular download source
		int			SaveInterval;							// Time between writes to disc of downloads
		int			SearchPeriod;							// Time to send out search requests
		bool		ShowGroups;
		bool		ShowMonitorURLs;
		bool		ShowPercent;							// Display small green % complete bar on progress graphic
		bool		ShowSources;							// Display individual download sources in Downloads tree
		bool		SortColumns;							// Allow user to sort downloads by clicking column headers
		bool		SortSources;							// Automatically sort sources (Status, protocol, queue)
		int			SourcesWanted;							// Number of sources Quazaa 'wants'. (Will not request more than this number of sources from ed2k)
		bool		StaggardStart;							// Start file connections at different time intervals instead of all at the same time
		int			StarveGiveUp;							// How long (in hours) before Quazaa will give up on a download and try another if it gets no data. (+ 0-9 h, depending on sources)
		int			StarveTimeout;							// How long before Quazaa tries drops a connection for a particular file source
		bool		URIPrompt;								// Show prompt before opening URIs
		bool		VerifyED2K;								// Verify file integrity on eDonkey 2k downloads
		bool		VerifyFiles;							// Verify file integrity
		bool		VerifyTiger;							// Verify file integrity using Tiger hash
		bool		WebHookEnable;
		QStringList	WebHookExtensions;
	} Downloads;

	struct sUploads
	{
		bool		AllowBackwards;							// Allow data to be sent from end of range to begining where supported
		bool		AutoClear;								// Automatically clear completed uploads ('Completed' queue)
		int			ClampdownFactor;						// % threshold where additional uploads are denied
		int			ClampdownFloor;							// Floor to base clampdown factor off of
		int			ClearDelay;								// Delay between auto-clears
		bool		DynamicPreviews;						// Dynamically create previews on request
		int			FreeBandwidthFactor;					// Amount of bandwidth allocated for each upload
		int			FreeBandwidthValue;						// Amount of bandwidth remaining for uploads
		bool		HubShareLimiting;						// Limit sharing in hub mode
		int			MaxPerHost;								// Max simultaneous uploads to one remote client
		int			PreviewQuality;							// Quality of dynamically created previews
		int			PreviewTransfers;						// Max simultaneous uploads of previews
		int			QueuePollMax;
		int			QueuePollMin;
		int			RewardQueuePercentage;					// The percentage of each reward queue reserved for uploaders
		int			RotateChunkLimit;						// Limit on the size of rotating chunks
		bool		ShareHashset;							// Share the hashset for a particular file
		bool		ShareMetadata;							// Share metadata (ID3 tags, etc.) for files
		bool		SharePartials;							// Share new partial downloads
		bool		SharePreviews;							// Share previews
		bool		ShareTiger;								// Share tiger tree hashes
		bool		ThrottleMode;							// Are we throttling upload bandwidth
	} Uploads;

	struct sSecurity
	{
		bool		AllowProfileBrowse;						// Allow others to browse your user profile
		bool		AllowSharesBrowse;						// Allow others to browse your shares
		QStringList	BlockedAgentUploadFilter;				// List of bad user agents to block
		bool		DeleteFirewallException;				// Delete Firewall exception on shutdown
		bool		DeleteUPnPPorts;						// Delete forwarded ports on shutdown (UPnP)
		bool		ED2kChatFilter;							// Filter known ed2k spam. (pretty bad- always on)
		bool		EnableFirewallException;				// Create Firewall exception at startup
		bool		EnableUPnP;								// Use UPnP to automatically set up firewalls/routers
		int			FirewallState;							// Is a firewall blocking incloming connections?
		bool		ChatFilter;								// Filter out chat spam
		int			IrcFloodLimit;							// Number of messages sent by a user in 1 second before ignoring them
		bool		IrcFloodProtection;						// Automatically ignore users that flood chat rooms with text
		int			MaxMaliciousFileSize;					// Size for which to trigger malicious software search
		bool		RemoteEnable;							// Enable remote access?
		QString		RemotePassword;							// Remote access password
		QString		RemoteUsername;							// Remote access user name
		bool		SearchIgnoreLocalIP;					// Ingnore all 'local' (LAN) IPs
		bool		SearchIgnoreOwnIP;						// Do not accept any ports on your external IP as a source
		int			SearchSpamFilterThreshold;				// Percentage of spam hits which triggers file sources to be treated as spam
		bool		UPnPSkipWANIPSetup;						// Skip WANIPConn1 device setup (UPnP)
		bool		UPnPSkipWANPPPSetup;					// Skip WANPPPConn1 device setup (UPnP)
	} Security;

	struct sGnutella
	{
		bool		CompressHub2Hub;						// Hub to Hub Link compression
		bool		CompressHub2Leaf;						// Hub to Leaf Link Compression
		bool		CompressLeaf2Hub;						// Leaf to Hub Link Compression
		int			ConnectFactor;
		int			ConnectThrottle;						// Delay between connection attempts (seconds)
		int			HitsPerPacket;							// Max hits allowed for a single packet
		int			HostCacheSize;							// Size of host cache
		int			HostCacheView;
		int			MaxHits;								// Max hits allowed for a single query
		int			MaxResults;								// Maximum results to return to a single query
		int			RouteCache;								// Cache for route to peer
		bool		SpecifyProtocol;						// Specify G1 or G2 when initiating a connection
	} Gnutella;

	struct sGnutella2
	{
		int			ClientMode;								// Desired mode of operation: MODE_AUTO, MODE_LEAF, MODE_HUB
		bool		Enable;									// Connect to G2
		int			HAWPeriod;
		int			HostCount;								// Number of hosts in X-Try-Hubs
		int			HostCurrent;							// Index of current host
		int			HostExpire;								// Inactive time before a host expires
		int			HubHorizonSize;
		bool		HubVerified;							// Verified we are operating as a hub
		int			KHLHubCount;
		int			KHLPeriod;
		int			LNIPeriod;
		int			NumHubs;								// Number of hubs a leaf has (Leaf to Hub)
		int			NumLeafs;								// Number of leafs a hub has (Hub to Leaf)
		int			NumPeers;								// Number of peers a hub has (Hub to Hub)
		int			PingRate;								// Time in milliseconds between pings
		int			PingRelayLimit;							// Number of other leafs to forward a /PI/UDP to: 10 - 30
		int			QueryGlobalThrottle;					// Max G2 query rate (Cannot exceed 8/sec)
		int			QueryHostDeadline;						// Time before ending queries
		int			QueryHostThrottle;						// Bandwidth throttling for queries
		int			QueryLimit;								// Maximum amount of concurrent queries
		int			RequeryDelay;							// Time before sending another query
		int			UdpBuffers;								// UDP protocol buffer size
		int			UdpGlobalThrottle;						// Throttle UDP protocol bandwidth
		int			UdpInExpire;							// Time before incoming an incloming UDP connection
		int			UdpInFrames;							// UDP protocol in frame size
		int			UdpMTU;									// UDP protocol maximum transmission units
		int			UdpOutExpire;							// Time before dropping a UDP connection
		int			UdpOutFrames;							// UDP protocol out frame size
		int			UdpOutResend;							// Time before resending a UDP protocol packet
	} Gnutella2;

	struct sGnutella1
	{
		int			ClientMode;								// Desired mode of operation: MODE_AUTO, MODE_LEAF, MODE_ULTRAPEER
		int			DefaultTTL;								// Default time to live
		bool		Enable;									// Connect to G1
		bool		EnableDIPPSupport;						// Handle GDNA host cache exchange
		bool		EnableGGEP;
		int			HitQueueLimit;							// Protect G1 clients from badly configured queues
		int			HostCount;								// Number of hosts in X-Try-Ultrapeers
		int			HostExpire;
		int			MaxHostsInPongs;						// The number of hosts included in the response of pings having SCP GGEP block
		int			MaximumPacket;
		int			MaximumQuery;
		int			MaximumTTL;								// Maximum time to live
		int			NumHubs;								// Number of ultrapeers a leaf has (Leaf to Ultrapeer)
		int			NumLeafs;								// Number of leafs an ultrapeer has (Ultrapeer to Leaf)
		int			NumPeers;								// Number of peers an ultrapeer has (Ultrapeer to Ultrapper)
		int			PacketBufferSize;						// Size of the packet buffer
		int			PacketBufferTime;
		int			PingFlood;
		int			PingRate;
		int			PongCache;
		int			PongCount;
		bool		QueryHitUTF8;							// Use UTF-8 encoding to read Gnutella1 QueryHit packets
		bool		QuerySearchUTF8;						// Use UTF-8 encoding to create Gnutella1 Query packets
		int			QueryThrottle;
		int			RequeryDelay;
		int			SearchTTL;								// Search time to live
		bool		StrictPackets;
		int			TranslateTTL;							// Translate time to live
		bool		VendorMsg;
	} Gnutella1;

	struct sAres
	{
		bool		Enable;									// Connect to Ares
		int			MaximumDownloads;						// Max concurrent downloads
		int			MaximumUploads;							// Max concurrent uploads
		int			MaximumUploadsPerUser;					// Max uploads per user
	} Ares;

	struct seDonkey2k
	{
		QString		DefaultServerFlags;						// Default server flags (for UDP searches)
		int			DequeueTime;
		bool		Enable;									// Connect to eDonkey 2k
		bool		EnableKad;								// Connect to the Kad discovery service
		bool		Endgame;								// Allow endgame mode when completing downloads. (Download same chunk from multiple sources)
		int			ExtendedRequest;
		bool		FastConnect;							// Try connecting to 2 servers to get online faster
		bool		ForceHighID;							// Reconnect if low-id. (once only)
		int			FrameSize;
		int			GetSourcesThrottle;						// Max rate a general GetSources can done
		bool		LargeFileSupport;						// Allow 64 bit file sizes
		bool		LearnNewServers;						// Get new servers from servers
		bool		LearnNewServersClient;					// Get new servers from clients
		bool		MagnetSearch;							// Search for magnets over ed2k (lower server load)
		int			MaxClients;								// Max ed2k client links
		int			MaxResults;								// Stop searching after this many search results
		int			MaxShareCount;							// Hard limit on file list sent to server
		bool		MetAutoQuery;							// Auto query for a new server list
		int			MinServerFileSize;						// Minimum size a file in the library must be in order to be included in the server file list. (In KB)
		int			NumServers;								// 1
		int			PacketThrottle;							// ED2K packet rate limiter
		int			QueryFileThrottle;						// Max rate a file can have GetSources done
		int			QueryGlobalThrottle;					// Global throttle for all ed2k searches (TCP, UDP, manual and auto)
		int			QueueRankThrottle;						// How frequently queue ranks are sent
		int			QueryServerThrottle;					// Max rate at which an individual server can be queried
		int			ReAskTime;
		int			RequestPipe;
		int			RequestSize;
		bool		SearchCachedServers;					// Search on known remote servers in the server cache
		bool		SendPortServer;							// Send port in tag to ed2k servers. (not needed for newer servers)
		QString		ServerListURL;							// URL address to auto query for a new server list
		bool		ServerWalk;								// Enable global UDP walk of servers
		int			SourceThrottle;							// ED2K source rate limiter
		int			StatsGlobalThrottle;					// Global throttle for server UDP stats requests
		int			StatsServerThrottle;					// Max rate at which an individual server can be asked for stats
	} eDonkey2k;

	struct sBittorrent
	{
		bool		AutoClear;								// Clear completed torrents when they meet the required share ratio
		bool		AutoSeed;								// Automatically re-seed most recently completed torrent on start-up
		int			BandwidthPercentage;					// Percentage of bandwidth to use when BT active.
		int			ClearRatio;								// Share ratio a torrent must reach to be cleared. (Minimum 100%)
		int			CodePage;								// The code page to assume for a .torrent file if it isn't UTF-8
		QString		DefaultTracker;							// Default tracker to use if trackers listed in torrent are down
		int			DefaultTrackerPeriod;					// Delay between tracker contact attempts if one is not specified by tracker
		int			DhtPruneTime;
		int			DownloadConnections;					// Number of active torrent connections allowed
		int			DownloadTorrents;						// Number of torrents to download at once
		bool		Endgame;								// Allow endgame mode when completing torrents. (Download same chunk from multiple sources)
		bool		ExtraKeys;								// Check for '.utf8' keys if there is an encoding error
		int			LinkPing;
		int			LinkTimeout;
		bool		Managed;								// Managed Torrents
		bool		PreferBTSources;						// Prefer downloading from BT sources when appropriate
		int			RandomPeriod;
		int			RequestLimit;
		int			RequestPipe;
		int			RequestSize;
		bool		ShowFilesInDownload;					// Display Individual Files In The Download Tree
		int			SourceExchangePeriod;					// Time to exchange sources with new peers
		bool		StartPaused;							// Start torrents in a paused state
		bool		TestPartials;							// Do on integrity check of Bittorrent partials
		QString		TorrentPath;							// Where .torrent files are stored
		bool		TrackerKey;								// Send a key (random value) to trackers
		int			UploadCount;							// Number of active torrent uploads allowed
		bool		UseKademlia;							// Use Kademlia with bittorrent
		bool		UseSaveDialog;							// Use torrent save dialog or use default settings
		bool		UseTemp;								// Store incomplete torrent downloads in the temporary folder
	} Bittorrent;

	struct sDiscovery
	{
		int			AccessThrottle;
		int			BootstrapCount;
		int			CacheCount;								// Limit ability to learn new caches
		int			DefaultUpdate;
		bool		EnableG1GWC;
		int			FailureLimit;
		int			Lowpoint;
		int			UpdatePeriod;
	} Discovery;

	struct sScheduler
	{

	} Scheduler;
};

extern PersistentSettings Settings;

#endif // PERSISTENTSETTINGS_H
