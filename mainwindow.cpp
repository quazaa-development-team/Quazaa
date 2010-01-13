//
// mainwindow.cpp
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commonfunctions.h"
#include "quazaasettings.h"
#include "quazaaglobals.h"
#include "dialogclosetype.h"
#include "dialogsplash.h"
#include "dialogwizard.h"
#include "dialogabout.h"
#include "dialogopentorrent.h"
#include "dialogcreatetorrent.h"
#include "dialogsettings.h"
#include "dialogeditshares.h"
#include "dialogadddownload.h"
#include "dialogdownloadsimport.h"
#include "dialoglanguage.h"
#include "dialogscheduler.h"
#include "dialogprofile.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"
#include "dialoglibrarysearch.h"
#include "dialogfiltersearch.h"
#include "widgetsearchtemplate.h"
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	//Create splash window
	DialogSplash *dlgSplash = new DialogSplash();
	dlgSplash->setWindowFlags(Qt::SplashScreen);
	dlgSplash->show();
	//Load And Set Up User Interface
	dlgSplash->updateProgress(5, tr("Loading User Interface..."));
	ui->setupUi(this);
	bypassCloseEvent = false;
	quazaaSettings.loadSettings();

	// Set up the navigation toolbar
	quazaaSettings.loadWindowSettings(this);
	actionGroupMainNavigation = new QActionGroup(this);
	actionGroupMainNavigation->addAction(ui->actionHome);
	actionGroupMainNavigation->addAction(ui->actionLibrary);
	actionGroupMainNavigation->addAction(ui->actionMedia);
	actionGroupMainNavigation->addAction(ui->actionSearch);
	actionGroupMainNavigation->addAction(ui->actionTransfers);
	actionGroupMainNavigation->addAction(ui->actionSecurity);
	actionGroupMainNavigation->addAction(ui->actionNetwork);
	actionGroupMainNavigation->addAction(ui->actionChat);
	actionGroupMainNavigation->addAction(ui->actionHostCache);
	actionGroupMainNavigation->addAction(ui->actionDiscovery);
	actionGroupMainNavigation->addAction(ui->actionGraph);
	actionGroupMainNavigation->addAction(ui->actionPacketDump);
	actionGroupMainNavigation->addAction(ui->actionSearchMonitor);
	actionGroupMainNavigation->addAction(ui->actionHitMonitor);
	ui->stackedWidgetMain->setCurrentIndex(quazaaSettings.MainWindowActiveTab);
	switch (quazaaSettings.MainWindowActiveTab)
	{
		case 0:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Home.png"));
			ui->labelMainHeaderText->setText(tr("Quazaa Home"));
			ui->actionHome->setChecked(true);
			break;
		case 1:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Library/Library.png"));
			ui->labelMainHeaderText->setText(tr("Library"));
			ui->actionLibrary->setChecked(true);
			break;
		case 2:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Media/Media.png"));
			ui->labelMainHeaderText->setText(tr("Media"));
			ui->actionMedia->setChecked(true);
			break;
		case 3:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Search.png"));
			ui->labelMainHeaderText->setText(tr("Search"));
			ui->actionSearch->setChecked(true);
			break;
		case 4:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Transfers.png"));
			ui->labelMainHeaderText->setText(tr("Transfers"));
			ui->actionTransfers->setChecked(true);
			break;
		case 5:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Security/Security.png"));
			ui->labelMainHeaderText->setText(tr("Security"));
			ui->actionSecurity->setChecked(true);
			break;
		case 6:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Network.png"));
			ui->labelMainHeaderText->setText(tr("Network"));
			ui->actionNetwork->setChecked(true);
			break;
		case 7:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Chat/Chat.png"));
			ui->labelMainHeaderText->setText(tr("Chat"));
			ui->actionChat->setChecked(true);
			break;
		case 8:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HostCache.png"));
			ui->labelMainHeaderText->setText(tr("Host Cache"));
			ui->actionHostCache->setChecked(true);
			break;
		case 9:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Discovery.png"));
			ui->labelMainHeaderText->setText(tr("Discovery"));
			ui->actionDiscovery->setChecked(true);
			break;
		case 10:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Graph.png"));
			ui->labelMainHeaderText->setText(tr("Graph"));
			ui->actionGraph->setChecked(true);
			break;
		case 11:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/PacketDump.png"));
			ui->labelMainHeaderText->setText(tr("Packet Dump"));
			ui->actionPacketDump->setChecked(true);
			break;
		case 12:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/SearchMonitor.png"));
			ui->labelMainHeaderText->setText(tr("Search Monitor"));
			ui->actionSearchMonitor->setChecked(true);
			break;
		case 13:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HitMonitor.png"));
			ui->labelMainHeaderText->setText(tr("Hit Monitor"));
			ui->actionHitMonitor->setChecked(true);
			break;
		default:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Home.png"));
			ui->labelMainHeaderText->setText(tr("Quazaa Home"));
			ui->actionHome->setChecked(true);
			break;
	}
	ui->toolButtonLibraryWebServices->setVisible(false);
	ui->toolButtonChatFriendsHeader->setChecked(quazaaSettings.MainWindowChatFriendsTaskVisible);
	ui->toolButtonChatRoomsHeader->setChecked(quazaaSettings.MainWindowChatRoomsTaskVisible);
	ui->splitterGraph->restoreState(quazaaSettings.MainWindowGraphSplitter);
	ui->lineEditWelcomeSearch->setText(quazaaSettings.MainWindowHomeSearchString);
	ui->toolButtonHomeConnectionTaskHeader->setChecked(quazaaSettings.MainWindowHomeConnectionTaskVisible);
	ui->toolButtonHomeTransfersTaskDownloadsHeader->setChecked(quazaaSettings.MainWindowHomeDownloadsTaskVisible);
	ui->toolButtonHomeLibraryTaskHeader->setChecked(quazaaSettings.MainWindowHomeLibraryTaskVisible);
	ui->lineEditWelcomeSearch->setText(quazaaSettings.MainWindowHomeSearchString);
	ui->toolButtonHomeTorrentsTaskHeader->setChecked(quazaaSettings.MainWindowHomeTorrentsTaskVisible);
	ui->toolButtonHomeTransfersTaskUploadsHeader->setChecked(quazaaSettings.MainWindowHomeUploadsTaskVisible);
	ui->splitterHostCache->restoreState(quazaaSettings.MainWindowHostCacheSplitter);
	ui->splitterLibraryDetails->restoreState(quazaaSettings.MainWindowLibraryDetailsSplitter);
	ui->toolButtonLibraryDetailsToggle->setChecked(quazaaSettings.MainWindowLibraryDetailsVisible);
	ui->tabWidgetLibraryNavigator->setCurrentIndex(quazaaSettings.MainWindowLibraryNavigatorTab);
	ui->splitterLibrary->restoreState(quazaaSettings.MainWindowLibrarySplitter);
	ui->actionMediaPlaylistToggle->setChecked(quazaaSettings.MainWindowMediaPlaylistVisible);
	ui->splitterMediaPlaylist->restoreState(quazaaSettings.MainWindowMediaSplitter);
	ui->splitterNetwork->restoreState(quazaaSettings.MainWindowNetworkSplitter);
	ui->splitterSearchDetails->restoreState(quazaaSettings.MainWindowSearchDetailsSplitter);
	ui->actionSeachDetailsToggle->setChecked(quazaaSettings.MainWindowSearchDetailsVisible);
	ui->toolButtonSearchFiletypeTaskHeader->setChecked(quazaaSettings.MainWindowSearchFileTypeTaskVisible);
	ui->toolButtonSearchNetworksTaskHeader->setChecked(quazaaSettings.MainWindowSearchNetworksTaskVisible);
	ui->toolButtonSearchResultsTaskHeader->setChecked(quazaaSettings.MainWindowSearchResultsTaskVisible);
	ui->actionSearchToggle->setChecked(quazaaSettings.MainWindowSearchSidebarVisible);
	ui->toolButtonSearchTaskHeader->setChecked(quazaaSettings.MainWindowSearchTaskVisible);
	ui->splitterTransfers->restoreState(quazaaSettings.MainWindowTransfersSplitter);

	//Load profile
	dlgSplash->updateProgress(8, tr("Loading Profile..."));
	quazaaSettings.loadProfile();

	//Check if this is Quazaa's first run
	dlgSplash->updateProgress(9, tr("Checking for first run..."));
	if (quazaaSettings.FirstRun())
	{
		dlgSplash->updateProgress(10, tr("Running first run wizard..."));
		quazaaSettings.saveFirstRun(false);
		quazaaSettings.saveSettings();
		quazaaSettings.saveProfile();
		DialogWizard *dlgWizard = new DialogWizard();
		dlgWizard->exec();
	}



	//Load the library
	dlgSplash->updateProgress(15, tr("Loading Library..."));
	QApplication::processEvents();

	//Load the media player
	dlgSplash->updateProgress(20, tr("Loading Media Player..."));
	QApplication::processEvents();

	mediaPlayer = new MediaPlayer(this);
	ui->mediaWidgetLayout->addWidget(mediaPlayer);
	mediaPlayer->setAutoFillBackground(true);
	mediaPlayer->setWindowFlags(mediaPlayer->windowFlags() & ~Qt::FramelessWindowHint);

	connect(ui->actionMediaRewind, SIGNAL(triggered()), mediaPlayer, SLOT(rewind()));
	connect(ui->actionMediaOpen, SIGNAL(triggered()), mediaPlayer, SLOT(openFiles()));
	ui->actionMediaOpen->setMenu(mediaPlayer->fileMenu);

	connect(ui->actionMediaPlay, SIGNAL(triggered()), mediaPlayer, SLOT(playPause()));
	connect(ui->actionMediaStop, SIGNAL(triggered()), mediaPlayer, SLOT(stop()));
	connect(ui->actionMediaNextTrack, SIGNAL(triggered()), mediaPlayer, SLOT(forward()));
	//connect(openButton, SIGNAL(clicked()), this, SLOT(openFiles()));
	connect(ui->actionMediaFullscreen, SIGNAL(triggered()), mediaPlayer, SLOT(setVideoWindowFullscreen()));
	connect(ui->actionMediaSettings, SIGNAL(triggered()), mediaPlayer, SLOT(showSettingsDialog()));
	connect(ui->toolButtonMediaPlaylistAdd, SIGNAL(clicked()), mediaPlayer, SLOT(addFiles()));

	ui->frameMediaWindow->setMinimumSize(100, 100);

	ui->seekSlider->setMediaObject(&mediaPlayer->m_MediaObject);
	ui->volumeSlider->setAudioOutput(&mediaPlayer->m_AudioOutput);

	connect(ui->actionMediaMute, SIGNAL(triggered()), mediaPlayer, SLOT(toggleMute()));
	connect(mediaPlayer, SIGNAL(volumeEnableChanged(bool)), ui->volumeSlider, SLOT(setEnabled(bool)));
	connect(mediaPlayer, SIGNAL(muteStatusChanged(QIcon,QString)), this, SLOT(setMediaMute(QIcon,QString)));
	connect(mediaPlayer, SIGNAL(progressTextChanged(QString)), ui->labelMediaProgress, SLOT(setText(QString)));
	connect(mediaPlayer, SIGNAL(totalTimeTextChanged(QString)), ui->labelMediaTime, SLOT(setText(QString)));
	connect(mediaPlayer, SIGNAL(playlistRowChanged(int)), ui->tableWidgetMediaPlaylistTask, SLOT(selectRow(int)));
	connect(mediaPlayer, SIGNAL(playlistRowCountUpdate()), this, SLOT(playlistRowCountRequest()));
	connect(this, SIGNAL(currentPlaylistRowCount(int)), mediaPlayer, SLOT(setPlaylistRowCount(int)));
	connect(mediaPlayer, SIGNAL(playlistCurrentRowUpdate()), this, SLOT(playlistCurrentRowRequest()));
	connect(this, SIGNAL(currentPlaylistRow(int)), mediaPlayer, SLOT(setPlaylistCurrentRow(int)));
	connect(mediaPlayer, SIGNAL(playlistRemoveRow(int)), ui->tableWidgetMediaPlaylistTask, SLOT(removeRow(int)));
	connect(ui->toolButtonMediaPlaylistRemove, SIGNAL(clicked()), mediaPlayer, SLOT(onPlaylistRemove()));
	connect(ui->toolButtonMediaPlaylistClear, SIGNAL(clicked()), mediaPlayer, SLOT(onPlaylistClear()));
	connect(mediaPlayer, SIGNAL(playlistInsertRow(int)), ui->tableWidgetMediaPlaylistTask, SLOT(insertRow(int)));
	connect(mediaPlayer, SIGNAL(setPlaylistItem(int,int,QTableWidgetItem*)), this, SLOT(setPlaylistItem(int,int,QTableWidgetItem*)));
	connect(mediaPlayer, SIGNAL(playlistSelectedItemsIsEmptyUpdate()), this, SLOT(playlistSelectedItemsIsEmptyRequest()));
	connect(this, SIGNAL(playlistSelectedItemsIsEmpty(bool)), mediaPlayer, SLOT(setPlaylistSelectedItemsIsEmpty(bool)));
	connect(mediaPlayer, SIGNAL(playlistResizeColumnsToContents()), ui->tableWidgetMediaPlaylistTask, SLOT(resizeColumnsToContents()));
	connect(mediaPlayer, SIGNAL(setPlaylistStretchLastSection(bool)), this, SLOT(setPlaylistStretchLastSection(bool)));
	connect(mediaPlayer, SIGNAL(playButtonEnableChanged(bool)), ui->actionMediaPlay, SLOT(setEnabled(bool)));
	connect(mediaPlayer, SIGNAL(playStatusChanged(QIcon,QString)), this, SLOT(setMediaPlay(QIcon,QString)));
	connect(mediaPlayer, SIGNAL(stopButtonEnableChanged(bool)), ui->actionMediaStop, SLOT(setEnabled(bool)));
	connect(mediaPlayer, SIGNAL(rewindButtonEnableChanged(bool)), ui->actionMediaRewind, SLOT(setEnabled(bool)));
	connect(mediaPlayer, SIGNAL(forwardButtonEnableChanged(bool)), ui->actionMediaNextTrack, SLOT(setEnabled(bool)));
	connect(mediaPlayer, SIGNAL(fullscreenEnableChanged(bool)), ui->actionMediaFullscreen, SLOT(setEnabled(bool)));
	connect(mediaPlayer, SIGNAL(playlistClear()), this, SLOT(clearPlaylist()));
	connect(ui->tableWidgetMediaPlaylistTask, SIGNAL(doubleClicked(QModelIndex)), mediaPlayer, SLOT(playlistDoubleClicked(QModelIndex)));
	connect(ui->toolButtonMediaPlaylistOpenPlaylist, SIGNAL(clicked()), mediaPlayer, SLOT(openPlaylist()));
	connect(ui->toolButtonMediaPlaylistSavePlaylist, SIGNAL(clicked()), mediaPlayer, SLOT(savePlaylist()));
	connect(ui->toolButtonMediaPlaylistAddPlaylist, SIGNAL(clicked()), mediaPlayer, SLOT(addPlaylist()));
	ui->actionMediaRepeat->setChecked(quazaaSettings.MediaRepeat);
	ui->actionMediaShuffle->setChecked(quazaaSettings.MediaShuffle);
	mediaPlayer->m_AudioOutput.setMuted(quazaaSettings.MediaMute);
	mediaPlayer->m_AudioOutput.setVolume(quazaaSettings.MediaVolume);

	//Load the networks
	dlgSplash->updateProgress(25, tr("Loading Networks..."));
	QApplication::processEvents();
	ui->actionAres->setChecked(quazaaSettings.AresEnable);
	ui->actionEDonkey->setChecked(quazaaSettings.EDonkeyEnable);
	ui->actionGnutella1->setChecked(quazaaSettings.Gnutella1Enable);
	ui->actionGnutella2->setChecked(quazaaSettings.Gnutella2Enable);

	// Tray icon construction
	dlgSplash->updateProgress(95, tr("Loading Tray Icon..."));
	QApplication::processEvents();
	// Create the system tray right click menu.
	trayMenu = new QMenu(this);
	trayMenu->addAction(ui->actionShowOrHide);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionNewSearch);
	trayMenu->addAction(ui->actionURLDownload);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionMediaPlay);
	trayMenu->addAction(ui->actionMediaStop);
	trayMenu->addAction(ui->actionMediaOpen);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionMediaRewind);
	trayMenu->addAction(ui->actionMediaNextTrack);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionExitAfterTransfers);
	trayMenu->addAction(ui->actionExit);
	// Create the system tray icon
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(":/Resource/Quazaa.png"));
	trayIcon->setToolTip(tr("Quazaa"));
	trayIcon->setContextMenu(trayMenu);
	// Connect an event handler to the tray icon so we can handle mouse events
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(icon_activated(QSystemTrayIcon::ActivationReason)));
	trayIcon->show();
	dlgSplash->updateProgress(100, tr("Welcome to Quazaa!"));
	QApplication::processEvents();
	dlgSplash->close();
}

MainWindow::~MainWindow()
{
	delete ui;
}

bool MainWindow::event(QEvent *e)
{
	QMainWindow::event(e);
	if (e->type() == QEvent::Close)
	{
		if(!bypassCloseEvent)
		{
			if (quazaaSettings.BasicCloseMode == 0)
			{
				DialogCloseType *dlgCloseType = new DialogCloseType(this);
				dlgCloseType->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
				dlgCloseType->exec();
			}

			switch (quazaaSettings.BasicCloseMode)
			{
				case 1:
					quazaaShutdown();
					return false;
				case 2:
					hide();
					e->ignore();
					return true;
				case 3:
					quazaaShutdown();
					return false;
				default:
					quazaaShutdown();
					return false;
			}
		} else {
			quazaaShutdown();
			return false;
		}
	}

	return false;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::quazaaShutdown()
{
	DialogSplash *dlgSplash = new DialogSplash;
	dlgSplash->setWindowFlags(Qt::SplashScreen);
	dlgSplash->show();

	dlgSplash->updateProgress(5, tr("Saving Settings..."));
	quazaaSettings.saveSettings();

	dlgSplash->updateProgress(10, tr("Saving UI..."));
	quazaaSettings.MainWindowChatFriendsTaskVisible = ui->toolButtonChatFriendsHeader->isChecked();
	quazaaSettings.MainWindowChatRoomsTaskVisible = ui->toolButtonChatRoomsHeader->isChecked();
	quazaaSettings.MainWindowGraphSplitter = ui->splitterGraph->saveState();
	quazaaSettings.MainWindowHomeSearchString = ui->lineEditWelcomeSearch->text();
	quazaaSettings.MainWindowHomeConnectionTaskVisible = ui->toolButtonHomeConnectionTaskHeader->isChecked();
	quazaaSettings.MainWindowHomeDownloadsTaskVisible = ui->toolButtonHomeTransfersTaskDownloadsHeader->isChecked();
	quazaaSettings.MainWindowHomeLibraryTaskVisible = ui->toolButtonHomeLibraryTaskHeader->isChecked();
	quazaaSettings.MainWindowHomeSearchString = ui->lineEditWelcomeSearch->text();
	quazaaSettings.MainWindowHomeTorrentsTaskVisible = ui->toolButtonHomeTorrentsTaskHeader->isChecked();
	quazaaSettings.MainWindowHomeUploadsTaskVisible = ui->toolButtonHomeTransfersTaskUploadsHeader->isChecked();
	quazaaSettings.MainWindowHostCacheSplitter = ui->splitterHostCache->saveState();
	quazaaSettings.MainWindowLibraryDetailsSplitter = ui->splitterLibraryDetails->saveState();
	quazaaSettings.MainWindowLibraryDetailsVisible = ui->toolButtonLibraryDetailsToggle->isChecked();
	quazaaSettings.MainWindowLibraryNavigatorTab = ui->tabWidgetLibraryNavigator->currentIndex();
	quazaaSettings.MainWindowLibrarySplitter = ui->splitterLibrary->saveState();
	quazaaSettings.MainWindowMediaPlaylistVisible = ui->actionMediaPlaylistToggle->isChecked();
	quazaaSettings.MainWindowMediaSplitter = ui->splitterMediaPlaylist->saveState();
	quazaaSettings.MainWindowNetworkSplitter = ui->splitterNetwork->saveState();
	quazaaSettings.MainWindowSearchDetailsSplitter = ui->splitterSearchDetails->saveState();
	quazaaSettings.MainWindowSearchDetailsVisible = ui->actionSeachDetailsToggle->isChecked();
	quazaaSettings.MainWindowSearchFileTypeTaskVisible = ui->toolButtonSearchFiletypeTaskHeader->isChecked();
	quazaaSettings.MainWindowSearchNetworksTaskVisible = ui->toolButtonSearchNetworksTaskHeader->isChecked();
	quazaaSettings.MainWindowSearchResultsTaskVisible = ui->toolButtonSearchResultsTaskHeader->isChecked();
	quazaaSettings.MainWindowSearchSidebarVisible = ui->actionSearchToggle->isChecked();
	quazaaSettings.MainWindowSearchTaskVisible = ui->toolButtonSearchTaskHeader->isChecked();
	quazaaSettings.MainWindowTransfersSplitter = ui->splitterTransfers->saveState();
	quazaaSettings.saveWindowSettings(this);

	dlgSplash->updateProgress(15, tr("Removing Tray Icon..."));
	trayIcon->~QSystemTrayIcon();

	dlgSplash->close();
	emit closed();
}

void MainWindow::on_actionHome_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Home.png"));
	ui->labelMainHeaderText->setText(tr("Quazaa Home"));
	ui->stackedWidgetMain->setCurrentIndex(0);
	quazaaSettings.MainWindowActiveTab = 0;
}

void MainWindow::on_actionLibrary_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Library/Library.png"));
	ui->labelMainHeaderText->setText(tr("Library"));
	ui->stackedWidgetMain->setCurrentIndex(1);
	quazaaSettings.MainWindowActiveTab = 1;
}

void MainWindow::on_actionMedia_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Media/Media.png"));
	ui->labelMainHeaderText->setText(tr("Media"));
	ui->stackedWidgetMain->setCurrentIndex(2);
	quazaaSettings.MainWindowActiveTab = 2;
}

void MainWindow::on_actionSearch_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Search.png"));
	ui->labelMainHeaderText->setText(tr("Search"));
	ui->stackedWidgetMain->setCurrentIndex(3);
	quazaaSettings.MainWindowActiveTab = 3;
}

void MainWindow::on_actionTransfers_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Transfers.png"));
	ui->labelMainHeaderText->setText(tr("Transfers"));
	ui->stackedWidgetMain->setCurrentIndex(4);
	quazaaSettings.MainWindowActiveTab = 4;
}

void MainWindow::on_actionSecurity_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Security/Security.png"));
	ui->labelMainHeaderText->setText(tr("Security"));
	ui->stackedWidgetMain->setCurrentIndex(5);
	quazaaSettings.MainWindowActiveTab = 5;
}

void MainWindow::on_actionNetwork_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Network.png"));
	ui->labelMainHeaderText->setText(tr("Network"));
	ui->stackedWidgetMain->setCurrentIndex(6);
	quazaaSettings.MainWindowActiveTab = 6;
}

void MainWindow::on_actionChat_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Chat/Chat.png"));
	ui->labelMainHeaderText->setText(tr("Chat"));
	ui->stackedWidgetMain->setCurrentIndex(7);
	quazaaSettings.MainWindowActiveTab = 7;
}

void MainWindow::on_actionHostCache_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HostCache.png"));
	ui->labelMainHeaderText->setText(tr("Host Cache"));
	ui->stackedWidgetMain->setCurrentIndex(8);
	quazaaSettings.MainWindowActiveTab = 8;
}

void MainWindow::on_actionDiscovery_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Discovery.png"));
	ui->labelMainHeaderText->setText(tr("Discovery"));
	ui->stackedWidgetMain->setCurrentIndex(9);
	quazaaSettings.MainWindowActiveTab = 9;
}

void MainWindow::on_actionGraph_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Graph.png"));
	ui->labelMainHeaderText->setText(tr("Graph"));
	ui->stackedWidgetMain->setCurrentIndex(10);
	quazaaSettings.MainWindowActiveTab = 10;
}

void MainWindow::on_actionPacketDump_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/PacketDump.png"));
	ui->labelMainHeaderText->setText(tr("Packet Dump"));
	ui->stackedWidgetMain->setCurrentIndex(11);
	quazaaSettings.MainWindowActiveTab = 11;
}

void MainWindow::on_actionSearchMonitor_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/SearchMonitor.png"));
	ui->labelMainHeaderText->setText(tr("Search Monitor"));
	ui->stackedWidgetMain->setCurrentIndex(12);
	quazaaSettings.MainWindowActiveTab = 12;
}

void MainWindow::on_actionHitMonitor_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HitMonitor.png"));
	ui->labelMainHeaderText->setText(tr("Hit Monitor"));
	ui->stackedWidgetMain->setCurrentIndex(13);
	quazaaSettings.MainWindowActiveTab = 13;
}

void MainWindow::icon_activated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Unknown:
		break;
	case QSystemTrayIcon::DoubleClick:
		ui->actionShowOrHide->trigger();
		break;
	case QSystemTrayIcon::Trigger:
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		break;
	}
}

void MainWindow::on_actionShowOrHide_triggered()
{
	if (isVisible())
	{
		emit hideMain();
	} else {
		emit showMain();
	}
}

void MainWindow::on_actionExit_triggered()
{
	bypassCloseEvent = true;
	close();
}

void MainWindow::on_actionAbout_triggered()
{
	DialogAbout *dlgAbout = new DialogAbout;
	dlgAbout->show();
}

void MainWindow::on_actionCreateTorrent_triggered()
{
	DialogCreateTorrent *dlgCreateTorrent = new DialogCreateTorrent;
	dlgCreateTorrent->show();
}

void MainWindow::on_actionOpenTorrent_triggered()
{
	DialogOpenTorrent *dlgOpenTorrent = new DialogOpenTorrent;
	dlgOpenTorrent->show();
}

void MainWindow::on_actionSettings_triggered()
{
	DialogSettings *dlgSettings = new DialogSettings;
	dlgSettings->show();
}

void MainWindow::on_actionShares_triggered()
{
	DialogEditShares *dlgEditShares = new DialogEditShares;
	dlgEditShares->show();
}

void MainWindow::on_actionOpenDownloadFolder_triggered()
{
	Functions.FolderOpen(quazaaSettings.DownloadsCompletePath);
}

void MainWindow::on_actionURLDownload_triggered()
{
	DialogAddDownload *dlgAddDownload = new DialogAddDownload;
	dlgAddDownload->show();
}

void MainWindow::on_actionImportPartials_triggered()
{
	DialogDownloadsImport *dlgDownloadsImport = new DialogDownloadsImport;
	dlgDownloadsImport->show();
}

void MainWindow::on_actionChooseSkin_triggered()
{
	DialogSettings *dlgSettings = new DialogSettings;
	dlgSettings->switchSettingsPage(3);
	dlgSettings->show();
}

void MainWindow::on_actionChooseLanguage_triggered()
{
	DialogLanguage *dlgLanguage = new DialogLanguage;
	dlgLanguage->show();
}

void MainWindow::on_actionQuickstartWizard_triggered()
{
	DialogWizard *dlgWizard = new DialogWizard();
	dlgWizard->show();
}

void MainWindow::on_actionScheduler_triggered()
{
	DialogScheduler *dlgScheduler = new DialogScheduler;
	dlgScheduler->show();
}

void MainWindow::on_actionEditMyProfile_triggered()
{
	DialogProfile *dlgProfile = new DialogProfile;
	dlgProfile->show();
}

void MainWindow::on_actionSecurityAddRule_triggered()
{
	DialogAddRule *dlgAddRule = new DialogAddRule;
	dlgAddRule->show();
}

void MainWindow::on_toolButtonSecuritySubscribe_clicked()
{
	DialogSecuritySubscriptions *dlgSecuritySubscriptions = new DialogSecuritySubscriptions;
	dlgSecuritySubscriptions->show();
}

void MainWindow::on_toolButtonLibrarySearch_clicked()
{
	DialogLibrarySearch *dlgLibrarySearch = new DialogLibrarySearch;
	dlgLibrarySearch->show();
}

void MainWindow::on_toolButtonSearchFilter_clicked()
{
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch;
	dlgFilterSearch->show();
}

void MainWindow::on_actionNetworkSettings_triggered()
{
	DialogSettings *dlgSettings = new DialogSettings;
	dlgSettings->switchSettingsPage(20);
	dlgSettings->show();
}

void MainWindow::on_actionChatSettings_triggered()
{
	DialogSettings *dlgSettings = new DialogSettings;
	dlgSettings->switchSettingsPage(7);
	dlgSettings->show();
}

void MainWindow::on_toolButtonHitMonitorFilter_clicked()
{
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch;
	dlgFilterSearch->show();
}

void MainWindow::on_actionMediaRepeat_toggled(bool checked)
{
	quazaaSettings.MediaRepeat = checked;
}

void MainWindow::on_actionMediaShuffle_triggered(bool checked)
{
	quazaaSettings.MediaShuffle = checked;
}

void MainWindow::on_actionEDonkey_triggered(bool checked)
{
	quazaaSettings.EDonkeyEnable = checked;
}

void MainWindow::on_actionGnutella1_triggered(bool checked)
{
	quazaaSettings.Gnutella1Enable = checked;
}

void MainWindow::on_actionGnutella2_triggered(bool checked)
{
	quazaaSettings.Gnutella2Enable = checked;
}

void MainWindow::on_actionAres_triggered(bool checked)
{
	quazaaSettings.AresEnable = checked;
}

void MainWindow::on_toolButtonNewSearch_clicked()
{
	WidgetSearchTemplate *tabNewSearch = new WidgetSearchTemplate();
	ui->tabWidgetSearch->addTab(tabNewSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(ui->tabWidgetSearch->count());
	ui->tabWidgetSearch->setTabsClosable(true);
}

void MainWindow::on_tabWidgetSearch_tabCloseRequested(int index)
{
	ui->tabWidgetSearch->removeTab(index);
	if (ui->tabWidgetSearch->count() == 1)
	{
		ui->tabWidgetSearch->setTabsClosable(false);
	}
}


void MainWindow::setMediaPlay(QIcon icon, QString text)
{
	ui->actionMediaPlay->setIcon(icon);
	ui->actionMediaPlay->setText(text);
	ui->actionMediaPlay->setToolTip(text);
}

void MainWindow::setMediaMute(QIcon icon, QString text)
{
	ui->actionMediaMute->setIcon(icon);
	ui->actionMediaMute->setText(text);
	ui->actionMediaMute->setToolTip(text);
}

void MainWindow::on_tableWidgetMediaPlaylistTask_doubleClicked(QModelIndex index)
{
	mediaPlayer->playOnSwitch(true, mediaPlayer->sources.at(ui->tableWidgetMediaPlaylistTask->currentRow()));
}

void MainWindow::on_actionMediaRepeat_triggered(bool checked)
{
	quazaaSettings.MediaRepeat = checked;
}

void MainWindow::clearPlaylist()
{
	while (ui->tableWidgetMediaPlaylistTask->rowCount() != 0)
	{
		ui->tableWidgetMediaPlaylistTask->removeRow(0);
	}
}

void MainWindow::playlistRowCountRequest()
{
	emit currentPlaylistRowCount(ui->tableWidgetMediaPlaylistTask->rowCount());
}

void MainWindow::playlistCurrentRowRequest()
{
	emit currentPlaylistRow(ui->tableWidgetMediaPlaylistTask->currentRow());
}

void MainWindow::playlistSelectedItemsIsEmptyRequest()
{
	emit playlistSelectedItemsIsEmpty(ui->tableWidgetMediaPlaylistTask->selectedItems().isEmpty());
}

void MainWindow::setPlaylistItem(int row, int column, QTableWidgetItem *item)
{
	ui->tableWidgetMediaPlaylistTask->setItem(row, column, item);
}

void MainWindow::setPlaylistStretchLastSection(bool stretchLastSection)
{
	ui->tableWidgetMediaPlaylistTask->horizontalHeader()->setStretchLastSection(stretchLastSection);
}
