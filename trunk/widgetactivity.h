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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetActivity *ui;
};

#endif // WIDGETACTIVITY_H
