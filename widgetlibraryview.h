#ifndef WIDGETLIBRARYVIEW_H
#define WIDGETLIBRARYVIEW_H

#include <QMainWindow>

namespace Ui {
    class WidgetLibraryView;
}

class WidgetLibraryView : public QMainWindow {
    Q_OBJECT
public:
    WidgetLibraryView(QWidget *parent = 0);
    ~WidgetLibraryView();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetLibraryView *ui;
};

#endif // WIDGETLIBRARYVIEW_H
