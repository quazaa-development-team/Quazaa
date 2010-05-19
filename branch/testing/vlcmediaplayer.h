//
// vlcmediaplayer.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
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

#ifndef VLCMEDIAPLAYER_H
#define VLCMEDIAPLAYER_H

#include <QtGui>
#include "vlc/vlc.h"

#define POSITION_RESOLUTION 10000

class vlcMediaPlayer : public QWidget
{
Q_OBJECT
public:
	vlcMediaPlayer(QSlider *positionSlider = new QSlider(), QSlider *volumeSlider = new QSlider(),
				   QFrame *videoWidget = new QFrame(), QTableWidget *tablePlaylist = new QTableWidget(),
				   QAction *playButton = new QAction(0), QAction *stopButton = new QAction(0),
				   QAction *previousButton = new QAction(0), QAction *nextButton = new QAction(0),
				   QAction *openButton = new QAction(0), QAction *repeatButton = new QAction(0),
				   QAction *shuffleButton = new QAction(0), QAction *fullScreenButton = new QAction(0),
				   QAction *settingsButton = new QAction(0), QAction *muteButton = new QAction(0),
				   QToolButton *addToPlaylistButton = new QToolButton(),
				   QToolButton *removeFromPlaylistButton = new QToolButton(),
				   QToolButton *clearPlaylistButton = new QToolButton(),
				   QToolButton *loadPlaylistButton = new QToolButton(),
				   QToolButton *addPlaylistButton = new QToolButton(),
				   QToolButton *savePlaylistButton = new QToolButton(), QWidget *parent = 0);
	~vlcMediaPlayer();
	void raise(libvlc_exception_t * ex);

	QSlider *m_positionSlider;
	QSlider *m_volumeSlider;
	QFrame *m_videoWidget;
	QTableWidget *m_tablePlaylist;
	QAction *m_playButton;
	QAction *m_stopButton;
	QAction *m_previousButton;
	QAction *m_nextButton;
	QAction *m_openButton;
	QAction *m_repeatButton;
	QAction *m_shuffleButton;
	QAction *m_fullScreenButton;
	QAction *m_settingsButton;
	QAction *m_muteButton;
	QToolButton *m_addToPlaylistButton;
	QToolButton *m_removeFromPlaylistButton;
	QToolButton *m_clearPlaylistButton;
	QToolButton *m_loadPlaylistButton;
	QToolButton *m_addPlaylistButton;
	QToolButton *m_savePlaylistButton;
	QTimer *poller;
	bool m_bIsPlaying;
	libvlc_exception_t vlcException;
	libvlc_instance_t *m_vlcInstance;
	libvlc_media_player_t *m_mediaPlayer;
	libvlc_media_t *m_mediaDescriptor ;

signals:
	void playEnabled(bool);
	void playStateChanged(bool);

public slots:
	void openFile(QString file = "");
	void playFile(QString file);
	void stop();
	void updateInterface();
	void changeVolume(int newVolume);
	void changePosition(int newPosition);
};

#endif // VLCMEDIAPLAYER_H
