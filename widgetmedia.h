#ifndef WIDGETMEDIA_H
#define WIDGETMEDIA_H

#include <QWidget>

namespace Ui {
    class widgetMedia;
}

class widgetMedia : public QWidget {
    Q_OBJECT
public:
    widgetMedia(QWidget *parent = 0);
    ~widgetMedia();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::widgetMedia *ui;
};

#endif // WIDGETMEDIA_H
