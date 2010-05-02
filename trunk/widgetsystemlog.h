#ifndef WIDGETSYSTEMLOG_H
#define WIDGETSYSTEMLOG_H

#include <QMainWindow>

namespace Ui {
    class WidgetSystemLog;
}

class WidgetSystemLog : public QMainWindow {
    Q_OBJECT
public:
    WidgetSystemLog(QWidget *parent = 0);
    ~WidgetSystemLog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSystemLog *ui;
};

#endif // WIDGETSYSTEMLOG_H
