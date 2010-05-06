#ifndef WIDGETPACKETDUMP_H
#define WIDGETPACKETDUMP_H

#include <QMainWindow>

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
};

#endif // WIDGETPACKETDUMP_H
