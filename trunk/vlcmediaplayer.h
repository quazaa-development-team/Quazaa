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
