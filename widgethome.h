#ifndef WIDGETHOME_H
#define WIDGETHOME_H

#include <QWidget>

namespace Ui {
    class WidgetHome;
}

class WidgetHome : public QWidget {
    Q_OBJECT
public:
    WidgetHome(QWidget *parent = 0);
    ~WidgetHome();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetHome *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETHOME_H
