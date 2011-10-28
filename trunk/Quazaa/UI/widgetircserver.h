#ifndef WIDGETIRCSERVER_H
#define WIDGETIRCSERVER_H

#include <QWidget>

namespace Ui {
    class WidgetIRCServer;
}

class WidgetIRCServer : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetIRCServer(QWidget *parent = 0);
    ~WidgetIRCServer();

private:
    Ui::WidgetIRCServer *ui;
};

#endif // WIDGETIRCSERVER_H
