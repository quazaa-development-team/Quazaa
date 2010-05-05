#ifndef WIDGETDOWNLOADS_H
#define WIDGETDOWNLOADS_H

#include <QMainWindow>

namespace Ui {
    class WidgetDownloads;
}

class WidgetDownloads : public QMainWindow {
    Q_OBJECT
public:
    WidgetDownloads(QWidget *parent = 0);
    ~WidgetDownloads();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetDownloads *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETDOWNLOADS_H
