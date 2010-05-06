#ifndef WIDGETSYSTEMLOG_H
#define WIDGETSYSTEMLOG_H

#include <QMainWindow>
#include "types.h"

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

private slots:
	void skinChangeEvent();
	void appendLog(QString message, LogSeverity::Severity severity = LogSeverity::Information);
};

#endif // WIDGETSYSTEMLOG_H
