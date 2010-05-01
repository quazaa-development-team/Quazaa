#ifndef WIDGETLIBRARY_H
#define WIDGETLIBRARY_H

#include <QWidget>

namespace Ui {
    class widgetLibrary;
}

class widgetLibrary : public QWidget {
    Q_OBJECT
public:
    widgetLibrary(QWidget *parent = 0);
    ~widgetLibrary();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::widgetLibrary *ui;
};

#endif // WIDGETLIBRARY_H
