#ifndef WIDGETLIBRARY_H
#define WIDGETLIBRARY_H

#include <QWidget>
#include "widgetlibraryview.h"

namespace Ui {
    class WidgetLibrary;
}

class WidgetLibrary : public QWidget {
    Q_OBJECT
public:
    WidgetLibrary(QWidget *parent = 0);
    ~WidgetLibrary();
    WidgetLibraryView *panelLibraryView;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetLibrary *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETLIBRARY_H
