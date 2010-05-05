#ifndef WIDGETSECURITY_H
#define WIDGETSECURITY_H

#include <QMainWindow>

namespace Ui {
    class WidgetSecurity;
}

class WidgetSecurity : public QMainWindow {
    Q_OBJECT
public:
    WidgetSecurity(QWidget *parent = 0);
    ~WidgetSecurity();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSecurity *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETSECURITY_H
