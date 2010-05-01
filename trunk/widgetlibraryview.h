#ifndef WIDGETLIBRARYVIEW_H
#define WIDGETLIBRARYVIEW_H

#include <QMainWindow>

namespace Ui {
    class widgetLibraryView;
}

class widgetLibraryView : public QMainWindow {
    Q_OBJECT
public:
    widgetLibraryView(QWidget *parent = 0);
    ~widgetLibraryView();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::widgetLibraryView *ui;
};

#endif // WIDGETLIBRARYVIEW_H
