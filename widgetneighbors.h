#ifndef WIDGETNEIGHBORS_H
#define WIDGETNEIGHBORS_H

#include <QMainWindow>

namespace Ui {
    class WidgetNeighbors;
}

class WidgetNeighbors : public QMainWindow {
    Q_OBJECT
public:
    WidgetNeighbors(QWidget *parent = 0);
    ~WidgetNeighbors();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetNeighbors *ui;
};

#endif // WIDGETNEIGHBORS_H
