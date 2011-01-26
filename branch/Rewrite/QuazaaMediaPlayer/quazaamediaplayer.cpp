#include "quazaamediaplayer.h"

QuazaaMediaPlayer::QuazaaMediaPlayer(QHBoxLayout *videoWidgetLayout, QObject *parent) :
	QObject(parent)
{
	m_oMediaPlayer = new QMediaPlayer();
	mediaPlaylist = new QMediaPlaylist();
	mediaPlaylistModel = new PlaylistModel(this);
	videoWidget = new QVideoWidget();
	m_oMediaPlayer->setPlaylist(mediaPlaylist);
	mediaPlaylistModel->setPlaylist(mediaPlaylist);
	m_oMediaPlayer->setVideoOutput(videoWidget);
	videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	videoWidgetLayout->addWidget(videoWidget);

}
