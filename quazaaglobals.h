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

class QuazaaGlobals : public QObject
{
	Q_OBJECT

public:
	QuazaaGlobals() {}

public:
	QString ApplicationName() const { return "Quazaa"; }
	QString ApplicationVersion() const { return "0,0,1,1"; }
	QString ApplicationVersionString() const { return "0.0.1.1a (rev.384 2009-11-01)"; }
	QString ApplicationFingerprint() const { return "QZ,0,0,1,1"; }
	QString ApplicationOrganizationName() const { return "Quazaa Development Team"; }
	QString ApplicationOrganizationDomain() const { return "http://quazaa.sourceforge.net"; }
	int BitTorrentExtBegin() const { return 41000; }
	int BitTorrentEventFinished() const { return QuazaaGlobals::BitTorrentExtBegin() + 1; }
	int BitTorrentPeerBanAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 2; }
	int BitTorrentHashFailAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 3; }
	int BitTorrentURLSeedAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 5; }
	int BitTorrentTrackerWarningAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 4; }
	int BitTorrentTrackerAnnounceAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 6; }
	int BitTorrentTrackerAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 7; }
	int BitTorrentTrackerReplyAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 8; }
	int BitTorrentEventPaused	() const { return QuazaaGlobals::BitTorrentExtBegin() + 9; }
	int BitTorrentFastResumeAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 10; }
	int BitTorrentPieceFinishedAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 11; }
	int BitTorrentBlockFinishedAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 12; }
	int BitTorrentBlockDownloadingAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 13; }
	int BitTorrentListenSucceededAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 14; }
	int BitTorrentListenFailedAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 15; }
	int BitTorrentIPFilterAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 16; }
	int BitTorrentIncorrectEncodingLevel() const { return QuazaaGlobals::BitTorrentExtBegin() + 17; }
	int BitTorrentIncorrectConnectPolicy() const { return QuazaaGlobals::BitTorrentExtBegin() + 18; }
	int BitTorrentPeerAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 19; }
	int BitTorrentListenV6FailedAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 20; }
	int BitTorrentLoadFilters() const { return QuazaaGlobals::BitTorrentExtBegin() + 21; }
	int BitTorrentExternalIPAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 22; }
	int BitTorrentPortmapErrorAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 23; }
	int BitTorrentPortmapAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 24; }
	int BitTorrentPortmapTypePMP() const { return QuazaaGlobals::BitTorrentExtBegin() + 25; }
	int BitTorrentPortmapTypeUPNP() const { return QuazaaGlobals::BitTorrentExtBegin() + 26; }
	int BitTorrentFileErrorAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 27; }
	int BitTorrentDHTReplyAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 28; }
	int BitTorrentWriteResumeAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 29; }
	int BitTorrentWriteResumeFailAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 30; }
	int BitTorrentResumeAlert	() const { return QuazaaGlobals::BitTorrentExtBegin() + 31; }
	int BitTorrentErrorPauseAlert() const { return QuazaaGlobals::BitTorrentExtBegin() + 32; }
	int BitTorrentIntBegin() const { return 42000; }
	int BitTorrentPeerInteresting() const { return QuazaaGlobals::BitTorrentIntBegin() + 1; }
	int BitTorrentPeerChoked() const { return QuazaaGlobals::BitTorrentIntBegin() + 2; }
	int BitTorrentPeerRemoteInteresting() const { return QuazaaGlobals::BitTorrentIntBegin() + 3; }
	int BitTorrentPeerRemoteChoked() const { return QuazaaGlobals::BitTorrentIntBegin() + 4; }
	int BitTorrentPeerSupportExtensions() const { return QuazaaGlobals::BitTorrentIntBegin() + 5; }
	int BitTorrentPeerLocalConnection() const { return QuazaaGlobals::BitTorrentIntBegin() + 6; }
	int BitTorrentPeerHandshake() const { return QuazaaGlobals::BitTorrentIntBegin() + 7; }
	int BitTorrentPeerConnecting() const { return QuazaaGlobals::BitTorrentIntBegin() + 8; }
	int BitTorrentPeerQueued() const { return QuazaaGlobals::BitTorrentIntBegin() + 9; }
	int BitTorrentPeerRC4Encrypted() const { return QuazaaGlobals::BitTorrentIntBegin() + 10; }
	int BitTorrentPeerPlaintextEncrypted() const { return QuazaaGlobals::BitTorrentIntBegin() + 11; }
	int BitTorrentQueuedChecking() const { return QuazaaGlobals::BitTorrentIntBegin() + 12; }
	int BitTorrentCheckingFiles() const { return QuazaaGlobals::BitTorrentIntBegin() + 13; }
	int BitTorrentConnecting() const { return QuazaaGlobals::BitTorrentIntBegin() + 14; }
	int BitTorrentDownloading() const { return QuazaaGlobals::BitTorrentIntBegin() + 15; }
	int BitTorrentFinished() const { return QuazaaGlobals::BitTorrentIntBegin() + 16; }
	int BitTorrentSeeding() const { return QuazaaGlobals::BitTorrentIntBegin() + 17; }
	int BitTorrentAllocating() const { return QuazaaGlobals::BitTorrentIntBegin() + 18; }
	int BitTorrentQueued() const { return QuazaaGlobals::BitTorrentIntBegin() + 19; }
	int BitTorrentStopped() const { return QuazaaGlobals::BitTorrentIntBegin() + 20; }
	int BitTorrentPaused() const { return QuazaaGlobals::BitTorrentIntBegin() + 21; }
	int BitTorrentStopping() const { return QuazaaGlobals::BitTorrentIntBegin() + 22; }
	int BitTorrentPausing() const { return QuazaaGlobals::BitTorrentIntBegin() + 23; }
	int BitTorrentMetadata() const { return QuazaaGlobals::BitTorrentIntBegin() + 24; }
	int BitTorrentNewtCreating() const { return QuazaaGlobals::BitTorrentIntBegin() + 25; }
	int BitTorrentNewtHashingPieces() const { return QuazaaGlobals::BitTorrentIntBegin() + 26; }
	int BitTorrentImportFilters() const { return QuazaaGlobals::BitTorrentIntBegin() + 27; }
	int BitTorrentIntNewtAddPeersWeb() const { return QuazaaGlobals::BitTorrentIntBegin() + 28; }
	int BitTorrentIntNewtAddPeersDHT() const { return QuazaaGlobals::BitTorrentIntBegin() + 29; }
	int BitTorrentNewtCreationCanceled() const { return QuazaaGlobals::BitTorrentIntBegin() + 30; }
	int BitTorrentInError() const { return QuazaaGlobals::BitTorrentIntBegin() + 31; }
	QString MediaOpenFilter() const { return tr("All Media Files ") + "(*.3g2 *.3gp *.3gp2 *.3gpp *.d2v *.divx *.amr *.amv *.asf *.avi *.bik *.drc *.dsa *.dsm *.dss *.dsv *.evo *.flc *.fli *.flic *.flv *.hdmov *.ifo *.ivf *.m1v *.m2p *.m2t *.m2ts *.m2v *.m4v *.mkv *.mov *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpg *.mpv2 *.mpv4 *.mts *.ogm *.ogv *.pss *.pva *.ram *.ratdvd *.rm *.rmm *.roq *.rp *.rmvb *.rpm *.rt *.smi *.smil *.smk *.swf *.tp *.tpr *.ts *.tta *.vob *.vp6 *.wm *.wmp *.wmv *.aac *.ac3 *.aif *.aifc *.aiff *.alac *.au *.cda *.dts *.flac *.mid *.midi *.m1a *.m2a *.m4a *.m4b *.mka *.mpa *.mpc *.mp2 *.mp3 *.oga *.ogg *.ra *.rmi *.snd *.wav *.wma);;" + tr("All Files ") + "(*.*);;" + tr("Video Files ") + "(*.3g2 *.3gp *.3gp2 *.3gpp *.d2v *.divx *.amr *.amv *.asf *.avi *.bik *.drc *.dsa *.dsm *.dss *.dsv *.evo *.flc *.fli *.flic *.flv *.hdmov *.ifo *.ivf *.m1v *.m2p *.m2t *.m2ts *.m2v *.m4v *.mkv *.mov *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpg *.mpv2 *.mpv4 *.mts *.ogm *.ogv *.pss *.pva *.ram *.ratdvd *.rm *.rmm *.roq *.rp *.rmvb *.rpm *.rt *.smi *.smil *.smk *.swf *.tp *.tpr *.ts *.tta *.vob *.vp6 *.wm *.wmp *.wmv);;Audio Files (*.aac *.ac3 *.aif *.aifc *.aiff *.alac *.au *.cda *.dts *.flac *.mid *.midi *.m1a *.m2a *.m4a *.m4b *.mka *.mpa *.mpc *.mp2 *.mp3 *.oga *.ogg *.ra *.rmi *.snd *.wav *.wma)"; }
};

extern QuazaaGlobals quazaaGlobals;

#endif // QUAZAAGLOBALS_H
