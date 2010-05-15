#ifndef WIDGETHITMONITOR_H
#define WIDGETHITMONITOR_H

#include <QMainWindow>

namespace Ui {
    class WidgetHitMonitor;
}

class WidgetHitMonitor : public QMainWindow {
    Q_OBJECT
public:
    WidgetHitMonitor(QWidget *parent = 0);
    ~WidgetHitMonitor();
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetHitMonitor *ui;

private slots:
	void on_actionMore_triggered();
 void skinChangeEvent();
};

#endif // WIDGETHITMONITOR_H
