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

#include <QtGui>

#define SLIDER_RANGE 8
#define PHONON_MMF_DIRECT_WRITE_ALPHA

#include "mediaplayer.h"
#include "ui_mediasettings.h"
#include "quazaasettings.h"
#include "quazaaglobals.h"
#include <QFileInfo>
#include <QSettings>

class MediaVideoWidget : public Phonon::VideoWidget
{
public:
	MediaVideoWidget(MediaPlayer *player, QWidget *parent = 0) :
		Phonon::VideoWidget(parent), m_player(player), m_action(this)
	{
		m_action.setCheckable(true);
		m_action.setChecked(false);
		m_action.setShortcut(QKeySequence( Qt::AltModifier + Qt::Key_Return));
		m_action.setShortcutContext(Qt::WindowShortcut);
		connect(&m_action, SIGNAL(toggled(bool)), SLOT(setFullScreen(bool)));
		addAction(&m_action);
		setAcceptDrops(true);
	}

protected:
	void mouseDoubleClickEvent(QMouseEvent *e)
	{
		Phonon::VideoWidget::mouseDoubleClickEvent(e);
		setFullScreen(!isFullScreen());
	}

	void keyPressEvent(QKeyEvent *e)
	{
		if (e->key() == Qt::Key_Space && !e->modifiers()) {
			m_player->playPause();
			e->accept();
			return;
		} else if (e->key() == Qt::Key_Escape && !e->modifiers()) {
			setFullScreen(false);
			e->accept();
			return;
		}
		Phonon::VideoWidget::keyPressEvent(e);
	}

	bool event(QEvent *e)
	{
		switch(e->type())
		{
		case QEvent::Close:
			//we just ignore the cose events on the video widget
			//this prevents ALT+F4 from having an effect in fullscreen mode
			e->ignore();
			return true;
		case QEvent::MouseMove:
#ifndef QT_NO_CURSOR
			unsetCursor();
#endif
			//fall through
		case QEvent::WindowStateChange:
			{
				//we just update the state of the checkbox, in case it wasn't already
				m_action.setChecked(windowState() & Qt::WindowFullScreen);
				const Qt::WindowFlags flags = m_player->windowFlags();
				if (windowState() & Qt::WindowFullScreen) {
					m_timer.start(1000, this);
				} else {
					m_timer.stop();
#ifndef QT_NO_CURSOR
					unsetCursor();
#endif
				}
			}
			break;
		default:
			break;
		}

		return Phonon::VideoWidget::event(e);
	}

	void timerEvent(QTimerEvent *e)
	{
		if (e->timerId() == m_timer.timerId()) {
			//let's store the cursor shape
#ifndef QT_NO_CURSOR
			setCursor(Qt::BlankCursor);
#endif
		}
		Phonon::VideoWidget::timerEvent(e);
	}

	void dropEvent(QDropEvent *e)
	{
		m_player->handleDrop(e);
	}

	void dragEnterEvent(QDragEnterEvent *e) {
		if (e->mimeData()->hasUrls())
			e->acceptProposedAction();
	}

private:
	MediaPlayer *m_player;
	QBasicTimer m_timer;
	QAction m_action;
};


MediaPlayer::MediaPlayer(QWidget *parent) :
		QWidget(parent), nextEffect(0), settingsDialog(0), ui(0),
			metaInformationResolver(new Phonon::MediaObject(this)),
			m_AudioOutput(Phonon::VideoCategory),
			m_videoWidget(new MediaVideoWidget(this, parent))
{
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setAutoFillBackground(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	m_videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);

	info = new QLabel(this);
	info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	info->setMinimumHeight(70);
	info->setAcceptDrops(false);
	info->setMargin(2);
	info->setFrameStyle(QFrame::StyledPanel);
	info->setLineWidth(2);
	info->setAutoFillBackground(true);

	QPalette palette;
	palette.setBrush(QPalette::WindowText, Qt::white);

	info->setStyleSheet("background-color: black");
	info->setPalette(palette);
	info->setAlignment(Qt::AlignCenter);
	info->setText(tr("No Media"));

	vLayout->addWidget(info);
	m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	vLayout->addWidget(m_videoWidget);
	m_videoWidget->hide();

#ifdef Q_OS_MAC
	info->setMinimumHeight(100);
	info->setFont(QFont("verdana", 15));
	// QStyle *flatButtonStyle = new QWindowsStyle;
	// openButton->setStyle(flatButtonStyle);
	// playButton->setStyle(flatButtonStyle);
	// rewindButton->setStyle(flatButtonStyle);
	// forwardButton->setStyle(flatButtonStyle);
 #endif

	setLayout(vLayout);

	// Create menu bar:
	fileMenu = new QMenu(this);
	QAction *openFileAction = fileMenu->addAction(tr("Open &File..."));
//	QAction *openUrlAction = fileMenu->addAction(tr("Open &Location..."));

	fileMenu->addSeparator();
	QMenu *aspectMenu = fileMenu->addMenu(tr("&Aspect Ratio"));
	QActionGroup *aspectGroup = new QActionGroup(aspectMenu);
	connect(aspectGroup, SIGNAL(triggered(QAction *)), this, SLOT(aspectChanged(QAction *)));
	aspectGroup->setExclusive(true);
	QAction *aspectActionAuto = aspectMenu->addAction(tr("Auto"));
	aspectActionAuto->setCheckable(true);
	aspectActionAuto->setChecked(true);
	aspectGroup->addAction(aspectActionAuto);
	QAction *aspectActionScale = aspectMenu->addAction(tr("Scale"));
	aspectActionScale->setCheckable(true);
	aspectGroup->addAction(aspectActionScale);
	QAction *aspectAction16_9 = aspectMenu->addAction(tr("16/9"));
	aspectAction16_9->setCheckable(true);
	aspectGroup->addAction(aspectAction16_9);
	QAction *aspectAction4_3 = aspectMenu->addAction(tr("4/3"));
	aspectAction4_3->setCheckable(true);
	aspectGroup->addAction(aspectAction4_3);

	QMenu *scaleMenu = fileMenu->addMenu(tr("&Scale Mode"));
	QActionGroup *scaleGroup = new QActionGroup(scaleMenu);
	connect(scaleGroup, SIGNAL(triggered(QAction *)), this, SLOT(scaleChanged(QAction *)));
	scaleGroup->setExclusive(true);
	QAction *scaleActionFit = scaleMenu->addAction(tr("Fit In View"));
	scaleActionFit->setCheckable(true);
	scaleActionFit->setChecked(true);
	scaleGroup->addAction(scaleActionFit);
	QAction *scaleActionCrop = scaleMenu->addAction(tr("Scale and Crop"));
	scaleActionCrop->setCheckable(true);
	scaleGroup->addAction(scaleActionCrop);

	fileMenu->addSeparator();
	QAction *settingsAction = fileMenu->addAction(tr("&Settings..."));

	// Setup signal connections:
	connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFiles()));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
	connect(m_videoWidget, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
	connect(&m_AudioOutput, SIGNAL(mutedChanged(bool)), this, SLOT(muteChanged(bool)));
	connect(&m_AudioOutput, SIGNAL(volumeChanged(qreal)), this, SLOT(volumeChanged(qreal)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
	connect(&m_MediaObject, SIGNAL(metaDataChanged()), this, SLOT(updateInfo()));
	connect(&m_MediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(&metaInformationResolver, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
			this, SLOT(metaStateChanged(Phonon::State, Phonon::State)));
	connect(&m_MediaObject, SIGNAL(bufferStatus(int)), this, SLOT(bufferStatus(int)));
	connect(&m_MediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(updateTotalTime()));
	connect(&m_MediaObject, SIGNAL(tick(qint64)), this, SLOT(updateTotalTime()));
	connect(&m_MediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
			this, SLOT(sourceChanged(const Phonon::MediaSource &)));

	emit rewindButtonEnableChanged(false);
	emit playButtonEnableChanged(false);
	emit stopButtonEnableChanged(false);
	setAcceptDrops(true);

	m_audioOutputPath = Phonon::createPath(&m_MediaObject, &m_AudioOutput);
	Phonon::createPath(&m_MediaObject, m_videoWidget);
	openFiles(quazaaSettings.MediaPlaylist, true);
}

void MediaPlayer::stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
	Q_UNUSED(oldstate);

	switch (newstate) {
		case Phonon::ErrorState:
			QMessageBox::warning(0, "Quazaa Mediaplayer", m_MediaObject.errorString(), QMessageBox::Close);
			if (m_MediaObject.errorType() == Phonon::FatalError) {
				emit playButtonEnableChanged(false);
				emit rewindButtonEnableChanged(false);
				emit stopButtonEnableChanged(false);
			} else {
				m_MediaObject.pause();
			}
			break;
		case Phonon::PausedState:
			emit playStatusChanged(QIcon(":/Resource/Media/PlayMedia.png"), "Play");
		case Phonon::StoppedState:
			emit playStatusChanged(QIcon(":/Resource/Media/PlayMedia.png"), "Play");
			emit stopButtonEnableChanged(false);

			m_videoWidget->setFullScreen(false);

			if ((m_MediaObject.currentSource().type() != Phonon::MediaSource::Invalid) && (!sources.isEmpty())){
				emit playButtonEnableChanged(true);
				emit rewindButtonEnableChanged(true);
			} else {
				emit playButtonEnableChanged(false);
				emit rewindButtonEnableChanged(false);
			}
			break;
		case Phonon::PlayingState:
			emit playButtonEnableChanged(true);
			emit stopButtonEnableChanged(true);
			emit playStatusChanged(QIcon(":/Resource/Media/PauseMedia.png"), "Pause");
			if (m_MediaObject.hasVideo())
				toggleVideo(true);//m_videoWidget.show();
			// Fall through
		case Phonon::BufferingState:
			emit rewindButtonEnableChanged(true);
			break;
		case Phonon::LoadingState:
			emit rewindButtonEnableChanged(false);
			break;
	}

}

void MediaPlayer::initSettingsDialog()
{
	settingsDialog = new QDialog();
	ui = new Ui_mediaSettings();
	ui->setupUi(settingsDialog);

//	connect(ui->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(setBrightness(int)));
//	connect(ui->hueSlider, SIGNAL(valueChanged(int)), this, SLOT(setHue(int)));
//	connect(ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(setSaturation(int)));
//	connect(ui->contrastSlider , SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
	connect(ui->aspectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAspect(int)));
	connect(ui->scalemodeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setScale(int)));

//	ui->brightnessSlider->setValue(int(m_videoWidget->brightness() * SLIDER_RANGE));
//	ui->hueSlider->setValue(int(m_videoWidget->hue() * SLIDER_RANGE));
//	ui->saturationSlider->setValue(int(m_videoWidget->saturation() * SLIDER_RANGE));
//	ui->contrastSlider->setValue(int(m_videoWidget->contrast() * SLIDER_RANGE));
	ui->aspectCombo->setCurrentIndex(m_videoWidget->aspectRatio());
	ui->scalemodeCombo->setCurrentIndex(m_videoWidget->scaleMode());
	connect(ui->effectButton, SIGNAL(clicked()), this, SLOT(configureEffect()));

#ifdef Q_WS_X11
	//Cross fading is not currently implemented in the GStreamer backend
	ui->crossFadeSlider->setVisible(false);
	ui->crossFadeLabel->setVisible(false);
	ui->crossFadeLabel1->setVisible(false);
	ui->crossFadeLabel2->setVisible(false);
	ui->crossFadeLabel3->setVisible(false);
#endif
	ui->crossFadeSlider->setValue((int)(2 * m_MediaObject.transitionTime() / 1000.0f));

	// Insert audio devices:
	QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
	for (int i=0; i<devices.size(); i++){
		QString itemText = devices[i].name();
		if (!devices[i].description().isEmpty()) {
			itemText += QString::fromLatin1(" (%1)").arg(devices[i].description());
		}
		ui->deviceCombo->addItem(itemText);
		if (devices[i] == m_AudioOutput.outputDevice())
			ui->deviceCombo->setCurrentIndex(i);
	}

	// Insert audio effects:
	ui->audioEffectsCombo->addItem(tr("<no effect>"));
	QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
	Phonon::Effect *currEffect = currEffects.size() ? currEffects[0] : 0;
	QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
	for (int i=0; i<availableEffects.size(); i++){
		ui->audioEffectsCombo->addItem(availableEffects[i].name());
		if (currEffect && availableEffects[i] == currEffect->description())
			ui->audioEffectsCombo->setCurrentIndex(i+1);
	}
	connect(ui->audioEffectsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(effectChanged()));

}

void MediaPlayer::effectChanged()
{
	int currentIndex = ui->audioEffectsCombo->currentIndex();
	if (currentIndex) {
		QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
		Phonon::EffectDescription chosenEffect = availableEffects[currentIndex - 1];

		QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
		Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;

		// Deleting the running effect will stop playback, it is deleted when removed from path
		if (nextEffect && !(currentEffect && (currentEffect->description().name() == nextEffect->description().name())))
			delete nextEffect;

		nextEffect = new Phonon::Effect(chosenEffect);
	}
	ui->effectButton->setEnabled(currentIndex);
}

void MediaPlayer::showSettingsDialog()
{
	if (!settingsDialog)
		initSettingsDialog();

//	float oldBrightness = m_videoWidget->brightness();
//	float oldHue = m_videoWidget->hue();
//	float oldSaturation = m_videoWidget->saturation();
//	float oldContrast = m_videoWidget->contrast();
	Phonon::VideoWidget::AspectRatio oldAspect = m_videoWidget->aspectRatio();
	Phonon::VideoWidget::ScaleMode oldScale = m_videoWidget->scaleMode();
	int currentEffect = ui->audioEffectsCombo->currentIndex();
	settingsDialog->exec();

	if (settingsDialog->result() == QDialog::Accepted){
		m_MediaObject.setTransitionTime((int)(1000 * float(ui->crossFadeSlider->value()) / 2.0f));
		QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
		m_AudioOutput.setOutputDevice(devices[ui->deviceCombo->currentIndex()]);
		QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
		QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();

		if (ui->audioEffectsCombo->currentIndex() > 0){
			Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;
			if (!currentEffect || currentEffect->description() != nextEffect->description()){
				foreach(Phonon::Effect *effect, currEffects) {
					m_audioOutputPath.removeEffect(effect);
					delete effect;
				}
				m_audioOutputPath.insertEffect(nextEffect);
			}
		} else {
			foreach(Phonon::Effect *effect, currEffects) {
				m_audioOutputPath.removeEffect(effect);
				delete effect;
				nextEffect = 0;
			}
		}
	} else {
		// Restore previous settings
//		m_videoWidget->setBrightness(oldBrightness);
//		m_videoWidget->setSaturation(oldSaturation);
//		m_videoWidget->setHue(oldHue);
//		m_videoWidget->setContrast(oldContrast);
		m_videoWidget->setAspectRatio(oldAspect);
		m_videoWidget->setScaleMode(oldScale);
		ui->audioEffectsCombo->setCurrentIndex(currentEffect);
	}
}

void MediaPlayer::configureEffect()
{
	if (!nextEffect)
		return;


	QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
	const QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
	if (ui->audioEffectsCombo->currentIndex() > 0) {
		Phonon::EffectDescription chosenEffect = availableEffects[ui->audioEffectsCombo->currentIndex() - 1];

		QDialog effectDialog;
		effectDialog.setWindowTitle(tr("Configure effect"));
		QVBoxLayout *topLayout = new QVBoxLayout(&effectDialog);

		QLabel *description = new QLabel("<b>Description:</b><br>" + chosenEffect.description(), &effectDialog);
		description->setWordWrap(true);
		topLayout->addWidget(description);

		QScrollArea *scrollArea = new QScrollArea(&effectDialog);
		topLayout->addWidget(scrollArea);

		QVariantList savedParamValues;
		foreach(Phonon::EffectParameter param, nextEffect->parameters()) {
			savedParamValues << nextEffect->parameterValue(param);
		}

		QWidget *scrollWidget = new Phonon::EffectWidget(nextEffect);
		scrollWidget->setMinimumWidth(320);
		scrollWidget->setContentsMargins(10, 10, 10,10);
		scrollArea->setWidget(scrollWidget);

		QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &effectDialog);
		connect(bbox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), &effectDialog, SLOT(accept()));
		connect(bbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), &effectDialog, SLOT(reject()));
		topLayout->addWidget(bbox);

		effectDialog.exec();

		if (effectDialog.result() != QDialog::Accepted) {
			//we need to restore the paramaters values
			int currentIndex = 0;
			foreach(Phonon::EffectParameter param, nextEffect->parameters()) {
				nextEffect->setParameterValue(param, savedParamValues.at(currentIndex++));
			}

		}
	}
}

void MediaPlayer::handleDrop(QDropEvent *e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (e->proposedAction() == Qt::MoveAction){
		// Just add to the queue:
		int index = sources.size();
		for (int i=0; i<urls.size(); i++)
			sources.append(Phonon::MediaSource(urls[i].toLocalFile()));
		if (!sources.isEmpty())
		{
			if (!playlistProcessing)
			metaInformationResolver.setCurrentSource(sources.at(index));
		}
	} else {
		// Create new queue:
		m_MediaObject.clearQueue();
		if (urls.size() > 0) {
			QString fileName = urls[0].toLocalFile();
			QDir dir(fileName);
			if (dir.exists()) {
				dir.setFilter(QDir::Files);
				QStringList entries = dir.entryList();
				if (entries.size() > 0) {
					playOnSwitch(true, Phonon::MediaSource(fileName + QDir::separator() +  entries[0]));
					int index = sources.size();
					for (int i=1; i< entries.size(); ++i)
						sources.append(Phonon::MediaSource(fileName + QDir::separator() + entries[i]));
					if (!sources.isEmpty())
					{
						if (!playlistProcessing)
						metaInformationResolver.setCurrentSource(sources.at(index));
					}
				}
			} else {
				playOnSwitch(true, Phonon::MediaSource(fileName));
				int index = sources.size();
				for (int i=1; i<urls.size(); i++)
					sources.append(Phonon::MediaSource(urls[i].toLocalFile()));
				if (!sources.isEmpty())
				{
					if (!playlistProcessing)
					metaInformationResolver.setCurrentSource(sources.at(index));
				}
			}
		}
	}
	emit forwardButtonEnableChanged(sources.count() > 1);
	m_MediaObject.play();
}

void MediaPlayer::dropEvent(QDropEvent *e)
{
	if (e->mimeData()->hasUrls() && e->proposedAction() != Qt::LinkAction) {
		e->acceptProposedAction();
		handleDrop(e);
	} else {
		e->ignore();
	}
}

void MediaPlayer::dragEnterEvent(QDragEnterEvent *e)
{
	dragMoveEvent(e);
}

void MediaPlayer::dragMoveEvent(QDragMoveEvent *e)
{
	if (e->mimeData()->hasUrls()) {
		if (e->proposedAction() == Qt::CopyAction || e->proposedAction() == Qt::MoveAction){
			e->acceptProposedAction();
		}
	}
}

void MediaPlayer::playPause()
{
	if (m_MediaObject.state() == Phonon::PlayingState)
		m_MediaObject.pause();
	else {
		if (m_MediaObject.currentTime() == m_MediaObject.totalTime())
			m_MediaObject.seek(0);
		toggleVideo(m_MediaObject.hasVideo());
		m_MediaObject.play();
	}
}

void MediaPlayer::stop()
{
	m_MediaObject.stop();
	toggleVideo(false);
}

void MediaPlayer::openFiles(QStringList files, bool requireFileList, bool addToList)
{
	if (files.isEmpty() && requireFileList)
		return;
	if (files.isEmpty())
	files = QFileDialog::getOpenFileNames(this, tr("Open Media Files"), quazaaSettings.MediaOpenPath, quazaaGlobals.MediaOpenFilter());
	if (files.isEmpty())
		return;

	if (!requireFileList)
	{
		QFileInfo *fileInfo = new QFileInfo(files.at(0));
		quazaaSettings.MediaOpenPath = fileInfo->canonicalPath();
	}

	if(!addToList)
	{
		stop();
		sources.clear();
		emit playlistClear();
	}

	int index = sources.size();
	foreach (QString string, files) {
			Phonon::MediaSource source(string);

		sources.append(source);
	}

	emit forwardButtonEnableChanged(sources.count() > 1);

	if (!sources.isEmpty())
	{
		if (!playlistProcessing)
		metaInformationResolver.setCurrentSource(sources.at(index));
	}
}

void MediaPlayer::addFiles(QStringList files, bool requireFileList)
{
	openFiles(files, requireFileList, true);
}

void MediaPlayer::setSaturation(int val)
{
//	m_videoWidget->setSaturation(val / qreal(SLIDER_RANGE));
}

void MediaPlayer::setHue(int val)
{
//	m_videoWidget->setHue(val / qreal(SLIDER_RANGE));
}

void MediaPlayer::setAspect(int val)
{
	m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio(val));
}

void MediaPlayer::setScale(int val)
{
	m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleMode(val));
}

void MediaPlayer::setBrightness(int val)
{
//	m_videoWidget->setBrightness(val / qreal(SLIDER_RANGE));
}

void MediaPlayer::setContrast(int val)
{
//	m_videoWidget->setContrast(val / qreal(SLIDER_RANGE));
}

void MediaPlayer::updateInfo()
{
	int maxLength = 30;
	QString font = "<font color=#ffeeaa>";
	QString fontmono = "<font family=\"monospace,courier new\" color=#ffeeaa>";

	QMap <QString, QString> metaData = m_MediaObject.metaData();
	QString trackArtist = metaData.value("ARTIST");
	if (trackArtist.length() > maxLength)
		trackArtist = trackArtist.left(maxLength) + "...";

	QString trackTitle = metaData.value("TITLE");
	int trackBitrate = metaData.value("BITRATE").toInt();

	QString fileName;
	if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) {
		fileName = m_MediaObject.currentSource().url().toString();
	} else {
		fileName = m_MediaObject.currentSource().fileName();
		fileName = fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1);
	}

	QString title;
	if (!trackTitle.isEmpty()) {
		title = "Title: " + font + trackTitle + "<br></font>";
	} else if (!fileName.isEmpty()) {
		title = font + fileName + "</font>";
		if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) {
			title.prepend("Url: ");
		} else {
			title.prepend("File: ");
		}
	}

	QString artist;
	if (!trackArtist.isEmpty())
		artist = "Artist:  " + font + trackArtist + "</font>";

	QString bitrate;
	if (trackBitrate != 0)
		bitrate = "<br>Bitrate:  " + font + QString::number(trackBitrate/1000) + "kbit</font>";

	info->setText(title + artist + bitrate);
}

void MediaPlayer::rewind()
{
	long current = m_MediaObject.currentTime();
	int currentSeconds;
	if (current)
	{
		currentSeconds = current/1000;
	} else {
		currentSeconds = 0;
	}

	if ((currentSeconds <= 1) && (!sources.isEmpty()))
	{
		int index = (sources.indexOf(m_MediaObject.currentSource())) - 1;
		if (index > -1) {
			playOnSwitch(m_MediaObject.state() == Phonon::PlayingState, sources.at(index));
		} else {
			playOnSwitch(m_MediaObject.state() == Phonon::PlayingState, sources.last());
		}
	} else {
		m_MediaObject.seek(0);
	}
}

void MediaPlayer::forward()
{
	int index = sources.indexOf(m_MediaObject.currentSource()) + 1;
	if (sources.size() > index) {
		playOnSwitch(m_MediaObject.state() == Phonon::PlayingState, sources.at(index));
	} else {
		if (sources.size() > 0) {
			playOnSwitch(m_MediaObject.state() == Phonon::PlayingState, sources[0]);
		}
	}
	emit forwardButtonEnableChanged(sources.count() > 1);
}

void MediaPlayer::showContextMenu(const QPoint &p)
{
	fileMenu->popup(m_videoWidget->isFullScreen() ? p : mapToGlobal(p));
}

void MediaPlayer::scaleChanged(QAction *act)
{
	if (act->text() == tr("Scale and crop"))
		m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
	else
		m_videoWidget->setScaleMode(Phonon::VideoWidget::FitInView);
}

void MediaPlayer::aspectChanged(QAction *act)
{
	if (act->text() == tr("16/9"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
	else if (act->text() == tr("Scale"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	else if (act->text() == tr("4/3"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
	else
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
}

void MediaPlayer::toggleVideo(bool bShowVideo)
{
	info->setVisible(!bShowVideo);
	m_videoWidget->setVisible(bShowVideo);
	emit fullscreenEnableChanged(bShowVideo);
}

void MediaPlayer::setVideoWindowFullscreen()
{
	m_videoWidget->setFullScreen(!m_videoWidget->isFullScreen());
}

void MediaPlayer::mediaSourceFinished()
{
	if (!sources.isEmpty())
	{
		if (quazaaSettings.MediaShuffle)
		{
			playOnSwitch(true, sources.at((int)qrand() % sources.size()));
		} else {
			int index = sources.indexOf(m_MediaObject.currentSource()) + 1;
			if (sources.size() > index) {
				playOnSwitch(true, sources.at(index));
			} else {
				if (quazaaSettings.MediaRepeat)
				{
					playOnSwitch(true, sources.at(0));
				}
			}
		}
	} else {
		if (quazaaSettings.MediaRepeat)
		{
			m_MediaObject.seek(0);
			m_MediaObject.play();
		}
	}
}

void MediaPlayer::bufferStatus(int percent)
{
	if (percent == 0 || percent == 100)
		emit progressTextChanged(QString());
	else {
		QString str = QString::fromLatin1("(%1%)").arg(percent);
		emit progressTextChanged(str);
	}
}

void MediaPlayer::updateTotalTime()
{
	long len = m_MediaObject.totalTime();
	long pos = m_MediaObject.currentTime();
	QString timeString;
	if (pos || len)
	{
		int sec = pos/1000;
		int min = sec/60;
		int hour = min/60;
		int msec = pos;

		QTime playTime(hour%60, min%60, sec%60, msec%1000);
		sec = len / 1000;
		min = sec / 60;
		hour = min / 60;
		msec = len;

		QTime stopTime(hour%60, min%60, sec%60, msec%1000);
		QString timeFormat = "m:ss";
		if (hour > 0)
			timeFormat = "h:mm:ss";
		timeString = playTime.toString(timeFormat);
		if (len)
			timeString += " / " + stopTime.toString(timeFormat);

		if ((pos == len) && (m_MediaObject.state() == Phonon::PlayingState))
		{
			mediaSourceFinished();
		}
	}
	emit totalTimeTextChanged(timeString);
}

void MediaPlayer::sourceChanged(const Phonon::MediaSource &source)
{
	emit playlistRowChanged(sources.indexOf(source));
}

void MediaPlayer::metaStateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
	playlistProcessing = true;
	if (newState == Phonon::ErrorState) {
		QMessageBox::warning(0, tr("Error Opening File"),
			metaInformationResolver.errorString() + '\n' + metaInformationResolver.currentSource().fileName());
		int index = sources.indexOf(metaInformationResolver.currentSource());
		sources.removeAt(index);

		if (sources.isEmpty())
		{
			playlistClear();
			quazaaSettings.MediaPlaylist.clear();
			return;
		}

		if (sources.size() > index) {
			metaInformationResolver.setCurrentSource(sources.at(index));
		} else {
			playlistProcessing = false;
			QApplication::processEvents();
			return;
		}
	}

	if (newState != Phonon::StoppedState && newState != Phonon::PausedState)
		return;

	if (metaInformationResolver.currentSource().type() == Phonon::MediaSource::Invalid)
	{
		int index = sources.indexOf(metaInformationResolver.currentSource());
		sources.removeAt(index);
		if (sources.size() > index) {
			metaInformationResolver.setCurrentSource(sources.at(index));
		} else {
			return;
		}
	}

	QMap<QString, QString> metaData = metaInformationResolver.metaData();

	QString title = metaData.value("TITLE");
	if (title == "")
		title = metaInformationResolver.currentSource().fileName();

	QTableWidgetItem *titleItem = new QTableWidgetItem(title);
	titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *artistItem = new QTableWidgetItem(metaData.value("ARTIST"));
	artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *albumItem = new QTableWidgetItem(metaData.value("ALBUM"));
	albumItem->setFlags(albumItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *trackItem = new QTableWidgetItem(metaData.value("TRACK"));
	trackItem->setFlags(trackItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *yearItem = new QTableWidgetItem(metaData.value("DATE"));
	yearItem->setFlags(yearItem->flags() ^ Qt::ItemIsEditable);
	QTableWidgetItem *genreItem = new QTableWidgetItem(metaData.value("GENRE"));
	genreItem->setFlags(genreItem->flags() ^ Qt::ItemIsEditable);

	emit playlistRowCountUpdate();
	emit playlistInsertRow(playlistRowCount);
	emit setPlaylistItem(playlistRowCount, 0, artistItem);
	emit setPlaylistItem(playlistRowCount, 1, titleItem);
	emit setPlaylistItem(playlistRowCount, 2, albumItem);
	emit setPlaylistItem(playlistRowCount, 3, trackItem);
	emit setPlaylistItem(playlistRowCount, 4, yearItem);
	emit setPlaylistItem(playlistRowCount, 5, genreItem);

	emit playlistSelectedItemsIsEmptyUpdate();
	if (this->playlistSelectedItemsIsEmpty) {
		emit playlistRowChanged(0);
		m_MediaObject.setCurrentSource(metaInformationResolver.currentSource());
	}

	int index = sources.indexOf(metaInformationResolver.currentSource()) + 1;
	emit forwardButtonEnableChanged(sources.count() > 1);
	if (sources.size() > index) {
		metaInformationResolver.setCurrentSource(sources.at(index));
	}
	else {
		playlistProcessing = false;
		emit playlistResizeColumnsToContents();
		emit setPlaylistStretchLastSection(true);

		quazaaSettings.MediaPlaylist.clear();
		foreach (Phonon::MediaSource source, sources) {
				quazaaSettings.MediaPlaylist.append(source.fileName());
		}
		return;
	}
}

void MediaPlayer::playOnSwitch(bool play, Phonon::MediaSource source)
{
	if (play)
	{
		m_MediaObject.setCurrentSource(source);
		m_MediaObject.play();
		if (m_MediaObject.hasVideo()) //Try to clear the screen from the last video by forcing a redraw
		{
			m_videoWidget->setVisible(false);
			QApplication::processEvents();
			m_videoWidget->setVisible(true);
		}
	} else {
		m_MediaObject.setCurrentSource(source);
	}
}

void MediaPlayer::playlistDoubleClicked(QModelIndex index)
{
	m_MediaObject.setCurrentSource(sources.at(index.row()));
	m_MediaObject.play();
}

void MediaPlayer::onPlaylistRemove()
{
	if(sources.count() == 0)
		return;

	if(sources.count() == 1)
	{
		onPlaylistClear();
	} else {
		emit playlistCurrentRowUpdate();
		if (playlistCurrentRow == sources.indexOf(m_MediaObject.currentSource()))
		{
			sources.removeAt(playlistCurrentRow);
			emit playlistRemoveRow(playlistCurrentRow);
			if (sources.size() > playlistCurrentRow)
			{
				m_MediaObject.setCurrentSource(sources.at(playlistCurrentRow));
			} else {
				m_MediaObject.setCurrentSource(sources.last());
			}
			m_MediaObject.play();
		} else {
			sources.removeAt(playlistCurrentRow);
			emit playlistRemoveRow(playlistCurrentRow);
		}
	}
}

void MediaPlayer::onPlaylistClear()
{
	if (m_MediaObject.state() == Phonon::PlayingState)
	{
		m_MediaObject.stop();
	} else {
		emit playButtonEnableChanged(false);
		emit rewindButtonEnableChanged(false);
	}
	sources.clear();
	emit playlistClear();
	toggleVideo(false);
	info->setText(tr("No Media"));
	emit playStatusChanged(QIcon(":/Resource/Media/PlayMedia.png"), tr("Play"));
	emit stopButtonEnableChanged(false);
	emit forwardButtonEnableChanged(false);
	emit fullscreenEnableChanged(false);
	QApplication::processEvents();
}

void MediaPlayer::toggleMute()
{
	m_AudioOutput.setMuted(!m_AudioOutput.isMuted());
}

void MediaPlayer::muteChanged(bool muted)
{
	quazaaSettings.MediaMute = muted;
	if (muted)
	{
		emit volumeEnableChanged(false);
		emit muteStatusChanged(QIcon(":/Resource/Media/Mute.png"), "Muted");
	} else {
		emit volumeEnableChanged(true);
		emit muteStatusChanged(QIcon(":/Resource/Media/Unmute.png"), "Mute");
	}
}

void MediaPlayer::volumeChanged(qreal volume)
{
	quazaaSettings.MediaVolume = volume;
}

void MediaPlayer::savePlaylist()
{
	if (sources.isEmpty())
		return;

	QString fileName = QFileDialog::getSaveFileName(0, tr("Save Playlist"),
								quazaaSettings.MediaOpenPath,
								tr("Quazaa Playlist (*.qpl)"));
	if (fileName.isEmpty())
		return;

	QFileInfo fileInfo(fileName);
	quazaaSettings.MediaOpenPath = fileInfo.canonicalPath();

	QStringList playlistEntries;
	foreach (Phonon::MediaSource source, sources) {
		playlistEntries.append(source.fileName());
	}

	QSettings writer(fileName, QSettings::IniFormat);
	writer.setValue("Files", playlistEntries);
}

void MediaPlayer::openPlaylist(bool addToPlaylist)
{
	QString fileName = QFileDialog::getOpenFileName(0, tr("Open Playlist"),
													quazaaSettings.MediaOpenPath,
													tr("Quazaa Playlist (*.qpl)"));
	if (fileName.isEmpty())
		return;

	QFile *file = new QFile(fileName);
	if (!file->exists())
		return;


	QFileInfo fileInfo(fileName);
	quazaaSettings.MediaOpenPath = fileInfo.canonicalPath();

	QStringList playlistEntries;
	QSettings reader(fileName, QSettings::IniFormat);
	playlistEntries = reader.value("Files", QStringList()).toStringList();

	if (addToPlaylist)
		addFiles(playlistEntries, true);
	else
		openFiles(playlistEntries, true);
}

void MediaPlayer::addPlaylist()
{
	openPlaylist(true);
}

void MediaPlayer::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	opaquePaint(&painter, event);
	painter.end();
}

void MediaPlayer::opaquePaint(QPainter *painter, QPaintEvent *event)
{
	painter->setBrush(QBrush(QColor(0,0,0,255)));
	painter->drawRect(event->rect());
}
