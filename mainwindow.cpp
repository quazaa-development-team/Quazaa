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
#include "QSkinDialog/qskinsettings.h"
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	//Initialize Skin Settings
	quazaaSettings.loadSkinSettings();
	skinSettings.loadSkin(quazaaSettings.SkinFile);
	skinChangeEvent();

	//Initialize Settings
	quazaaSettings.loadSettings();
	interfaceLoaded = false;

	//Initialize multilanguage support
	quazaaSettings.loadLanguageSettings();

	if (quazaaSettings.FirstRun())
	{
		QSkinDialog *skinDlgLanguage = new QSkinDialog(false, true, false, this);
		DialogLanguage *dlgLanguage = new DialogLanguage();
		skinDlgLanguage->addChildWidget(dlgLanguage);
		QObject::connect(dlgLanguage, SIGNAL(closed()), skinDlgLanguage, SLOT(close()));
		skinDlgLanguage->exec();
	}

	quazaaGlobals.translator.load(quazaaSettings.LanguageFile);
	qApp->installTranslator(&quazaaGlobals.translator);

	qApp->processEvents();

	//Create splash window
	DialogSplash *dlgSplash = new DialogSplash();
	dlgSplash->show();
	//Load And Set Up User Interface
	dlgSplash->updateProgress(5, tr("Loading User Interface..."));
	bypassCloseEvent = false;
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));

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
	actionGroupMainNavigation->addAction(ui->actionScheduler);
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
			ui->frameMainHeader->setStyleSheet(skinSettings.homeHeader);
			ui->actionHome->setChecked(true);
			break;
		case 1:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Library/Library.png"));
			ui->labelMainHeaderText->setText(tr("Library"));
			ui->frameMainHeader->setStyleSheet(skinSettings.libraryHeader);
			ui->actionLibrary->setChecked(true);
			break;
		case 2:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Media/Media.png"));
			ui->labelMainHeaderText->setText(tr("Media"));
			ui->frameMainHeader->setStyleSheet(skinSettings.mediaHeader);
			ui->actionMedia->setChecked(true);
			break;
		case 3:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Search.png"));
			ui->labelMainHeaderText->setText(tr("Search"));
			ui->frameMainHeader->setStyleSheet(skinSettings.searchHeader);
			ui->actionSearch->setChecked(true);
			break;
		case 4:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Transfers.png"));
			ui->labelMainHeaderText->setText(tr("Transfers"));
			ui->frameMainHeader->setStyleSheet(skinSettings.transfersHeader);
			ui->actionTransfers->setChecked(true);
			break;
		case 5:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Security/Security.png"));
			ui->labelMainHeaderText->setText(tr("Security"));
			ui->frameMainHeader->setStyleSheet(skinSettings.securityHeader);
			ui->actionSecurity->setChecked(true);
			break;
		case 6:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Network.png"));
			ui->labelMainHeaderText->setText(tr("Network"));
			ui->frameMainHeader->setStyleSheet(skinSettings.networkHeader);
			ui->actionNetwork->setChecked(true);
			break;
		case 7:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Chat/Chat.png"));
			ui->labelMainHeaderText->setText(tr("Chat"));
			ui->frameMainHeader->setStyleSheet(skinSettings.chatHeader);
			ui->actionChat->setChecked(true);
			break;
		case 8:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HostCache.png"));
			ui->labelMainHeaderText->setText(tr("Host Cache"));
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			ui->actionHostCache->setChecked(true);
			break;
		case 9:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Discovery.png"));
			ui->labelMainHeaderText->setText(tr("Discovery"));
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			ui->actionDiscovery->setChecked(true);
			break;
		case 10:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Scheduler.png"));
			ui->labelMainHeaderText->setText(tr("Scheduler"));
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			ui->actionScheduler->setChecked(true);
			break;
		case 11:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Graph.png"));
			ui->labelMainHeaderText->setText(tr("Graph"));
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			ui->actionGraph->setChecked(true);
			break;
		case 12:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/PacketDump.png"));
			ui->labelMainHeaderText->setText(tr("Packet Dump"));
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			ui->actionPacketDump->setChecked(true);
			break;
		case 13:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/SearchMonitor.png"));
			ui->labelMainHeaderText->setText(tr("Search Monitor"));
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			ui->actionSearchMonitor->setChecked(true);
			break;
		case 14:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HitMonitor.png"));
			ui->labelMainHeaderText->setText(tr("Hit Monitor"));
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			ui->actionHitMonitor->setChecked(true);
			break;
		default:
			ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Home.png"));
			ui->labelMainHeaderText->setText(tr("Quazaa Home"));
			ui->frameMainHeader->setStyleSheet(skinSettings.homeHeader);
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
	interfaceLoaded = true;

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
		QSkinDialog *dlgSkinWizard = new QSkinDialog(false, true, false, this);
		DialogWizard *dlgWizard = new DialogWizard();

		dlgSkinWizard->addChildWidget(dlgWizard);

		connect(dlgWizard, SIGNAL(closed()), dlgSkinWizard, SLOT(close()));
		dlgSkinWizard->exec();
	}

	//Load the library
	dlgSplash->updateProgress(15, tr("Loading Library..."));
	qApp->processEvents();

	//Load the media player
	dlgSplash->updateProgress(20, tr("Loading Media Player..."));
	qApp->processEvents();

	mediaPlayer = new vlcMediaPlayer(ui->seekSlider, ui->volumeSlider, ui->frameMediaWindow, ui->tableWidgetMediaPlaylistTask,
									 ui->actionMediaPlay, ui->actionMediaStop, ui->actionMediaRewind, ui->actionMediaNextTrack,
									 ui->actionMediaOpen, ui->actionMediaRepeat , ui->actionMediaShuffle,
									 ui->actionMediaFullscreen, ui->actionMediaSettings, ui->actionMediaMute,
									 ui->toolButtonMediaPlaylistAdd, ui->toolButtonMediaPlaylistRemove,
									 ui->toolButtonMediaPlaylistClear,ui->toolButtonMediaPlaylistOpenPlaylist,
									 ui->toolButtonMediaPlaylistAddPlaylist,
									 ui->toolButtonMediaPlaylistSavePlaylist, this);
	ui->actionMediaRepeat->setChecked(quazaaSettings.MediaRepeat);
	ui->actionMediaShuffle->setChecked(quazaaSettings.MediaShuffle);

	//Load the networks
	dlgSplash->updateProgress(25, tr("Loading Networks..."));
	qApp->processEvents();
	ui->actionAres->setChecked(quazaaSettings.AresEnable);
	ui->actionEDonkey->setChecked(quazaaSettings.EDonkeyEnable);
	ui->actionGnutella1->setChecked(quazaaSettings.Gnutella1Enable);
	ui->actionGnutella2->setChecked(quazaaSettings.Gnutella2Enable);

	// Tray icon construction
	dlgSplash->updateProgress(95, tr("Loading Tray Icon..."));
	qApp->processEvents();
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
	qApp->processEvents();
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

				QSkinDialog *dlgSkinCloseType = new QSkinDialog(false, false, false);
				DialogCloseType *dlgCloseType = new DialogCloseType(this);

				dlgSkinCloseType->addChildWidget(dlgCloseType);

				connect(dlgCloseType, SIGNAL(closed()), dlgSkinCloseType, SLOT(close()));
				dlgSkinCloseType->exec();
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
		ui->toolBarMain->setStyleSheet(skinSettings.navigationToolbar);
		if (interfaceLoaded)
		{
			switch (ui->stackedWidgetMain->currentIndex())
			{
				case 0:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Home.png"));
					ui->labelMainHeaderText->setText(tr("Quazaa Home"));
					ui->frameMainHeader->setStyleSheet(skinSettings.homeHeader);
					ui->actionHome->setChecked(true);
					break;
				case 1:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Library/Library.png"));
					ui->labelMainHeaderText->setText(tr("Library"));
					ui->frameMainHeader->setStyleSheet(skinSettings.libraryHeader);
					ui->actionLibrary->setChecked(true);
					break;
				case 2:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Media/Media.png"));
					ui->labelMainHeaderText->setText(tr("Media"));
					ui->frameMainHeader->setStyleSheet(skinSettings.mediaHeader);
					ui->actionMedia->setChecked(true);
					break;
				case 3:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Search.png"));
					ui->labelMainHeaderText->setText(tr("Search"));
					ui->frameMainHeader->setStyleSheet(skinSettings.searchHeader);
					ui->actionSearch->setChecked(true);
					break;
				case 4:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Transfers.png"));
					ui->labelMainHeaderText->setText(tr("Transfers"));
					ui->frameMainHeader->setStyleSheet(skinSettings.transfersHeader);
					ui->actionTransfers->setChecked(true);
					break;
				case 5:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Security/Security.png"));
					ui->labelMainHeaderText->setText(tr("Security"));
					ui->frameMainHeader->setStyleSheet(skinSettings.securityHeader);
					ui->actionSecurity->setChecked(true);
					break;
				case 6:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Network.png"));
					ui->labelMainHeaderText->setText(tr("Network"));
					ui->frameMainHeader->setStyleSheet(skinSettings.networkHeader);
					ui->actionNetwork->setChecked(true);
					break;
				case 7:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Chat/Chat.png"));
					ui->labelMainHeaderText->setText(tr("Chat"));
					ui->frameMainHeader->setStyleSheet(skinSettings.chatHeader);
					ui->actionChat->setChecked(true);
					break;
				case 8:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HostCache.png"));
					ui->labelMainHeaderText->setText(tr("Host Cache"));
					ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
					ui->actionHostCache->setChecked(true);
					break;
				case 9:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Discovery.png"));
					ui->labelMainHeaderText->setText(tr("Discovery"));
					ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
					ui->actionDiscovery->setChecked(true);
					break;
				case 10:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Scheduler.png"));
					ui->labelMainHeaderText->setText(tr("Scheduler"));
					ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
					ui->actionScheduler->setChecked(true);
					break;
				case 11:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Graph.png"));
					ui->labelMainHeaderText->setText(tr("Graph"));
					ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
					ui->actionGraph->setChecked(true);
					break;
				case 12:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/PacketDump.png"));
					ui->labelMainHeaderText->setText(tr("Packet Dump"));
					ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
					ui->actionPacketDump->setChecked(true);
					break;
				case 13:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/SearchMonitor.png"));
					ui->labelMainHeaderText->setText(tr("Search Monitor"));
					ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
					ui->actionSearchMonitor->setChecked(true);
					break;
				case 14:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HitMonitor.png"));
					ui->labelMainHeaderText->setText(tr("Hit Monitor"));
					ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
					ui->actionHitMonitor->setChecked(true);
					break;
				default:
					ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Home.png"));
					ui->labelMainHeaderText->setText(tr("Quazaa Home"));
					ui->frameMainHeader->setStyleSheet(skinSettings.homeHeader);
					ui->actionHome->setChecked(true);
					break;
			}
		}
		break;
	default:
		break;
	}
}

void MainWindow::quazaaShutdown()
{
	DialogSplash *dlgSplash = new DialogSplash;
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
	ui->frameMainHeader->setStyleSheet(skinSettings.homeHeader);
	ui->stackedWidgetMain->setCurrentIndex(0);
	quazaaSettings.MainWindowActiveTab = 0;
}

void MainWindow::on_actionLibrary_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Library/Library.png"));
	ui->labelMainHeaderText->setText(tr("Library"));
	ui->frameMainHeader->setStyleSheet(skinSettings.libraryHeader);
	ui->stackedWidgetMain->setCurrentIndex(1);
	quazaaSettings.MainWindowActiveTab = 1;
}

void MainWindow::on_actionMedia_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Media/Media.png"));
	ui->labelMainHeaderText->setText(tr("Media"));
	ui->frameMainHeader->setStyleSheet(skinSettings.mediaHeader);
	ui->stackedWidgetMain->setCurrentIndex(2);
	quazaaSettings.MainWindowActiveTab = 2;
}

void MainWindow::on_actionSearch_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Search.png"));
	ui->labelMainHeaderText->setText(tr("Search"));
	ui->frameMainHeader->setStyleSheet(skinSettings.searchHeader);
	ui->stackedWidgetMain->setCurrentIndex(3);
	quazaaSettings.MainWindowActiveTab = 3;
}

void MainWindow::on_actionTransfers_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Transfers.png"));
	ui->labelMainHeaderText->setText(tr("Transfers"));
	ui->frameMainHeader->setStyleSheet(skinSettings.transfersHeader);
	ui->stackedWidgetMain->setCurrentIndex(4);
	quazaaSettings.MainWindowActiveTab = 4;
}

void MainWindow::on_actionSecurity_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Security/Security.png"));
	ui->labelMainHeaderText->setText(tr("Security"));
	ui->frameMainHeader->setStyleSheet(skinSettings.securityHeader);
	ui->stackedWidgetMain->setCurrentIndex(5);
	quazaaSettings.MainWindowActiveTab = 5;
}

void MainWindow::on_actionNetwork_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Network.png"));
	ui->labelMainHeaderText->setText(tr("Network"));
	ui->frameMainHeader->setStyleSheet(skinSettings.networkHeader);
	ui->stackedWidgetMain->setCurrentIndex(6);
	quazaaSettings.MainWindowActiveTab = 6;
}

void MainWindow::on_actionChat_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Chat/Chat.png"));
	ui->labelMainHeaderText->setText(tr("Chat"));
	ui->frameMainHeader->setStyleSheet(skinSettings.chatHeader);
	ui->stackedWidgetMain->setCurrentIndex(7);
	quazaaSettings.MainWindowActiveTab = 7;
}

void MainWindow::on_actionHostCache_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HostCache.png"));
	ui->labelMainHeaderText->setText(tr("Host Cache"));
	ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
	ui->stackedWidgetMain->setCurrentIndex(8);
	quazaaSettings.MainWindowActiveTab = 8;
}

void MainWindow::on_actionDiscovery_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/Discovery.png"));
	ui->labelMainHeaderText->setText(tr("Discovery"));
	ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
	ui->stackedWidgetMain->setCurrentIndex(9);
	quazaaSettings.MainWindowActiveTab = 9;
}

void MainWindow::on_actionScheduler_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Scheduler.png"));
	ui->labelMainHeaderText->setText(tr("Scheduler"));
	ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
	ui->stackedWidgetMain->setCurrentIndex(10);
	quazaaSettings.MainWindowActiveTab = 10;
}

void MainWindow::on_actionGraph_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Generic/Graph.png"));
	ui->labelMainHeaderText->setText(tr("Graph"));
	ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
	ui->stackedWidgetMain->setCurrentIndex(11);
	quazaaSettings.MainWindowActiveTab = 11;
}

void MainWindow::on_actionPacketDump_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/PacketDump.png"));
	ui->labelMainHeaderText->setText(tr("Packet Dump"));
	ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
	ui->stackedWidgetMain->setCurrentIndex(12);
	quazaaSettings.MainWindowActiveTab = 12;
}

void MainWindow::on_actionSearchMonitor_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/SearchMonitor.png"));
	ui->labelMainHeaderText->setText(tr("Search Monitor"));
	ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
	ui->stackedWidgetMain->setCurrentIndex(13);
	quazaaSettings.MainWindowActiveTab = 13;
}

void MainWindow::on_actionHitMonitor_triggered()
{
	ui->labelMainHeaderLogo->setPixmap(QPixmap(":/Resource/Network/HitMonitor.png"));
	ui->labelMainHeaderText->setText(tr("Hit Monitor"));
	ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
	ui->stackedWidgetMain->setCurrentIndex(14);
	quazaaSettings.MainWindowActiveTab = 14;
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
	QSkinDialog *dlgSkinAbout = new QSkinDialog(false, true, false, this);
	DialogAbout *dlgAbout = new DialogAbout;

	dlgSkinAbout->addChildWidget(dlgAbout);

	connect(dlgAbout, SIGNAL(closed()), dlgSkinAbout, SLOT(close()));
	dlgSkinAbout->show();
}

void MainWindow::on_actionCreateTorrent_triggered()
{
	QSkinDialog *dlgSkinCreateTorrent = new QSkinDialog(false, true, false, this);
	DialogCreateTorrent *dlgCreateTorrent = new DialogCreateTorrent;

	dlgSkinCreateTorrent->addChildWidget(dlgCreateTorrent);

	connect(dlgCreateTorrent, SIGNAL(closed()), dlgSkinCreateTorrent, SLOT(close()));
	dlgSkinCreateTorrent->show();
}

void MainWindow::on_actionOpenTorrent_triggered()
{
	QSkinDialog *dlgSkinOpenTorrent = new QSkinDialog(false, true, false, this);
	DialogOpenTorrent *dlgOpenTorrent = new DialogOpenTorrent;

	dlgSkinOpenTorrent->addChildWidget(dlgOpenTorrent);

	connect(dlgOpenTorrent, SIGNAL(closed()), dlgSkinOpenTorrent, SLOT(close()));
	dlgSkinOpenTorrent->show();
}

void MainWindow::on_actionSettings_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSkinSettings->show();
}

void MainWindow::on_actionShares_triggered()
{
	QSkinDialog *dlgSkinEditShares = new QSkinDialog(false, true, false, this);
	DialogEditShares *dlgEditShares = new DialogEditShares;

	dlgSkinEditShares->addChildWidget(dlgEditShares);

	connect(dlgEditShares, SIGNAL(closed()), dlgSkinEditShares, SLOT(close()));
	dlgSkinEditShares->show();
}

void MainWindow::on_actionOpenDownloadFolder_triggered()
{
	Functions.FolderOpen(quazaaSettings.DownloadsCompletePath);
}

void MainWindow::on_actionURLDownload_triggered()
{
	QSkinDialog *dlgSkinAddDownload = new QSkinDialog(false, true, false, this);
	DialogAddDownload *dlgAddDownload = new DialogAddDownload;

	dlgSkinAddDownload->addChildWidget(dlgAddDownload);

	connect(dlgAddDownload, SIGNAL(closed()), dlgSkinAddDownload, SLOT(close()));
	dlgSkinAddDownload->show();
}

void MainWindow::on_actionImportPartials_triggered()
{
	QSkinDialog *dlgSkinDownloadsImport = new QSkinDialog(false, true, false, this);
	DialogDownloadsImport *dlgDownloadsImport = new DialogDownloadsImport;

	dlgSkinDownloadsImport->addChildWidget(dlgDownloadsImport);

	connect(dlgDownloadsImport, SIGNAL(closed()), dlgSkinDownloadsImport, SLOT(close()));
	dlgSkinDownloadsImport->show();
}

void MainWindow::on_actionChooseSkin_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSettings->switchSettingsPage(3);
	dlgSkinSettings->show();
}

void MainWindow::on_actionChooseLanguage_triggered()
{
	QSkinDialog *dlgSkinLanguage = new QSkinDialog(false, true, false, this);
	DialogLanguage *dlgLanguage = new DialogLanguage;

	dlgSkinLanguage->addChildWidget(dlgLanguage);

	connect(dlgLanguage, SIGNAL(closed()), dlgSkinLanguage, SLOT(close()));
	dlgSkinLanguage->exec();
}

void MainWindow::on_actionQuickstartWizard_triggered()
{
	QSkinDialog *dlgSkinWizard = new QSkinDialog(false, true, false, this);
	DialogWizard *dlgWizard = new DialogWizard();

	dlgSkinWizard->addChildWidget(dlgWizard);

	connect(dlgWizard, SIGNAL(closed()), dlgSkinWizard, SLOT(close()));
	dlgSkinWizard->show();
}

void MainWindow::on_actionEditMyProfile_triggered()
{
	QSkinDialog *dlgSkinProfile = new QSkinDialog(false, true, false, this);
	DialogProfile *dlgProfile = new DialogProfile;

	dlgSkinProfile->addChildWidget(dlgProfile);

	connect(dlgProfile, SIGNAL(closed()), dlgSkinProfile, SLOT(close()));
	dlgSkinProfile->show();
}

void MainWindow::on_actionSecurityAddRule_triggered()
{
	QSkinDialog *dlgSkinAddRule = new QSkinDialog(false, true, false, this);
	DialogAddRule *dlgAddRule = new DialogAddRule;

	dlgSkinAddRule->addChildWidget(dlgAddRule);

	connect(dlgAddRule, SIGNAL(closed()), dlgSkinAddRule, SLOT(close()));
	dlgSkinAddRule->show();
}

void MainWindow::on_toolButtonSecuritySubscribe_clicked()
{
	QSkinDialog *dlgSkinSecuritySubscriptions = new QSkinDialog(false, true, false, this);
	DialogSecuritySubscriptions *dlgSecuritySubscriptions = new DialogSecuritySubscriptions;

	dlgSkinSecuritySubscriptions->addChildWidget(dlgSecuritySubscriptions);

	connect(dlgSecuritySubscriptions, SIGNAL(closed()), dlgSkinSecuritySubscriptions, SLOT(close()));
	dlgSkinSecuritySubscriptions->show();
}

void MainWindow::on_toolButtonLibrarySearch_clicked()
{
	QSkinDialog *dlgSkinLibrarySearch = new QSkinDialog(false, true, false, this);
	DialogLibrarySearch *dlgLibrarySearch = new DialogLibrarySearch;

	dlgSkinLibrarySearch->addChildWidget(dlgLibrarySearch);

	connect(dlgLibrarySearch, SIGNAL(closed()), dlgSkinLibrarySearch, SLOT(close()));
	dlgSkinLibrarySearch->show();
}

void MainWindow::on_toolButtonSearchFilter_clicked()
{
	QSkinDialog *dlgSkinFilterSearch = new QSkinDialog(false, true, false, this);
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch;

	dlgSkinFilterSearch->addChildWidget(dlgFilterSearch);

	connect(dlgFilterSearch, SIGNAL(closed()), dlgSkinFilterSearch, SLOT(close()));
	dlgSkinFilterSearch->show();
}

void MainWindow::on_actionNetworkSettings_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSettings->switchSettingsPage(20);
	dlgSkinSettings->show();
}

void MainWindow::on_actionChatSettings_triggered()
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSettings->switchSettingsPage(7);
	dlgSkinSettings->show();
}

void MainWindow::on_toolButtonHitMonitorFilter_clicked()
{
	QSkinDialog *dlgSkinFilterSearch = new QSkinDialog(false, true, false, this);
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch;

	dlgSkinFilterSearch->addChildWidget(dlgFilterSearch);

	connect(dlgFilterSearch, SIGNAL(closed()), dlgSkinFilterSearch, SLOT(close()));
	dlgSkinFilterSearch->show();
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

void MainWindow::on_tableWidgetMediaPlaylistTask_doubleClicked(QModelIndex index)
{

}

void MainWindow::on_actionMediaRepeat_triggered(bool checked)
{
	quazaaSettings.MediaRepeat = checked;
}

void MainWindow::clearPlaylist()
{

}

void MainWindow::skinChangeEvent()
{
	setStyleSheet(skinSettings.standardItems);
	ui->toolBarMain->setStyleSheet(skinSettings.navigationToolbar);
	switch (ui->stackedWidgetMain->currentIndex())
	{
		case 0:
		ui->frameMainHeader->setStyleSheet(skinSettings.homeHeader);
		break;
		case 1:
		ui->frameMainHeader->setStyleSheet(skinSettings.libraryHeader);
			break;
		case 2:
			ui->frameMainHeader->setStyleSheet(skinSettings.mediaHeader);
			break;
		case 3:
			ui->frameMainHeader->setStyleSheet(skinSettings.searchHeader);
			break;
		case 4:
			ui->frameMainHeader->setStyleSheet(skinSettings.transfersHeader);
			break;
		case 5:
			ui->frameMainHeader->setStyleSheet(skinSettings.securityHeader);
			break;
		case 6:
			ui->frameMainHeader->setStyleSheet(skinSettings.networkHeader);
			break;
		case 7:
			ui->frameMainHeader->setStyleSheet(skinSettings.chatHeader);
			break;
		case 8:
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			break;
		case 9:
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			break;
		case 10:
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			break;
		case 11:
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			break;
		case 12:
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			break;
		case 13:
			ui->frameMainHeader->setStyleSheet(skinSettings.genericHeader);
			break;
		default:
			ui->frameMainHeader->setStyleSheet(skinSettings.homeHeader);
			break;
	}
	ui->scrollAreaHomeSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->scrollAreaSearchSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->frameLibraryNavigator->setStyleSheet(skinSettings.sidebarBackground);
	ui->frameChatLeftSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->frameChatRightSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonHomeLibraryTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeConnectionTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeLibraryTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeTorrentsTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeTransfersTaskDownloadsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeTransfersTaskUploadsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonMediaPlaylistTaskHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonSearchFiletypeTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonSearchNetworksTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonSearchResultsTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonSearchTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonNewSearch->setStyleSheet(skinSettings.addSearchButton);
	ui->toolButtonDownloadsHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonUploadsHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonNeighborsHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonSystemLogHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonChatFriendsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonChatRoomsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonChatUsersHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->frameLibraryTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameConnectionTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameTorrentsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameTransfersTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameMediaPlaylistSidebar->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameSearchTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameSearchNetworksTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameSearchResultsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameSearchFiletypeTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameChatUsersTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameChatFriendsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameChatRoomsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->tabWidgetLibraryNavigator->setStyleSheet(skinSettings.libraryNavigator);
	ui->frameSearches->setStyleSheet(skinSettings.tabSearches);
	ui->toolFrameChat->setStyleSheet(skinSettings.chatToolbar);
	ui->toolFrameMedia->setStyleSheet(skinSettings.mediaToolbar);
	ui->frameChatWelcome->setStyleSheet(skinSettings.chatWelcome);
	ui->toolFrameDiscoveryBottom->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameDiscoveryTop->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameDownloads->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameDownloadsFilter->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameGraph->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameHitMonitor->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameHostCache->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameLibrary->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameLibraryStatus->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameNeighbors->setStyleSheet(skinSettings.toolbars);
	ui->toolFramePacketDump->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameScheduler->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameSearch->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameSearchMonitor->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameSecurity->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameUploads->setStyleSheet(skinSettings.toolbars);
}

void MainWindow::on_actionMediaOpen_triggered()
{
	mediaPlayer->openFile();
}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
	quazaaSettings.MediaVolume = value;
}

void MainWindow::render(QPaintDevice * target, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags)
{
	renderFlags = RenderFlags( QWidget::DrawWindowBackground | QWidget::DrawChildren | QWidget::IgnoreMask);
	render(target, targetOffset, sourceRegion, renderFlags);
}

void MainWindow::render(QPainter * painter, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags)
{
	renderFlags = RenderFlags( QWidget::DrawWindowBackground | QWidget::DrawChildren | QWidget::IgnoreMask);
	render(painter, targetOffset, sourceRegion, renderFlags);
}

void MainWindow::on_actionQuazaaForums_triggered()
{
	QDesktopServices::openUrl(QUrl("https://sourceforge.net/apps/phpbb/quazaa/index.php", QUrl::TolerantMode));
}

void MainWindow::on_actionUsersGuide_triggered()
{
	QDesktopServices::openUrl(QUrl("https://sourceforge.net/apps/mediawiki/quazaa/index.php?title=Manual&action=edit&redlink=1", QUrl::TolerantMode));
}

void MainWindow::on_actionFAQ_triggered()
{
	QDesktopServices::openUrl(QUrl("https://sourceforge.net/apps/mediawiki/quazaa/index.php?title=FAQ&action=edit&redlink=1", QUrl::TolerantMode));
}

void MainWindow::on_actionConnectionTest_triggered()
{
	QDesktopServices::openUrl(QUrl("http://jlh.no-ip.org/connectiontest", QUrl::TolerantMode));
}

void MainWindow::on_labelWelcomeUserGuideLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void MainWindow::on_actionAddScheduledTask_triggered()
{
	QSkinDialog *dlgSkinScheduler = new QSkinDialog(false, true, false, this);
	DialogScheduler *dlgScheduler = new DialogScheduler;

	dlgSkinScheduler->addChildWidget(dlgScheduler);

	connect(dlgScheduler, SIGNAL(closed()), dlgSkinScheduler, SLOT(close()));
	dlgSkinScheduler->show();
}

void MainWindow::on_actionScheduleProperties_triggered()
{
	QSkinDialog *dlgSkinScheduler = new QSkinDialog(false, true, false, this);
	DialogScheduler *dlgScheduler = new DialogScheduler;

	dlgSkinScheduler->addChildWidget(dlgScheduler);

	connect(dlgScheduler, SIGNAL(closed()), dlgSkinScheduler, SLOT(close()));
	dlgSkinScheduler->show();
}
