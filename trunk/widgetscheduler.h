#ifndef WIDGETSCHEDULER_H
#define WIDGETSCHEDULER_H

#include <QMainWindow>

namespace Ui {
    class WidgetScheduler;
}

class WidgetScheduler : public QMainWindow {
    Q_OBJECT
public:
    WidgetScheduler(QWidget *parent = 0);
    ~WidgetScheduler();
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetScheduler *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETSCHEDULER_H
