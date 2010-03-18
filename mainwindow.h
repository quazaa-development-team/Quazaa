//
// mainwindow.h
//
// Copyright © Quazaa Development Team, 2009.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QActionGroup>
#include <QSystemTrayIcon>
#include <QHideEvent>
#include "vlcmediaplayer.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void quazaaShutdown();

private:
	Ui::MainWindow *ui;
	QActionGroup *actionGroupMainNavigation;
	QSystemTrayIcon *trayIcon;
	QMenu *trayMenu;
	bool bypassCloseEvent;
	bool event(QEvent *e);
	void changeEvent(QEvent *e);
	vlcMediaPlayer *mediaPlayer;
	void render(QPaintDevice * target, const QPoint & targetOffset = QPoint(), const QRegion & sourceRegion = QRegion(), RenderFlags renderFlags = RenderFlags( DrawWindowBackground | DrawChildren ));
	void render(QPainter * painter, const QPoint & targetOffset = QPoint(), const QRegion & sourceRegion = QRegion(), RenderFlags renderFlags = RenderFlags( DrawWindowBackground | DrawChildren ));

signals:
	void closed();
	void hideMain();
	void showMain();

private slots:
 void on_actionConnectionTest_triggered();
 void on_actionFAQ_triggered();
 void on_actionUsersGuide_triggered();
 void on_actionQuazaaForums_triggered();
 void on_volumeSlider_valueChanged(int value);
 void on_actionMediaOpen_triggered();
	void on_actionMediaRepeat_triggered(bool checked);
	void on_tableWidgetMediaPlaylistTask_doubleClicked(QModelIndex index);
	void on_tabWidgetSearch_tabCloseRequested(int index);
	void on_toolButtonNewSearch_clicked();
	void on_actionAres_triggered(bool checked);
	void on_actionGnutella2_triggered(bool checked);
	void on_actionGnutella1_triggered(bool checked);
	void on_actionEDonkey_triggered(bool checked);
	void on_actionMediaShuffle_triggered(bool checked);
	void on_actionMediaRepeat_toggled(bool checked);
	void on_toolButtonHitMonitorFilter_clicked();
	void on_actionChatSettings_triggered();
	void on_actionNetworkSettings_triggered();
	void on_toolButtonSearchFilter_clicked();
	void on_toolButtonLibrarySearch_clicked();
	void on_toolButtonSecuritySubscribe_clicked();
	void on_actionSecurityAddRule_triggered();
	void on_actionEditMyProfile_triggered();
	void on_actionScheduler_triggered();
	void on_actionQuickstartWizard_triggered();
	void on_actionChooseLanguage_triggered();
	void on_actionChooseSkin_triggered();
	void on_actionImportPartials_triggered();
	void on_actionURLDownload_triggered();
	void on_actionOpenDownloadFolder_triggered();
	void on_actionShares_triggered();
	void on_actionSettings_triggered();
	void on_actionOpenTorrent_triggered();
	void on_actionCreateTorrent_triggered();
	void on_actionAbout_triggered();
	void on_actionExit_triggered();
	void on_actionShowOrHide_triggered();
	void on_actionHitMonitor_triggered();
	void on_actionSearchMonitor_triggered();
	void on_actionPacketDump_triggered();
	void on_actionGraph_triggered();
	void on_actionDiscovery_triggered();
	void on_actionHostCache_triggered();
	void on_actionChat_triggered();
	void on_actionNetwork_triggered();
	void on_actionSecurity_triggered();
	void on_actionTransfers_triggered();
	void on_actionSearch_triggered();
	void on_actionMedia_triggered();
	void on_actionLibrary_triggered();
	void on_actionHome_triggered();
	void icon_activated(QSystemTrayIcon::ActivationReason reason);
	void clearPlaylist();
	void skinChangeEvent();
};

#endif // MAINWINDOW_H
