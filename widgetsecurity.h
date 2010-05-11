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
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSecurity *ui;

private slots:
	void on_actionSubscribeSecurityList_triggered();
 void on_actionSecurityAddRule_triggered();
 void skinChangeEvent();
};

#endif // WIDGETSECURITY_H
