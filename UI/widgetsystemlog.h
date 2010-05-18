#ifndef WIDGETSYSTEMLOG_H
#define WIDGETSYSTEMLOG_H

#include <QMainWindow>
#include <QTime>
#include "types.h"

namespace Ui {
    class WidgetSystemLog;
}

class WidgetSystemLog : public QMainWindow {
    Q_OBJECT
public:
    WidgetSystemLog(QWidget *parent = 0);
    ~WidgetSystemLog();
    void saveWidget();
	QTime timeStamp;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSystemLog *ui;

private slots:
	void on_actionClearBuffer_triggered();
 void skinChangeEvent();
        void appendLog(QString message, LogSeverity::Severity severity = LogSeverity::Information);
};

#endif // WIDGETSYSTEMLOG_H
