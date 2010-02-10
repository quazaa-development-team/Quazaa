/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
***************************************************************************/

#ifndef MEDIALAYER_H
#define MEDIAPLAYER_H

#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtCore/QTimerEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QIcon>
#include <QTableWidgetItem>

#include <Phonon/AudioOutput>
#include <Phonon/BackendCapabilities>
#include <Phonon/Effect>
#include <Phonon/EffectParameter>
#include <Phonon/EffectWidget>
#include <Phonon/MediaObject>
#include <Phonon/SeekSlider>
#include <Phonon/VideoWidget>
#include <Phonon/VolumeSlider>
#include <QList>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QSlider;
class QTextEdit;
class QMenu;
class Ui_mediaSettings;
QT_END_NAMESPACE

class MediaPlayer :
			public QWidget
{
	Q_OBJECT
public:
	MediaPlayer(QWidget *parent = 0);
	void dragEnterEvent(QDragEnterEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *e);
	void handleDrop(QDropEvent *e);
	void setFile(const QString &text);
	void initSettingsDialog();
	QMenu *fileMenu;
	QList<Phonon::MediaSource> sources;
	int playlistRowCount;
	bool playlistSelectedItemsIsEmpty;
	int playlistCurrentRow;
	bool playlistProcessing;

signals:
	void playButtonEnableChanged(bool enabled);
	void playStatusChanged(QIcon icon, QString text);
	void stopButtonEnableChanged(bool enabled);
	void rewindButtonEnableChanged(bool enabled);
	void forwardButtonEnableChanged(bool enabled);
	void fullscreenEnableChanged(bool enabled);
	void volumeEnableChanged(bool enabled);
	void muteStatusChanged(QIcon icon, QString text);
	void playlistClear();
	void progressTextChanged(QString progress);
	void totalTimeTextChanged(QString totalTime);
	void playlistRowChanged(int row);
	void playlistRowCountUpdate();
	void playlistInsertRow(int row);
	void playlistRemoveRow(int row);
	void setPlaylistItem(int row, int column, QTableWidgetItem *item);
	void playlistSelectedItemsIsEmptyUpdate();
	void playlistResizeColumnsToContents();
	void setPlaylistStretchLastSection(bool stretch);
	void playlistCurrentRowUpdate();

public slots:
	void openFiles(QStringList files = QStringList(), bool requireFileList = false, bool addToList = false);
	void addFiles(QStringList files = QStringList(), bool requireFileList = false);
	void rewind();
	void forward();
	void updateInfo();
	void playPause();
	void playOnSwitch(bool play, Phonon::MediaSource source);
	void stop();
	void scaleChanged(QAction *);
	void aspectChanged(QAction *);
	void setVideoWindowFullscreen();
	void mediaSourceFinished();
	void setPlaylistRowCount(int rowCount) { playlistRowCount = rowCount; }
	void setPlaylistSelectedItemsIsEmpty(bool isEmpty) { playlistSelectedItemsIsEmpty = isEmpty; }
	void playlistDoubleClicked(QModelIndex index);
	void setPlaylistCurrentRow(int row) { playlistCurrentRow = row; }
	void onPlaylistClear();
	void onPlaylistRemove();
	void savePlaylist();
	void openPlaylist(bool addToPlaylist = false);
	void addPlaylist();
	void opaquePaint(QPainter *painter, QPaintEvent *event);

private slots:
	void setAspect(int);
	void setScale(int);
	void setSaturation(int);
	void setContrast(int);
	void setHue(int);
	void setBrightness(int);
	void stateChanged(Phonon::State newstate, Phonon::State oldstate);
	void effectChanged();
	void showSettingsDialog();
	void showContextMenu(const QPoint &);
	void configureEffect();
	void toggleVideo(bool);
	void bufferStatus(int percent);
	void updateTotalTime();
	void sourceChanged(const Phonon::MediaSource &source);
	void metaStateChanged(Phonon::State newState, Phonon::State oldState);
	void toggleMute();
	void muteChanged(bool muted);
	void volumeChanged(qreal volume);
	void paintEvent(QPaintEvent *event);

private:
	QSlider *m_hueSlider;
	QSlider *m_satSlider;
	QSlider *m_contSlider;
	QLabel *info;
	Phonon::Effect *nextEffect;
	QDialog *settingsDialog;
	Ui_mediaSettings *ui;
	QWidget m_videoWindow;

public:
	Phonon::MediaObject m_MediaObject;
	Phonon::MediaObject metaInformationResolver;
	Phonon::AudioOutput m_AudioOutput;

private:
	Phonon::VideoWidget *m_videoWidget;
	Phonon::Path m_audioOutputPath;
};

#endif //MEDIAPLAYER_H
