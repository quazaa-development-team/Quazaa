#ifndef WIDGETACTIVITY_H
#define WIDGETACTIVITY_H

#include <QWidget>

namespace Ui {
    class WidgetActivity;
}

class WidgetActivity : public QWidget {
    Q_OBJECT
public:
    WidgetActivity(QWidget *parent = 0);
    ~WidgetActivity();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetActivity *ui;
};

#endif // WIDGETACTIVITY_H
