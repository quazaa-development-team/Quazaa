#ifndef WIDGETLIBRARY_H
#define WIDGETLIBRARY_H

#include <QWidget>

namespace Ui {
    class WidgetLibrary;
}

class WidgetLibrary : public QWidget {
    Q_OBJECT
public:
    WidgetLibrary(QWidget *parent = 0);
    ~WidgetLibrary();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetLibrary *ui;
};

#endif // WIDGETLIBRARY_H
