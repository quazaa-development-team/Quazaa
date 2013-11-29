#ifndef MEDIAOVERLAY_H
#define MEDIAOVERLAY_H

#include <QWidget>
#include <QTimer>

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
};

#endif // MEDIAOVERLAY_H
