//
// winmain.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef WINMAIN_H
#define WINMAIN_H

#include <QMainWindow>
#include <QActionGroup>
#include <QSystemTrayIcon>
#include <QHideEvent>

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

#include "neighbourstablemodel.h"

namespace Ui
{
	class WinMain;
}

class WinMain : public QMainWindow
{
	Q_OBJECT
public:
	WinMain(QWidget* parent = 0);
	~WinMain();
	void loadTrayIcon();
	bool event(QEvent* e);

	bool interfaceLoaded;
	QTimer* neighboursRefresher;
	CNeighboursTableModel* neighboursList;
	QActionGroup* actionGroupMainNavigation;
	QLabel* labelBandwidthTotals;
	QLabel* labelFirewallStatus;

	QSystemTrayIcon* trayIcon;
	QMenu* trayMenu;
	WidgetHome* pageHome;
	WidgetLibrary* pageLibrary;
	WidgetMedia* pageMedia;
	WidgetSearch* pageSearch;
	WidgetTransfers* pageTransfers;
	WidgetSecurity* pageSecurity;
	WidgetActivity* pageActivity;
	WidgetChat* pageChat;
	WidgetHostCache* pageHostCache;
	WidgetDiscovery* pageDiscovery;
	WidgetScheduler* pageScheduler;
	WidgetGraph* pageGraph;
	WidgetPacketDump* pagePacketDump;
	WidgetSearchMonitor* pageSearchMonitor;
	WidgetHitMonitor* pageHitMonitor;

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WinMain* ui;
	bool bypassCloseEvent;
	QString tcpFirewalled;
	QString udpFirewalled;

private slots:
	void on_actionConnectTo_triggered();
	void on_actionAres_triggered(bool checked);
	void on_actionGnutella2_triggered(bool checked);
	void on_actionEDonkey_triggered(bool checked);
	void on_actionDisconnect_triggered();
	void on_actionConnect_triggered();
	void on_actionNewSearch_triggered();
	void on_actionEditMyProfile_triggered();
	void on_actionQuazaaForums_triggered();
	void on_actionDonate_triggered();
	void on_actionCheckForNewVersion_triggered();
	void on_actionConnectionTest_triggered();
	void on_actionFAQ_triggered();
	void on_actionUsersGuide_triggered();
	void on_actionQuickstartWizard_triggered();
	void on_actionChooseLanguage_triggered();
	void on_actionChooseSkin_triggered();
	void on_actionImportPartials_triggered();
	void on_actionURLDownload_triggered();
	void on_actionOpenDownloadFolder_triggered();
	void on_actionShares_triggered();
	void on_actionOpenTorrent_triggered();
	void on_actionSeedTorrent_triggered();
	void on_actionCreateTorrent_triggered();
	void on_actionSettings_triggered();
	void on_actionAbout_triggered();
	void on_actionExit_triggered();
	void on_actionShowOrHide_triggered();
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
	void quazaaShutdown();
	void quazaaStartup();
	void icon_activated(QSystemTrayIcon::ActivationReason reason);
	 
	void startNewSearch(QString* searchString);
	void updateStatusBar();
};

#endif // WINMAIN_H
