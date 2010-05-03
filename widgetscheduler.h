#ifndef WIDGETSCHEDULER_H
#define WIDGETSCHEDULER_H

#include <QMainWindow>

namespace Ui {
    class WidgetScheduler;
}

class WidgetScheduler : public QMainWindow {
    Q_OBJECT
public:
    WidgetScheduler(QWidget *parent = 0);
    ~WidgetScheduler();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetScheduler *ui;
};

#endif // WIDGETSCHEDULER_H
