#ifndef WIDGETMEDIA_H
#define WIDGETMEDIA_H

#include <QMainWindow>

namespace Ui {
    class WidgetMedia;
}

class WidgetMedia : public QMainWindow {
    Q_OBJECT
public:
    WidgetMedia(QWidget *parent = 0);
    ~WidgetMedia();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetMedia *ui;
};

#endif // WIDGETMEDIA_H
