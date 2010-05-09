#ifndef WIDGETGRAPH_H
#define WIDGETGRAPH_H

#include <QMainWindow>

namespace Ui {
    class WidgetGraph;
}

class WidgetGraph : public QMainWindow {
    Q_OBJECT
public:
    WidgetGraph(QWidget *parent = 0);
    ~WidgetGraph();
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetGraph *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETGRAPH_H
