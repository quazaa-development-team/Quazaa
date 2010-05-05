#ifndef WIDGETPACKETDUMP_H
#define WIDGETPACKETDUMP_H

#include <QMainWindow>

namespace LogSeverity {
	enum Severity { Information, Security, Notice, Debug, Warning, Error, Critical };
}

namespace Ui {
    class WidgetPacketDump;
}

class WidgetPacketDump : public QMainWindow {
    Q_OBJECT
public:
    WidgetPacketDump(QWidget *parent = 0);
    ~WidgetPacketDump();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetPacketDump *ui;

private slots:
	void skinChangeEvent();
	void appendLog(QString *message, LogSeverity::Severity severity = LogSeverity::Information);
};

#endif // WIDGETPACKETDUMP_H
