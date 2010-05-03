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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetHitMonitor *ui;
};

#endif // WIDGETHITMONITOR_H
