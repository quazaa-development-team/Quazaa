#ifndef WIDGETNEIGHBORS_H
#define WIDGETNEIGHBORS_H

#include <QMainWindow>
#include <QAbstractItemModel>

namespace Ui {
    class WidgetNeighbors;
}

class WidgetNeighbors : public QMainWindow {
    Q_OBJECT
public:
    WidgetNeighbors(QWidget *parent = 0);
    ~WidgetNeighbors();
	void setModel(QAbstractItemModel *model);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetNeighbors *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETNEIGHBORS_H
