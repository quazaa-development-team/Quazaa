#ifndef QUAZAAMEDIAPLAYER_H
#define QUAZAAMEDIAPLAYER_H

#include <QtGui>
#include <QtMultimediaKit/QMediaPlayer>
#include <QtMultimediaKit/QMediaPlaylist>
#include <playlistmodel.h>
#include <QtMultimediaKit/QVideoWidget>

class QuazaaMediaPlayer : public QObject
{
	Q_OBJECT

public:
	explicit QuazaaMediaPlayer(QHBoxLayout *videoWidgetLayout, QObject *parent = 0);
	QMediaPlayer *m_oMediaPlayer;
	QMediaPlaylist *mediaPlaylist;
	PlaylistModel *mediaPlaylistModel;
	QVideoWidget *videoWidget;

signals:

public slots:

};

#endif // QUAZAAMEDIAPLAYER_H
