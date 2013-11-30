#ifndef MEDIAOVERLAY_H
#define MEDIAOVERLAY_H

#include <QWidget>
#include <QTimer>
#include <QToolButton>

namespace Ui {
class MediaOverlay;
}

class MediaOverlay : public QWidget
{
	Q_OBJECT

public:
	explicit MediaOverlay(QWidget *parent = 0);
	~MediaOverlay();

private:
	Ui::MediaOverlay *ui;
	QTimer* m_tMediaControls;
	QTimer* m_tVolumeControl;

protected:
	void mouseMoveEvent(QMouseEvent *e);

public slots:
	void showControls();
	QToolButton* playPauseButton();
	QToolButton* previousButton();
	QToolButton* stopButton();
	QToolButton* nextButton();
	QToolButton* openButton();
	QToolButton* replayButton();
	QToolButton* shuffleButton();
	QToolButton* fullScreenButton();
	QToolButton* settingsButton();
	QToolButton* muteButton();
};

#endif // MEDIAOVERLAY_H
