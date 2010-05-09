#ifndef WIDGETSEARCHMONITOR_H
#define WIDGETSEARCHMONITOR_H

#include <QMainWindow>

namespace Ui {
    class WidgetSearchMonitor;
}

class WidgetSearchMonitor : public QMainWindow {
    Q_OBJECT
public:
    WidgetSearchMonitor(QWidget *parent = 0);
    ~WidgetSearchMonitor();
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearchMonitor *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETSEARCHMONITOR_H
