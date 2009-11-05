//
// quazaaglobals.cpp
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

#include <QApplication>
#include "quazaaglobals.h"

QuazaaGlobals Globals;

QuazaaGlobals::QuazaaGlobals()
{

}

void QuazaaGlobals::InitializeGlobals()
{
	//Application Globals
	Application.Name				= "Quazaa";
	Application.Version				= "0,0,1,1";
	Application.VersionString		= "0.0.1.1a (rev.384 2009-11-01)";
	Application.Fingerprint			= "QZ,0,0,1,1";
	Application.OrganizationName	= "Quazaa Development Team";
	Application.OrganizationDomain	= "http://quazaa.sourceforge.net";
	Application.BypassCloseEvent	= false;
	//Bittorrent Globals
	Bittorrent.ExtBegin					= 41000;
	Bittorrent.EventFinished			= Bittorrent.ExtBegin + 1;
	Bittorrent.PeerBanAlert				= Bittorrent.ExtBegin + 2;
	Bittorrent.HashFailAlert			= Bittorrent.ExtBegin + 3;
	Bittorrent.URLSeedAlert				= Bittorrent.ExtBegin + 5;
	Bittorrent.TrackerWarningAlert		= Bittorrent.ExtBegin + 4;
	Bittorrent.TrackerAnnounceAlert		= Bittorrent.ExtBegin + 6;
	Bittorrent.TrackerAlert				= Bittorrent.ExtBegin + 7;
	Bittorrent.TrackerReplyAlert		= Bittorrent.ExtBegin + 8;
	Bittorrent.EventPaused				= Bittorrent.ExtBegin + 9;
	Bittorrent.FastResumeAlert			= Bittorrent.ExtBegin + 10;
	Bittorrent.PieceFinishedAlert		= Bittorrent.ExtBegin + 11;
	Bittorrent.BlockFinishedAlert		= Bittorrent.ExtBegin + 12;
	Bittorrent.BlockDownloadingAlert	= Bittorrent.ExtBegin + 13;
	Bittorrent.ListenSucceededAlert		= Bittorrent.ExtBegin + 14;
	Bittorrent.ListenFailedAlert		= Bittorrent.ExtBegin + 15;
	Bittorrent.IPFilterAlert			= Bittorrent.ExtBegin + 16;
	Bittorrent.IncorrectEncodingLevel	= Bittorrent.ExtBegin + 17;
	Bittorrent.IncorrectConnectPolicy	= Bittorrent.ExtBegin + 18;
	Bittorrent.PeerAlert				= Bittorrent.ExtBegin + 19;
	Bittorrent.ListenV6FailedAlert		= Bittorrent.ExtBegin + 20;
	Bittorrent.LoadFilters				= Bittorrent.ExtBegin + 21;
	Bittorrent.ExternalIPAlert			= Bittorrent.ExtBegin + 22;
	Bittorrent.PortmapErrorAlert		= Bittorrent.ExtBegin + 23;
	Bittorrent.PortmapAlert				= Bittorrent.ExtBegin + 24;
	Bittorrent.PortmapTypePMP			= Bittorrent.ExtBegin + 25;
	Bittorrent.PortmapTypeUPNP			= Bittorrent.ExtBegin + 26;
	Bittorrent.FileErrorAlert			= Bittorrent.ExtBegin + 27;
	Bittorrent.DHTReplyAlert			= Bittorrent.ExtBegin + 28;
	Bittorrent.WriteResumeAlert			= Bittorrent.ExtBegin + 29;
	Bittorrent.WriteResumeFailAlert		= Bittorrent.ExtBegin + 30;
	Bittorrent.ResumeAlert				= Bittorrent.ExtBegin + 31;
	Bittorrent.ErrorPauseAlert			= Bittorrent.ExtBegin + 32;

	Bittorrent.IntBegin					= 42000;
	Bittorrent.PeerInteresting			= Bittorrent.IntBegin + 1;
	Bittorrent.PeerChoked				= Bittorrent.IntBegin + 2;
	Bittorrent.PeerRemoteInteresting	= Bittorrent.IntBegin + 3;
	Bittorrent.PeerRemoteChoked			= Bittorrent.IntBegin + 4;
	Bittorrent.PeerSupportExtensions	= Bittorrent.IntBegin + 5;
	Bittorrent.PeerLocalConnection		= Bittorrent.IntBegin + 6;
	Bittorrent.PeerHandshake			= Bittorrent.IntBegin + 7;
	Bittorrent.PeerConnecting			= Bittorrent.IntBegin + 8;
	Bittorrent.PeerQueued				= Bittorrent.IntBegin + 9;
	Bittorrent.PeerRC4Encrypted			= Bittorrent.IntBegin + 10;
	Bittorrent.PeerPlaintextEncrypted	= Bittorrent.IntBegin + 11;
	Bittorrent.QueuedChecking			= Bittorrent.IntBegin + 12;
	Bittorrent.CheckingFiles			= Bittorrent.IntBegin + 13;
	Bittorrent.Connecting				= Bittorrent.IntBegin + 14;
	Bittorrent.Downloading				= Bittorrent.IntBegin + 15;
	Bittorrent.Finished					= Bittorrent.IntBegin + 16;
	Bittorrent.Seeding					= Bittorrent.IntBegin + 17;
	Bittorrent.Allocating				= Bittorrent.IntBegin + 18;
	Bittorrent.Queued					= Bittorrent.IntBegin + 19;
	Bittorrent.Stopped					= Bittorrent.IntBegin + 20;
	Bittorrent.Paused					= Bittorrent.IntBegin + 21;
	Bittorrent.Stopping					= Bittorrent.IntBegin + 22;
	Bittorrent.Pausing					= Bittorrent.IntBegin + 23;
	Bittorrent.Metadata					= Bittorrent.IntBegin + 24;
	Bittorrent.NewtCreating				= Bittorrent.IntBegin + 25;
	Bittorrent.NewtHashingPieces		= Bittorrent.IntBegin + 26;
	Bittorrent.ImportFilters			= Bittorrent.IntBegin + 27;
	Bittorrent.IntNewtAddPeersWeb		= Bittorrent.IntBegin + 28;
	Bittorrent.IntNewtAddPeersDHT		= Bittorrent.IntBegin + 29;
	Bittorrent.NewtCreationCanceled		= Bittorrent.IntBegin + 30;
	Bittorrent.InError					= Bittorrent.IntBegin + 31;
	Folders.LanguageDirectory			= QApplication::applicationDirPath() + "/Language";
}
