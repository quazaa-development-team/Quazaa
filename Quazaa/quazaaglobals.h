//
// quazaaglobals.h
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

#ifndef QUAZAAGLOBALS_H
#define QUAZAAGLOBALS_H

#include <QObject>
#include <QDir>

class QuazaaGlobals : public QObject
{
public:
	QuazaaGlobals();
	void InitializeGlobals();

	//Attriutes
public:
	//Application Globals
	struct sApplication
	{
		QString Name;
		QString Version;
		QString VersionString;
		QString Fingerprint;
		QString OrganizationName;
		QString OrganizationDomain;
		bool BypassCloseEvent;
	} Application;

	struct sBittorrent
	{
		int ExtBegin;
		int EventFinished;
		int PeerBanAlert;
		int HashFailAlert;
		int URLSeedAlert;
		int TrackerWarningAlert;
		int TrackerAnnounceAlert;
		int TrackerAlert;
		int TrackerReplyAlert;
		int EventPaused;
		int FastResumeAlert;
		int PieceFinishedAlert;
		int BlockFinishedAlert;
		int BlockDownloadingAlert;
		int ListenSucceededAlert;
		int ListenFailedAlert;
		int IPFilterAlert;
		int IncorrectEncodingLevel;
		int IncorrectConnectPolicy;
		int PeerAlert;
		int ListenV6FailedAlert;
		int LoadFilters;
		int ExternalIPAlert;
		int PortmapErrorAlert;
		int PortmapAlert;
		int PortmapTypePMP;
		int PortmapTypeUPNP;
		int FileErrorAlert;
		int DHTReplyAlert;
		int WriteResumeAlert;
		int WriteResumeFailAlert;
		int ResumeAlert;
		int ErrorPauseAlert;
		int IntBegin;
		int PeerInteresting;
		int PeerChoked;
		int PeerRemoteInteresting;
		int PeerRemoteChoked;
		int PeerSupportExtensions;
		int PeerLocalConnection;
		int PeerHandshake;
		int PeerConnecting;
		int PeerQueued;
		int PeerRC4Encrypted;
		int PeerPlaintextEncrypted;
		int QueuedChecking;
		int CheckingFiles;
		int Connecting;
		int Downloading;
		int Finished	;
		int Seeding	;
		int Allocating;
		int Queued;
		int Stopped;
		int Paused;
		int Stopping;
		int Pausing;
		int Metadata;
		int NewtCreating;
		int NewtHashingPieces;
		int ImportFilters;
		int IntNewtAddPeersWeb;
		int IntNewtAddPeersDHT;
		int NewtCreationCanceled;
		int InError;
	} Bittorrent;

	struct sFolders
	{
		QString ApplicationDirectory;
		QString LanguageDirectory;
	} Folders;
};

extern QuazaaGlobals Globals;

#endif // QUAZAAGLOBALS_H
