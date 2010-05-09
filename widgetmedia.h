#ifndef WIDGETMEDIA_H
#define WIDGETMEDIA_H

#include <QMainWindow>

#include "vlcmediaplayer.h"

namespace Ui {
    class WidgetMedia;
}

class WidgetMedia : public QMainWindow {
    Q_OBJECT
public:
    WidgetMedia(QWidget *parent = 0);
    ~WidgetMedia();
	vlcMediaPlayer *mediaPlayer;
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetMedia *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETMEDIA_H
