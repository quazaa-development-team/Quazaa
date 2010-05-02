#ifndef WINMAIN_H
#define WINMAIN_H

#include <QMainWindow>
#include "widgethome.h"
#include "widgetlibrary.h"
#include "widgetmedia.h"
#include "widgetsearch.h"
#include "widgettransfers.h"

namespace Ui {
    class WinMain;
}

class WinMain : public QMainWindow {
    Q_OBJECT
public:
    WinMain(QWidget *parent = 0);
    ~WinMain();
	bool event(QEvent *e);
	void quazaaShutdown();
    WidgetHome *pageHome;
	WidgetLibrary *pageLibrary;
    WidgetMedia *pageMedia;
	WidgetSearch *pageSearch;
	WidgetTransfers *pageTransfers;

signals:
	void closed();
	void hideMain();
	void showMain();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WinMain *ui;
	bool bypassCloseEvent;

private slots:
	void on_actionSecurity_triggered();
 void on_actionActivity_triggered();
 void on_actionTransfers_triggered();
 void on_actionSearch_triggered();
	void on_actionMedia_triggered();
    void on_actionLibrary_triggered();
    void on_actionHome_triggered();
};

#endif // WINMAIN_H
