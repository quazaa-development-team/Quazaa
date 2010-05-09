#ifndef WIDGETDISCOVERY_H
#define WIDGETDISCOVERY_H

#include <QMainWindow>

namespace Ui {
    class WidgetDiscovery;
}

class WidgetDiscovery : public QMainWindow {
    Q_OBJECT
public:
    WidgetDiscovery(QWidget *parent = 0);
    ~WidgetDiscovery();
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetDiscovery *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETDISCOVERY_H
