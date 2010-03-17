#include "vlcmediaplayer.h"
#include "quazaaSettings.h"
#include "quazaaglobals.h"
#include <QFileDialog>

vlcMediaPlayer::vlcMediaPlayer(QSlider *positionSlider, QSlider *volumeSlider, QFrame *videoWidget,
							   QTableWidget *tablePlaylist, QAction *playButton, QAction *stopButton,
							   QAction *previousButton, QAction *nextButton, QAction *openButton,
							   QAction *repeatButton, QAction *shuffleButton, QAction *fullScreenButton,
							   QAction *settingsButton, QAction *muteButton, QToolButton *addToPlaylistButton,
							   QToolButton *removeFromPlaylistButton, QToolButton *clearPlaylistButton,
							   QToolButton *loadPlaylistButton, QToolButton *addPlaylistButton,
							   QToolButton *savePlaylistButton, QWidget *parent)
	:QWidget(parent)
{
	//preparation of the vlc command
	const char * const vlc_args[] = {
		"-I", "dummy", // Don't use any interface
#if defined(Q_OS_UNIX)
		"--plugin-path=./vlcplugins",
#elif defined(Q_OS_WIN)
		"--plugin-path=.\\vlcplugins",
#endif
#if defined(QT_DEBUG)
		"--verbose=2", //be much more verbose then normal for debugging purpose
		"--extraintf=logger", //log anything
#endif
		"--ignore-config", // Don't use VLC's config
		"--reset-plugins-cache",
		"--no-media-library",
		"--no-one-instance",
		"--no-osd",
		"--no-stats",
		"--no-video-title-show"
	};

	m_videoWidget = videoWidget;
	m_videoWidget->setAutoFillBackground(true);

	m_tablePlaylist=tablePlaylist;

	m_volumeSlider=volumeSlider;
	m_volumeSlider->setMaximum(100); //the volume is between 0 and 100
	m_volumeSlider->setToolTip("Audio Slider");

	// Note: if you use streaming, there is no ability to use the position slider
	m_positionSlider=positionSlider;
	m_positionSlider->setMaximum(POSITION_RESOLUTION);

	m_playButton=playButton;
	m_stopButton=stopButton;
	m_previousButton=previousButton;
	m_nextButton=nextButton;
	m_openButton=openButton;
	m_repeatButton=repeatButton;
	m_shuffleButton=shuffleButton;
	m_fullScreenButton=fullScreenButton;
	m_settingsButton=settingsButton;
	m_muteButton=muteButton;
	m_addToPlaylistButton=addToPlaylistButton;
	m_removeFromPlaylistButton=removeFromPlaylistButton;
	m_clearPlaylistButton=clearPlaylistButton;
	m_loadPlaylistButton=loadPlaylistButton;
	m_addPlaylistButton=addPlaylistButton;
	m_savePlaylistButton=savePlaylistButton;

	m_bIsPlaying=false;
	poller=new QTimer(this);

	//Initialize an instance of vlc
	//a structure for the exception is neede for this initalization
	libvlc_exception_init(&vlcException);

	//create a new libvlc instance
	m_vlcInstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args,&vlcException);  //tricky calculation of the char space used
	raise (&vlcException);

	// Create a media player playing environement
	m_mediaPlayer = libvlc_media_player_new (m_vlcInstance, &vlcException);
	raise (&vlcException);

	//connect the two sliders to the corresponding slots (uses Qt's signal / slots technology)
	connect(poller, SIGNAL(timeout()), this, SLOT(updateInterface()));
	connect(m_positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(changePosition(int)));
	connect(m_volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(changeVolume(int)));
	connect(this, SIGNAL(playStateChanged(bool)), m_playButton, SLOT(setChecked(bool)));
	connect(this, SIGNAL(playEnabled(bool)), m_playButton, SLOT(setEnabled(bool)));
	emit playEnabled(m_bIsPlaying);
	emit playStateChanged(m_bIsPlaying);

	poller->start(100); //start timer to trigger every 100 ms the updateInterface slot
}


//desctructor
vlcMediaPlayer::~vlcMediaPlayer()
{
	/* Stop playing */
	libvlc_media_player_stop (m_mediaPlayer, &vlcException);

	/* Free the media_player */
	libvlc_media_player_release (m_mediaPlayer);

	libvlc_release (m_vlcInstance);
	raise (&vlcException);
}

void vlcMediaPlayer::playFile(QString file)
{
	//the file has to be in one of the following formats /perhaps a little bit outdated)
	/*
	[file://]filename              Plain media file
	http://ip:port/file            HTTP URL
	ftp://ip:port/file             FTP URL
	mms://ip:port/file             MMS URL
	screen://                      Screen capture
	[dvd://][device][@raw_device]  DVD device
	[vcd://][device]               VCD device
	[cdda://][device]              Audio CD device
	udp:[[<source address>]@[<bind address>][:<bind port>]]
	*/

	/* Create a new LibVLC media descriptor */
	m_mediaDescriptor = libvlc_media_new (m_vlcInstance, file.toAscii(), &vlcException);
	raise(&vlcException);

	libvlc_media_player_set_media (m_mediaPlayer, m_mediaDescriptor, &vlcException);
	raise(&vlcException);

	/* /!\ Please note /!\
	//
	// passing the widget to the lib shows vlc at which position it should show up
	// vlc automatically resizes the video to the ´given size of the widget
	// and it even resizes it, if the size changes at the playing

	// Get our media instance to use our window */
	#if defined(Q_OS_WIN)
		libvlc_media_player_set_drawable(m_mediaPlayer, reinterpret_cast<unsigned int>(m_videoWidget->winId()), &vlcException );
		//libvlc_media_player_set_hwnd(m_mediaPlayer, m_videoWidget->winId(), &vlcException ); // for vlc 1.0
	#elif defined(Q_OS_MAC)
		libvlc_media_player_set_drawable(m_mediaPlayer, m_videoWidget->winId(), &vlcException );
		//libvlc_media_player_set_agl (m_mediaPlayer, m_videoWidget->winId(), &vlcException); // for vlc 1.0
	#else //Linux
		libvlc_media_player_set_drawable(m_mediaPlayer, m_videoWidget->winId(), &vlcException );
		//libvlc_media_player_set_xwindow(m_mediaPlayer, m_videoWidget->winId(), &vlcException ); // for vlc 1.0
	#endif
	raise(&vlcException);

	/* Play */
	libvlc_media_player_play (m_mediaPlayer, &vlcException );
	raise(&vlcException);
	changeVolume(quazaaSettings.MediaVolume);

	m_bIsPlaying=true;
	emit playStateChanged(m_bIsPlaying);
}

void vlcMediaPlayer::stop()
{
	/* Stop playing */
	libvlc_media_player_stop (m_mediaPlayer, &vlcException);
	emit playEnabled(false);
}

void vlcMediaPlayer::changeVolume(int newVolume)
{
	libvlc_exception_clear(&vlcException);
	libvlc_audio_set_volume (m_vlcInstance,newVolume , &vlcException);
	raise(&vlcException);
	quazaaSettings.MediaVolume = newVolume;
}

void vlcMediaPlayer::changePosition(int newPosition)
{
	libvlc_exception_clear(&vlcException);
	// It's possible that the vlc doesn't play anything
	// so check before
	libvlc_media_t *curMedia = libvlc_media_player_get_media (m_mediaPlayer, &vlcException);
	libvlc_exception_clear(&vlcException);
	if (curMedia == NULL)
		return;

	float pos=(float)(newPosition)/(float)POSITION_RESOLUTION;
	libvlc_media_player_set_position (m_mediaPlayer, pos, &vlcException);
	raise(&vlcException);
}

void vlcMediaPlayer::updateInterface()
{
	if(!m_bIsPlaying)
		return;

	// It's possible that the vlc doesn't play anything
	// so check before
	libvlc_media_t *curMedia = libvlc_media_player_get_media (m_mediaPlayer, &vlcException);
	libvlc_exception_clear(&vlcException);
	if (curMedia == NULL)
		return;

	float pos=libvlc_media_player_get_position (m_mediaPlayer, &vlcException);
	int siderPos=(int)(pos*(float)(POSITION_RESOLUTION));
	m_positionSlider->setValue(siderPos);
	int volume=libvlc_audio_get_volume (m_vlcInstance,&vlcException);
	m_volumeSlider->setValue(volume);
}

void vlcMediaPlayer::raise(libvlc_exception_t * ex)
{
	if (libvlc_exception_raised (ex))
	{
		 fprintf (stderr, "error: %s\n", libvlc_exception_get_message(ex));
		 exit (-1);
	}
}

void vlcMediaPlayer::openFile(QString file)
{
	if (file.isEmpty())
		file = QFileDialog::getOpenFileName(this, "Open Media File", quazaaSettings.MediaOpenPath, quazaaGlobals.MediaOpenFilter());
	if (!file.isEmpty())
	{
		if (m_bIsPlaying)
		{
			stop();
		}
		QFileInfo *fileInfo = new QFileInfo();
		fileInfo->setFile(file);
		quazaaSettings.MediaOpenPath = fileInfo->canonicalPath();
		file = file.prepend("file:///");
		playFile(file);
		emit playEnabled(true);
	}
}
