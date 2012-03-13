/*
** $Id: widgetmedia.cpp 952 2012-03-12 03:15:15Z smokexyz $
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public 
** License version 3.0 requirements will be met: 
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version 
** 3.0 along with Quazaa; if not, write to the Free Software Foundation, 
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "widgetmedia.h"
#include "ui_widgetmedia.h"

#include "quazaasettings.h"
#include "skinsettings.h"
#include "commonfunctions.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QPushButton>

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetMedia::WidgetMedia(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetMedia)
  , videoWidget(0)
  , coverLabel(0)
  , seekSlider(0)
#ifndef PLAYER_NO_COLOROPTIONS
  , colorDialog(0)
#endif
  , playerState(QMediaPlayer::StoppedState)
  , playerMuted(false)
  , volumeSlider(0)
  , rateBox(0)
{
	ui->setupUi(this);

	//! [create-objs]
		player = new QMediaPlayer(this);
		// owned by PlaylistModel
		playlist = new QMediaPlaylist();
		player->setPlaylist(playlist);

		seekSlider = new QSlider(Qt::Horizontal, this);
		seekSlider->setRange(0, player->duration() / 1000);
		connect(seekSlider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));

		labelTime = new QLabel(this);
		labelTime->setMinimumWidth(100);

		volumeSlider = new QSlider(Qt::Horizontal, this);
		volumeSlider->setRange(0, 100);
		connect(volumeSlider, SIGNAL(sliderMoved(int)), player, SLOT(setVolume(int)));
		volumeSlider->setMaximumWidth(100);

		rateBox = new QComboBox(this);
		rateBox->addItem("0.5x", QVariant(0.5));
		rateBox->addItem("1.0x", QVariant(1.0));
		rateBox->addItem("2.0x", QVariant(2.0));
		rateBox->setCurrentIndex(1);

		connect(rateBox, SIGNAL(activated(int)), SLOT(updateRate()));
		connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
		connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
		connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
		connect(playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
		connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
				this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
		connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
		connect(player, SIGNAL(videoAvailableChanged(bool)), this, SLOT(videoAvailableChanged(bool)));
		connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));
	//! [create-objs]

	ui->toolBarPlayControls->addWidget(seekSlider);
	ui->toolBarPlayControls->addWidget(labelTime);
	ui->toolBarPlayControls->addWidget(rateBox);
	ui->toolBarPlayControls->addSeparator();
	ui->toolBarPlayControls->addAction(ui->actionMediaMute);
	ui->toolBarPlayControls->addWidget(volumeSlider);

	restoreState(quazaaSettings.WinMain.MediaToolbars);
	ui->splitterMedia->restoreState(quazaaSettings.WinMain.MediaSplitter);
	ui->actionMediaRepeat->setChecked(quazaaSettings.Media.Repeat);
	ui->actionMediaShuffle->setChecked(quazaaSettings.Media.Shuffle);

	setSkin();

	//! [2]
		videoWidget = new WidgetVideo(this);
		player->setVideoOutput(videoWidget);

		coverLabel = new QLabel();
		coverLabel->setAlignment(Qt::AlignBottom);
		coverLabel->setStyleSheet("background-color: black;0 color: white;");

		playlistModel = new PlaylistModel(this);
		playlistModel->setPlaylist(playlist);
	//! [2]

		ui->listViewMediaPlaylist->setModel(playlistModel);
		ui->listViewMediaPlaylist->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));

		connect(ui->listViewMediaPlaylist, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

		connect(ui->toolButtonMediaPlaylistAdd, SIGNAL(clicked()), this, SLOT(open()));

		setState(player->state());
		setVolume(player->volume());
		setMuted(isMuted());

		connect(ui->actionMediaPlay, SIGNAL(triggered()), this, SLOT(playClicked()));
		connect(this, SIGNAL(play()), player, SLOT(play()));
		connect(this, SIGNAL(pause()), player, SLOT(pause()));
		connect(ui->actionMediaStop, SIGNAL(triggered()), this, SLOT(stopClicked()));
		connect(ui->actionMediaNextTrack, SIGNAL(triggered()), playlist, SLOT(next()));
		connect(ui->actionMediaRewind, SIGNAL(triggered()), this, SLOT(previousClicked()));
		connect(ui->actionMediaMute, SIGNAL(toggled(bool)), player, SLOT(setMuted(bool)));
		connect(this, SIGNAL(changeRate(qreal)), player, SLOT(setPlaybackRate(qreal)));

		connect(ui->actionMediaStop, SIGNAL(triggered()), videoWidget, SLOT(update()));

		connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
				this, SLOT(setState(QMediaPlayer::State)));
		connect(player, SIGNAL(volumeChanged(int)), this, SLOT(setVolume(int)));
		connect(player, SIGNAL(mutedChanged(bool)), this, SLOT(setMuted(bool)));

		connect(ui->actionMediaFullscreen, SIGNAL(triggered()), this, SLOT(showFullScreen()));

	#ifndef PLAYER_NO_COLOROPTIONS
		connect(ui->actionMediaSettings, SIGNAL(triggered()), this, SLOT(showColorDialog()));
	#else
		ui->actionMediaSettings->setVisible(false);
	#endif

		ui->horizontalLayoutVideoWidget->addWidget(videoWidget, 2);
		ui->horizontalLayoutVideoWidget->addWidget(coverLabel, 2);
		toggleVideo(false);

		if (!player->isAvailable()) {
			QMessageBox::warning(this, tr("Service not available"),
								 tr("The QMediaPlayer object does not have a valid service.\n"\
									"Please check the media service plugins are installed."));

			setControlsEnabled(false);
			ui->listViewMediaPlaylist->setEnabled(false);
			ui->toolButtonMediaPlaylistAdd->setEnabled(false);
	#ifndef PLAYER_NO_COLOROPTIONS
			ui->actionMediaSettings->setEnabled(false);
	#endif
			ui->actionMediaFullscreen->setEnabled(false);
		}

		metaDataChanged();
}

WidgetMedia::~WidgetMedia()
{
	delete ui;
}

void WidgetMedia::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

QMediaPlayer::State WidgetMedia::state() const
{
	return playerState;
}

void WidgetMedia::saveWidget()
{
	quazaaSettings.WinMain.MediaToolbars = saveState();
	quazaaSettings.WinMain.MediaSplitter = ui->splitterMedia->saveState();
}

void WidgetMedia::on_actionMediaRepeat_triggered(bool checked)
{
	quazaaSettings.Media.Repeat = checked;
}

void WidgetMedia::on_actionMediaShuffle_triggered(bool checked)
{
	quazaaSettings.Media.Shuffle = checked;
}

void WidgetMedia::on_splitterMedia_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterMedia->handle(1)->underMouse())
	{
		if(ui->splitterMedia->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.MediaSplitterRestoreLeft = ui->splitterMedia->sizes()[0];
			quazaaSettings.WinMain.MediaSplitterRestoreRight = ui->splitterMedia->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterMedia->sizes()[0] + ui->splitterMedia->sizes()[1]);
			newSizes.append(0);
			ui->splitterMedia->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreRight);
			ui->splitterMedia->setSizes(sizesList);
		}
	}
}

void WidgetMedia::on_toolButtonMediaPlaylistTaskHeader_clicked()
{
	if(ui->splitterMedia->sizes()[0] > 0)
	{
		quazaaSettings.WinMain.MediaSplitterRestoreLeft = ui->splitterMedia->sizes()[0];
		quazaaSettings.WinMain.MediaSplitterRestoreRight = ui->splitterMedia->sizes()[1];
		QList<int> newSizes;
		newSizes.append(0);
		newSizes.append(ui->splitterMedia->sizes()[0] + ui->splitterMedia->sizes()[1]);
		ui->splitterMedia->setSizes(newSizes);
	}
	else
	{
		QList<int> sizesList;
		sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreLeft);
		sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreRight);
		ui->splitterMedia->setSizes(sizesList);
	}
}

void WidgetMedia::setSkin()
{
	seekSlider->setStyleSheet(skinSettings.seekSlider);
	volumeSlider->setStyleSheet(skinSettings.volumeSlider);
	ui->toolButtonMediaPlaylistTaskHeader->setStyleSheet(skinSettings.taskHeader);
	ui->frameMediaPlaylistTask->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolBarPlayControls->setStyleSheet(skinSettings.mediaToolbar);
	ui->toolBarSettings->setStyleSheet(skinSettings.mediaToolbar);
	ui->listViewMediaPlaylist->setStyleSheet(skinSettings.listViews);
}

void WidgetMedia::open()
{
#ifdef Q_WS_MAEMO_5
	QStringList fileNames;
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Files"), "/");
	if (!fileName.isEmpty())
		fileNames << fileName;
#else
	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"));
#endif
	addToPlaylist(fileNames);
}

void WidgetMedia::addToPlaylist(const QStringList& fileNames)
{
	foreach (QString const &argument, fileNames) {
		QFileInfo fileInfo(argument);
		if (fileInfo.exists()) {
			QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
			if (fileInfo.suffix().toLower() == QLatin1String("m3u")) {
				playlist->load(url);
			} else
				playlist->addMedia(url);
		} else {
			QUrl url(argument);
			if (url.isValid()) {
				playlist->addMedia(url);
			}
		}
	}
	if(playlist->mediaCount() == 1)
	{
		playlist->setCurrentIndex(0);
		player->play();
		ui->actionMediaRewind->setEnabled(true);
	}
	if(playlist->mediaCount() > 1)
	{
		ui->actionMediaRewind->setEnabled(true);
		ui->actionMediaNextTrack->setEnabled(true);
	}
}

void WidgetMedia::durationChanged(qint64 duration)
{
	seekSlider->setMaximum(duration / 1000);
	mediaDuration = QString("%1").arg(duration / 1000);
	setTrackInfo(QString(" %1/%2 ").arg(mediaPosition).arg(mediaDuration));
}

void WidgetMedia::positionChanged(qint64 progress)
{
	if (!seekSlider->isSliderDown()) {
		seekSlider->setValue(progress / 1000);
	}
	mediaPosition = QString("%1").arg(progress / 1000);
	setTrackInfo(QString(" %1/%2 ").arg(mediaPosition).arg(mediaDuration));
}

void WidgetMedia::metaDataChanged()
{
	//qDebug() << "update metadata" << player->metaData(QtMultimediaKit::Title).toString();
	if (player->isMetaDataAvailable()) {
		coverLabel->setText(QString("%1 - %2")
				.arg(player->metaData(QtMultimediaKit::AlbumArtist).toString())
				.arg(player->metaData(QtMultimediaKit::Title).toString()));

		if (coverLabel->isVisible()) {
			QUrl url = player->metaData(QtMultimediaKit::CoverArtUrlLarge).value<QUrl>();

			coverLabel->setPixmap(!url.isEmpty()
					? QPixmap(url.toString())
					: QPixmap());
		}
	} else {
		coverLabel->setText(ui->listViewMediaPlaylist->currentIndex().data().toString());
	}
}

void WidgetMedia::previousClicked()
{
	// Go to previous track if we are within the first 5 seconds of playback
	// Otherwise, seek to the beginning.
	if(player->position() <= 5000)
		playlist->previous();
	else
		player->setPosition(0);
}

void WidgetMedia::jump(const QModelIndex &index)
{
	if (index.isValid()) {
		playlist->setCurrentIndex(index.row());
		player->play();
	}
}

void WidgetMedia::playlistPositionChanged(int currentItem)
{
	ui->listViewMediaPlaylist->setCurrentIndex(playlistModel->index(currentItem, 0));
}

void WidgetMedia::seek(int seconds)
{
	player->setPosition(seconds * 1000);
}

void WidgetMedia::statusChanged(QMediaPlayer::MediaStatus status)
{
	handleCursor(status);

	// handle status message
	switch (status) {
	case QMediaPlayer::UnknownMediaStatus:
	case QMediaPlayer::NoMedia:
	case QMediaPlayer::LoadedMedia:
	case QMediaPlayer::BufferingMedia:
	case QMediaPlayer::BufferedMedia:
		setStatusInfo(QString());
		break;
	case QMediaPlayer::LoadingMedia:
		setStatusInfo(tr("Loading..."));
		break;
	case QMediaPlayer::StalledMedia:
		setStatusInfo(tr("Media Stalled"));
		break;
	case QMediaPlayer::EndOfMedia:
		if(quazaaSettings.Media.Shuffle)
		{
			jump(playlistModel->index(common::getRandomNum(0,playlist->mediaCount()-1), 0));
		} else {
			if(quazaaSettings.Media.Repeat && (playlist->currentIndex() == playlist->mediaCount()-1))
			{
				jump(playlistModel->index(0, 0));
				play();
			} else {
				QApplication::alert(this);
			}
		}
		break;
	case QMediaPlayer::InvalidMedia:
		displayErrorMessage();
		break;
	}
}

void WidgetMedia::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
	if( status == QMediaPlayer::LoadingMedia ||
		status == QMediaPlayer::BufferingMedia ||
		status == QMediaPlayer::StalledMedia)
		setCursor(QCursor(Qt::BusyCursor));
	else
		unsetCursor();
#endif
}

void WidgetMedia::bufferingProgress(int progress)
{
	setStatusInfo(tr("Buffering %4%").arg(progress));
}

void WidgetMedia::videoAvailableChanged(bool available)
{
	ui->actionMediaFullscreen->setEnabled(available);
	toggleVideo(available);
	if (!available)
		videoWidget->setFullScreen(false);
#ifndef PLAYER_NO_COLOROPTIONS
	ui->actionMediaSettings->setEnabled(available);
#endif
}

void WidgetMedia::showFullScreen()
{
	if(player->isVideoAvailable())
		videoWidget->setFullScreen(true);
}

void WidgetMedia::setTrackInfo(const QString &info)
{
	qDebug() << "trackInfo should be: " << info;
	trackInfo = info;
	if (!statusInfo.isEmpty())
		labelTime->setText(QString(" %1 | %2 ").arg(trackInfo).arg(statusInfo));
	else
		labelTime->setText(QString(" %1 ").arg(trackInfo));
}

void WidgetMedia::setStatusInfo(const QString &info)
{
	statusInfo = info;
	if (!statusInfo.isEmpty())
		labelTime->setText(QString(" %1 | %2 ").arg(trackInfo).arg(statusInfo));
	else
		labelTime->setText(QString(" %1 ").arg(trackInfo));
}

void WidgetMedia::displayErrorMessage()
{
	setStatusInfo(player->errorString());


}

#ifndef PLAYER_NO_COLOROPTIONS
void WidgetMedia::showColorDialog()
{
	if (!colorDialog) {
		QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
		brightnessSlider->setRange(-100, 100);
		brightnessSlider->setValue(videoWidget->brightness());
		connect(brightnessSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setBrightness(int)));
		connect(videoWidget, SIGNAL(brightnessChanged(int)), brightnessSlider, SLOT(setValue(int)));

		QSlider *contrastSlider = new QSlider(Qt::Horizontal);
		contrastSlider->setRange(-100, 100);
		contrastSlider->setValue(videoWidget->contrast());
		connect(contrastSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setContrast(int)));
		connect(videoWidget, SIGNAL(contrastChanged(int)), contrastSlider, SLOT(setValue(int)));

		QSlider *hueSlider = new QSlider(Qt::Horizontal);
		hueSlider->setRange(-100, 100);
		hueSlider->setValue(videoWidget->hue());
		connect(hueSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setHue(int)));
		connect(videoWidget, SIGNAL(hueChanged(int)), hueSlider, SLOT(setValue(int)));

		QSlider *saturationSlider = new QSlider(Qt::Horizontal);
		saturationSlider->setRange(-100, 100);
		saturationSlider->setValue(videoWidget->saturation());
		connect(saturationSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setSaturation(int)));
		connect(videoWidget, SIGNAL(saturationChanged(int)), saturationSlider, SLOT(setValue(int)));

		QFormLayout *layout = new QFormLayout;
		layout->addRow(tr("Brightness"), brightnessSlider);
		layout->addRow(tr("Contrast"), contrastSlider);
		layout->addRow(tr("Hue"), hueSlider);
		layout->addRow(tr("Saturation"), saturationSlider);

		QPushButton *button = new QPushButton(tr("Close"));
		layout->addRow(button);

		colorDialog = new QDialog(this);
		colorDialog->setWindowTitle(tr("Color Options"));
		colorDialog->setLayout(layout);

		connect(button, SIGNAL(clicked()), colorDialog, SLOT(close()));
	}
	colorDialog->show();
}
#endif

void WidgetMedia::setState(QMediaPlayer::State state)
{
	if (state != playerState) {
		playerState = state;

		switch (state) {
		case QMediaPlayer::StoppedState:
			ui->actionMediaStop->setEnabled(false);
			ui->actionMediaPlay->setEnabled(true);
			ui->actionMediaPlay->setChecked(false);
			ui->actionMediaPlay->setToolTip(tr("Play"));
			break;
		case QMediaPlayer::PlayingState:
			ui->actionMediaStop->setEnabled(true);
			ui->actionMediaPlay->setEnabled(true);
			ui->actionMediaPlay->setChecked(true);
			ui->actionMediaPlay->setToolTip(tr("Pause"));
			break;
		case QMediaPlayer::PausedState:
			ui->actionMediaStop->setEnabled(true);
			ui->actionMediaPlay->setEnabled(true);
			ui->actionMediaPlay->setChecked(false);
			ui->actionMediaPlay->setToolTip(tr("Play"));
			break;
		}
	}
}

int WidgetMedia::volume() const
{
	return volumeSlider ? volumeSlider->value() : 0;
}

void WidgetMedia::setVolume(int volume)
{
	if (volumeSlider)
		volumeSlider->setValue(volume);
}

bool WidgetMedia::isMuted() const
{
	return playerMuted;
}

void WidgetMedia::setMuted(bool muted)
{
	if (muted != playerMuted) {
		playerMuted = muted;

		ui->actionMediaMute->setChecked(muted);
		if(muted)
			ui->actionMediaMute->setToolTip(tr("Unmute"));
		else
			ui->actionMediaMute->setToolTip(tr("Mute"));
	}
}

void WidgetMedia::playClicked()
{
	if(player->isVideoAvailable())
	{
		toggleVideo(true);
	}
	switch (playerState) {
	case QMediaPlayer::StoppedState:
		play();
		break;
	case QMediaPlayer::PausedState:
		play();
		break;
	case QMediaPlayer::PlayingState:
		pause();
		break;
	}
}

void WidgetMedia::stopClicked()
{
	toggleVideo(true);
	player->stop();
}

qreal WidgetMedia::playbackRate() const
{
	return rateBox->itemData(rateBox->currentIndex()).toDouble();
}

void WidgetMedia::setPlaybackRate(float rate)
{
	for (int i=0; i<rateBox->count(); i++) {
		if (qFuzzyCompare(rate, float(rateBox->itemData(i).toDouble()))) {
			rateBox->setCurrentIndex(i);
			return;
		}
	}

	rateBox->addItem( QString("%1x").arg(rate), QVariant(rate));
	rateBox->setCurrentIndex(rateBox->count()-1);
}

void WidgetMedia::updateRate()
{
	emit changeRate(playbackRate());
}

void WidgetMedia::setControlsEnabled(bool enabled)
{
	if(enabled)
	{
		ui->actionMediaRewind->setEnabled(true);
		ui->actionMediaStop->setEnabled(true);
		ui->actionMediaPlay->setEnabled(true);
		ui->actionMediaNextTrack->setEnabled(true);
		seekSlider->setEnabled(true);
		volumeSlider->setEnabled(true);
	} else {
		ui->actionMediaRewind->setEnabled(false);
		ui->actionMediaStop->setEnabled(false);
		ui->actionMediaPlay->setEnabled(false);
		ui->actionMediaNextTrack->setEnabled(false);
		seekSlider->setEnabled(false);
		volumeSlider->setEnabled(false);
	}
}

void WidgetMedia::toggleVideo(bool visible)
{
	videoWidget->setVisible(visible);
	coverLabel->setVisible(!visible);
}
