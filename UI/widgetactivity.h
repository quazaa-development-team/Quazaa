#ifndef WIDGETACTIVITY_H
#define WIDGETACTIVITY_H

#include <QWidget>
#include "widgetneighbors.h"
#include "widgetsystemlog.h"

namespace Ui {
    class WidgetActivity;
}

class WidgetActivity : public QWidget {
    Q_OBJECT
public:
    WidgetActivity(QWidget *parent = 0);
    ~WidgetActivity();
	WidgetNeighbors *panelNeighbors;
	WidgetSystemLog *panelSystemLog;
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetActivity *ui;

private slots:
	void on_splitterActivity_customContextMenuRequested(QPoint pos);
 void skinChangeEvent();
};

#endif // WIDGETACTIVITY_H
