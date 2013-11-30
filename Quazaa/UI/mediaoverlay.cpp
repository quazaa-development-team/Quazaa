#include "mediaoverlay.h"
#include "ui_mediaoverlay.h"

#include <QDebug>

MediaOverlay::MediaOverlay(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MediaOverlay)
{
	ui->setupUi(this);

	setPalette(Qt::transparent);
	setAttribute(Qt::WA_OpaquePaintEvent, false);
	setAttribute(Qt::WA_NoSystemBackground);

	m_tMediaControls = new QTimer(this);
	m_tMediaControls->setSingleShot(true);
	connect(m_tMediaControls, SIGNAL(timeout()), ui->frameMediaControls, SLOT(hide()));
	m_tVolumeControl = new QTimer(this);
	m_tVolumeControl->setSingleShot(true);
	connect(m_tVolumeControl, SIGNAL(timeout()), ui->horizontalSliderVolume, SLOT(hide()));

	ui->frameMediaControls->hide();
	ui->horizontalSliderVolume->hide();
}

MediaOverlay::~MediaOverlay()
{
	delete ui;
}

void MediaOverlay::mouseMoveEvent(QMouseEvent *e)
{
	if(underMouse()) {
		if(!ui->frameMediaControls->isVisible())
			ui->frameMediaControls->show();

		m_tMediaControls->start(5000);
	}

	if(ui->toolButtonMute->underMouse()) {
		if(!ui->horizontalSliderVolume->isVisible())
			ui->horizontalSliderVolume->show();

		m_tVolumeControl->start(5000);
	}

	QWidget::mouseMoveEvent(e);
}

void MediaOverlay::showControls()
{
	if(!ui->frameMediaControls->isVisible())
		ui->frameMediaControls->show();

	m_tMediaControls->start(5000);
}

QToolButton *MediaOverlay::playPauseButton()
{
	return ui->toolButtonPlayPause;
}

QToolButton *MediaOverlay::previousButton()
{
	return ui->toolButtonPrevious;
}

QToolButton *MediaOverlay::stopButton()
{
	return ui->toolButtonStop;
}

QToolButton *MediaOverlay::nextButton()
{
	return ui->toolButtonNext;
}

QToolButton *MediaOverlay::openButton()
{
	return ui->toolButtonOpen;
}

QToolButton *MediaOverlay::replayButton()
{
	return ui->toolButtonReplay;
}

QToolButton *MediaOverlay::shuffleButton()
{
	return ui->toolButtonShuffle;
}

QToolButton *MediaOverlay::fullScreenButton()
{
	return ui->toolButtonFullScreen;
}

QToolButton *MediaOverlay::settingsButton()
{
	return ui->toolButtonSettings;
}

QToolButton *MediaOverlay::muteButton()
{
	return ui->toolButtonMute;
}
