#ifndef WIDGETHOME_H
#define WIDGETHOME_H

#include <QWidget>

namespace Ui {
    class widgetHome;
}

class widgetHome : public QWidget {
    Q_OBJECT
public:
    widgetHome(QWidget *parent = 0);
    ~widgetHome();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::widgetHome *ui;
};

#endif // WIDGETHOME_H
