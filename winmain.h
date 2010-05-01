#ifndef WINMAIN_H
#define WINMAIN_H

#include <QMainWindow>
#include "widgethome.h"
#include "widgetlibrary.h"
#include "widgetlibraryview.h"
#include "widgetmedia.h"
#include "widgetsearchtemplate.h"

namespace Ui {
    class WinMain;
}

class WinMain : public QMainWindow {
    Q_OBJECT
public:
    WinMain(QWidget *parent = 0);
    ~WinMain();
    WidgetHome *pageHome;
    WidgetLibrary *pageLibrary;
    WidgetLibraryView *panelLibraryView;
    WidgetMedia *pageMedia;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WinMain *ui;

private slots:
    void on_actionMedia_triggered();
    void on_actionLibrary_triggered();
    void on_actionHome_triggered();
};

#endif // WINMAIN_H
