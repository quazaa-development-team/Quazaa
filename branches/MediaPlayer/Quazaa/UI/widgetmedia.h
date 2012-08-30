//
// widgetmedia.h
//
// Copyright © Quazaa Development Team, 2009-2012.
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

#ifndef WIDGETMEDIA_H
#define WIDGETMEDIA_H

#include <QMainWindow>
#include <QSlider>
#include <QComboBox>
#include <QLabel>
#include "widgetvideo.h"
#include "playlistmodel.h"

#include <qmediaplayer.h>
#include <qmediaplaylist.h>

QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QLabel;
class QModelIndex;
class QSlider;
class QPushButton;

class QMediaPlayer;
class QVideoWidget;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class PlaylistModel;

namespace Ui
{
	class WidgetMedia;
}

class WidgetMedia : public QMainWindow
{
	Q_OBJECT
public:
	WidgetMedia(QWidget* parent = 0);
	~WidgetMedia();
	void saveWidget();

	QMediaPlayer::State state() const;

	int volume() const;
	bool isMuted() const;
	qreal playbackRate() const;

public slots:
	void setState(QMediaPlayer::State state);
	void setVolume(int volume);
	void setMuted(bool muted);
	void setPlaybackRate(float rate);

signals:
	void fullScreenChanged(bool fullScreen);
	void play();
	void pause();
	void changeVolume(int volume);
	void changeMuting(bool muting);
	void changeRate(qreal rate);

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetMedia* ui;
	QMediaPlayer *player;
	QMediaPlaylist *playlist;
	WidgetVideo *videoWidget;
	QLabel *coverLabel;
	QLabel *labelTime;
#ifndef PLAYER_NO_COLOROPTIONS
	QDialog *colorDialog;
#endif
	PlaylistModel *playlistModel;
	QString trackInfo;
	QString statusInfo;
	QMediaPlayer::State playerState;
	bool playerMuted;
	QComboBox *rateBox;
	QSlider* seekSlider;
	QSlider* volumeSlider;
	QString mediaDuration;
	QString mediaPosition;

private slots:
	void on_toolButtonMediaPlaylistTaskHeader_clicked();
	void on_splitterMedia_customContextMenuRequested(QPoint pos);
	void on_actionMediaShuffle_triggered(bool checked);
	void on_actionMediaRepeat_triggered(bool checked);
	void setTrackInfo(const QString &info);
	void setStatusInfo(const QString &info);
	void handleCursor(QMediaPlayer::MediaStatus status);
	void open();
	void durationChanged(qint64 duration);
	void positionChanged(qint64 progress);
	void metaDataChanged();

	void playClicked();
	void stopClicked();
	void updateRate();
	void previousClicked();

	void seek(int seconds);
	void jump(const QModelIndex &index);
	void playlistPositionChanged(int);

	void statusChanged(QMediaPlayer::MediaStatus status);
	void bufferingProgress(int progress);
	void videoAvailableChanged(bool available);

	void displayErrorMessage();

#ifndef PLAYER_NO_COLOROPTIONS
	void showColorDialog();
#endif
	void addToPlaylist(const QStringList& fileNames);
	void setSkin();
	void setControlsEnabled(bool enabled);
	void showFullScreen();
	void toggleVideo(bool visible);
};

#endif // WIDGETMEDIA_H
