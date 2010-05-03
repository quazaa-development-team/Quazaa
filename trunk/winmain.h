#ifndef WINMAIN_H
#define WINMAIN_H

#include <QMainWindow>
#include "widgethome.h"
#include "widgetlibrary.h"
#include "widgetmedia.h"
#include "widgetsearch.h"
#include "widgettransfers.h"
#include "widgetsecurity.h"
#include "widgetactivity.h"
#include "widgetchat.h"
#include "widgethostcache.h"
#include "widgetdiscovery.h"
#include "widgetscheduler.h"
#include "widgetgraph.h"
#include "widgetpacketdump.h"
#include "widgetsearchmonitor.h"
#include "widgethitmonitor.h"

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
	WidgetSecurity *pageSecurity;
	WidgetActivity *pageActivity;
	WidgetChat *pageChat;
	WidgetHostCache *pageHostCache;
	WidgetDiscovery *pageDiscovery;
	WidgetScheduler *pageScheduler;
	WidgetGraph *pageGraph;
	WidgetPacketDump *pagePacketDump;
	WidgetSearchMonitor *pageSearchMonitor;
	WidgetHitMonitor *pageHitMonitor;

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
	void on_actionHitMonitor_triggered();
 void on_actionSearchMonitor_triggered();
 void on_actionPacketDump_triggered();
 void on_actionGraph_triggered();
 void on_actionScheduler_triggered();
 void on_actionDiscovery_triggered();
 void on_actionHostCache_triggered();
 void on_actionChat_triggered();
 void on_actionSecurity_triggered();
	void on_actionActivity_triggered();
	void on_actionTransfers_triggered();
	void on_actionSearch_triggered();
	void on_actionMedia_triggered();
    void on_actionLibrary_triggered();
    void on_actionHome_triggered();
};

#endif // WINMAIN_H
