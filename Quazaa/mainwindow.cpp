//
// mainwindow.cpp
//
// Copyright  Quazaa Development Team, 2009.
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
#include "persistentsettings.h"
#include "dialogsplash.h"
#include "dialogsettings.h"
#include "dialogprofile.h"
#include "dialogabout.h"
#include "dialogwizard.h"
#include "dialogeditshares.h"
#include "dialogopentorrent.h"
#include "dialogcreatetorrent.h"
#include "dialogadddownload.h"
#include "dialogdownloadsimport.h"
#include "dialogscheduler.h"
#include "dialoglanguage.h"
#include "dialogselectvisualisation.h"
#include "dialogfiltersearch.h"
#include "dialogaddrule.h"
#include "dialogsecuritysubscriptions.h"
#include "dialogtorrentproperties.h"
#include "dialogclosetype.h"
#include "commonfunctions.h"
#include "quazaaglobals.h"
#include <QSettings>
#include <QResource>
#include <QUrl>
#include <QMessageBox>
#include <QWindowStateChangeEvent>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindowClass)
{
	ui->setupUi(this);

	//Create splash window
	DialogSplash *dlgSplash = new DialogSplash();
	dlgSplash->setWindowFlags(Qt::SplashScreen);
	dlgSplash->show();
	//Load Settings
	dlgSplash->updateProgress(5, tr("Loading Settings..."));
	Settings.loadSettings();
	//Check if this is Quazaa's first run
	dlgSplash->updateProgress(8, tr("Checking for first run..."));
	if (Settings.Basic.FirstRun)
	{
		dlgSplash->updateProgress(9, tr("Running first run wizard..."));
		Settings.Basic.FirstRun = false;
		Settings.saveSettings();
		Settings.saveProfile();
		DialogWizard *dlgWizard = new DialogWizard();
		dlgWizard->exec();
	}
	//Load the library
	dlgSplash->updateProgress(10, tr("Loading Library..."));
	fileSystemModelLibraryFolders.setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
	fileSystemModelLibraryFolders.setRootPath(Settings.Library.Shares.join(" "));

	ui->treeViewLibraryNavigatorFolders->setModel(&fileSystemModelLibraryFolders);

	/*for (int m_iSharesIndex = 0; m_iSharesIndex < Settings.Library.Shares.size(); ++m_iSharesIndex)
	{
		QTreeWidgetItem *m_qTreeWidgetItem = new QTreeWidgetItem();
		m_qTreeWidgetItem->setText(0, Settings.Library.Shares.at(m_iSharesIndex));
		ui->treeWidgetLibraryNavigatorFolders->addTopLevelItem(m_qTreeWidgetItem);
	}*/
	//Load user interface
	dlgSplash->updateProgress(90, tr("Loading User Interface..."));
	Settings.loadMainWindowState(this);
	ui->pagesMain->setCurrentIndex(Settings.MainWindowState.ActiveTab);
	setToolbar(Settings.MainWindowState.ActiveTab);
	ui->pagesLibraryNavigator->setCurrentIndex(Settings.MainWindowState.LibraryTab);
	ui->comboBoxSearchFileType->setCurrentIndex(Settings.MainWindowState.SearchType);
	ui->lineEditSearch->setText(Settings.MainWindowState.HomeSearch);
	ui->comboBoxFileType->setCurrentIndex(Settings.MainWindowState.HomeSearchType);
	ui->toolButtonShowHidePlaylist->setChecked(Settings.MainWindowState.PlaylistVisible);
	ui->toolButtonMediaVolumeMute->setChecked(Settings.MediaPlayer.Mute);
	ui->horizontalSliderMediaVolume->setValue(Settings.MediaPlayer.Volume);
	ui->toolButtonRepeat->setChecked(Settings.MediaPlayer.Repeat);
	ui->toolButtonShuffle->setChecked(Settings.MediaPlayer.Random);
	ui->actionAres->setChecked(Settings.Ares.Enable);
	ui->actionEDonkey_2000->setChecked(Settings.eDonkey2k.Enable);
	ui->actionGnutella_1->setChecked(Settings.Gnutella1.Enable);
	ui->actionGnutella_2->setChecked(Settings.Gnutella2.Enable);
	//Tray icon construction.
	// Create the menu that will be used for the context menu
	dlgSplash->updateProgress(95, tr("Loading Tray Icon..."));
	// Create the system tray right click menu.
	trayMenu = new QMenu(this);
	trayMenu->addAction(ui->actionShowOrHide);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionNew_Search);
	trayMenu->addAction(ui->actionURL_Download);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionPlay_Media);
	trayMenu->addAction(ui->actionPause_Media);
	trayMenu->addAction(ui->actionStop_Media);
	trayMenu->addAction(ui->actionOpen_Media);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionPrevious_Track);
	trayMenu->addAction(ui->actionNext_Track);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionExit_After_Transfers);
	trayMenu->addAction(ui->actionExit);
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(":/Graphics/Graphics/Quazaa.png"));
	trayIcon->setToolTip("Quazaa");
	trayIcon->setContextMenu(trayMenu);
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(icon_activated(QSystemTrayIcon::ActivationReason)));
	trayIcon->show();
	dlgSplash->updateProgress(100, tr("Welcome to Quazaa!"));
	dlgSplash->close();
}

MainWindow::~MainWindow()
{
	DialogSplash *dlgSplash = new DialogSplash(this);
	dlgSplash->setWindowFlags(Qt::SplashScreen);
	dlgSplash->show();
	dlgSplash->updateProgress(5, tr("Saving Settings..."));
	Settings.saveSettings();
	dlgSplash->updateProgress(95, tr("Removing Tray Icon..."));
	trayIcon->~QSystemTrayIcon();
	dlgSplash->updateProgress(100, tr("Closing User Interface..."));
	delete ui;
	dlgSplash->close();
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

bool MainWindow::event(QEvent *e)
{
	switch (e->type())
	{
		case QEvent::Close:
			if(!Globals.Application.BypassCloseEvent)
			{
				switch (Settings.Basic.CloseMode)
				{
					case 0:
						DialogCloseType *dlgCloseType = new DialogCloseType(this);
						dlgCloseType->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
						dlgCloseType->exec();
						e->ignore();
						this->close();
						return true;
					case 1:
						Settings.saveMainWindowState(this);
						break;
					case 2:
						this->hide();
						e->ignore();
						return true;
					case 3:
						Settings.saveMainWindowState(this);
						break;
					default:
						Settings.saveMainWindowState(this);
						break;
				}
			} else {
				Settings.saveMainWindowState(this);
			}
			break;
		case QEvent::WindowStateChange:
			if (MainWindow::windowState() & Qt::WindowMinimized)
			{
				QWindowStateChangeEvent event = e->WindowStateChange;
				if (Settings.Basic.TrayMinimise)
				{
					this->setWindowState(event.oldState());
					if (this->isHidden())
						this->show();
					else
						this->hide();
						e->ignore();
						return true;
				}
			}
			break;
		default:
			break;
	}
}

void MainWindow::on_actionExit_triggered()
{
	Globals.Application.BypassCloseEvent = true;
	close();
}

void MainWindow::on_actionHome_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(0);
	if (checked == false)
	{
		ui->actionHome->setChecked(true);
		return;
	}
}

void MainWindow::on_actionLibrary_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(1);
	if (checked == false)
	{
		ui->actionLibrary->setChecked(true);
		return;
	}
}

void MainWindow::on_actionMedia_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(2);
	if (checked == false)
	{
		ui->actionMedia->setChecked(true);
		return;
	}
}

void MainWindow::on_actionSearch_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(3);
	if (checked == false)
	{
		ui->actionSearch->setChecked(true);
		return;
	}
}

void MainWindow::on_actionTransfers_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(4);
	if (checked == false)
	{
		ui->actionTransfers->setChecked(true);
		return;
	}
}

void MainWindow::on_actionSecurity_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(5);
	if (checked == false)
	{
		ui->actionSecurity->setChecked(true);
		return;
	}
}

void MainWindow::on_actionNetwork_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(6);
	if (checked == false)
	{
		ui->actionNetwork->setChecked(true);
		return;
	}
}

void MainWindow::on_actionChat_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(7);
	if (checked == false)
	{
		ui->actionChat->setChecked(true);
		return;
	}
}

void MainWindow::on_actionSettings_triggered()
{
	DialogSettings *dlgSettings = new DialogSettings(this);
	dlgSettings->show();
}

void MainWindow::on_pagesMain_currentChanged(int page)
{
	Settings.MainWindowState.ActiveTab = page;
	setToolbar(page);
}

void MainWindow::on_pagesLibraryNavigator_currentChanged(int index)
{
	Settings.MainWindowState.LibraryTab = index;
}

void MainWindow::on_labelMyTransfersLink_linkActivated(QString link)
{
	link.clear();
	ui->actionTransfers->trigger();
}

void MainWindow::on_labelMyLibraryLink_linkActivated(QString link)
{
	link.clear();
	ui->actionLibrary->trigger();
}

void MainWindow::on_toolButtonMediaVolumeMute_toggled(bool checked)
{
	Settings.MediaPlayer.Mute = checked;
	if(checked)
	{
		ui->toolButtonMediaVolumeMute->setIcon(QIcon(":/MediaPlayer/Graphics/Media Player/Mute.png"));
	} else {
		ui->toolButtonMediaVolumeMute->setIcon(QIcon(":/MediaPlayer/Graphics/Media Player/Unmute.png"));
	}
}

void MainWindow::on_toolButtonNetworkSettings_clicked()
{
	DialogSettings *dlgSettings = new DialogSettings(this);
	dlgSettings->switchSettingsPage(20);
	dlgSettings->show();
}

void MainWindow::on_toolButtonChatSettings_clicked()
{
	DialogSettings *dlgSettings = new DialogSettings(this);
	dlgSettings->switchSettingsPage(7);
	dlgSettings->show();
}

void MainWindow::on_toolButtonChatEditProfile_clicked()
{
	DialogProfile *dlgProfile = new DialogProfile(this);
	dlgProfile->show();
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

void MainWindow::on_labelCustomiseSkins_linkActivated(QString link)
{
	link.clear();
	DialogSettings *dlgSettings = new DialogSettings(this);
	dlgSettings->switchSettingsPage(3);
	dlgSettings->show();
}

void MainWindow::on_labelCustomiseSecurity_linkActivated(QString link)
{
	link.clear();
	ui->actionSecurity->trigger();
}

void MainWindow::on_actionAbout_triggered()
{
	DialogAbout *dlgAbout = new DialogAbout(this);
	dlgAbout->show();
}

void MainWindow::on_actionEdit_My_Profile_triggered()
{
	DialogProfile *dlgProfile = new DialogProfile(this);
	dlgProfile->show();
}

void MainWindow::on_labelCustomiseSettings_linkActivated(QString link)
{
	link.clear();
	DialogWizard *dlgWizard = new DialogWizard(this);
	dlgWizard->show();
}

void MainWindow::on_actionQuickstart_Wizard_triggered()
{
	DialogWizard *dlgWizard = new DialogWizard(this);
	dlgWizard->show();
}

void MainWindow::on_actionHostcache_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(8);
	if (checked == false)
	{
		ui->actionHostcache->setChecked(true);
		return;
	}
}

void MainWindow::on_actionDiscovery_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(9);
	if (checked == false)
	{
		ui->actionDiscovery->setChecked(true);
		return;
	}
}

void MainWindow::on_actionGraph_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(10);
	if (checked == false)
	{
		ui->actionGraph->setChecked(true);
		return;
	}
}

void MainWindow::on_actionPacket_Dump_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(11);
	if (checked == false)
	{
		ui->actionPacket_Dump->setChecked(true);
		return;
	}
}

void MainWindow::on_actionHit_Monitor_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(13);
	if (checked == false)
	{
		ui->actionHit_Monitor->setChecked(true);
		return;
	}
}

void MainWindow::on_actionSearch_Monitor_triggered(bool checked)
{
	ui->pagesMain->setCurrentIndex(12);
	if (checked == false)
	{
		ui->actionSearch_Monitor->setChecked(true);
		return;
	}
}

void MainWindow::on_actionShowOrHide_triggered()
{
	if (isHidden())
	{
		show();
		raise();
	}
	else
	{
		hide();
	}
}

void MainWindow::on_actionChoose_Skin_triggered()
{
	DialogSettings *dlgSettings = new DialogSettings(this);
	dlgSettings->switchSettingsPage(3);
	dlgSettings->show();
}

void MainWindow::on_toolButtonMyLibraryToolHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameMyLibraryTool->setMaximumHeight(180);
	} else {
		ui->frameMyLibraryTool->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonDownloadsToolHeader_clicked(bool checked)
{
	if (!checked && !ui->toolButtonUploadsToolHeader->isChecked())
	{
		ui->frameTransfersTool->setMaximumHeight(65);
	}
	else if (checked && ui->toolButtonUploadsToolHeader->isChecked())
	{
		ui->frameTransfersTool->setMaximumHeight(185);
	}
	else
	{
		ui->frameTransfersTool->setMaximumHeight(125);
	}
}


void MainWindow::on_toolButtonUploadsToolHeader_clicked(bool checked)
{
	if (!checked && !ui->toolButtonDownloadsToolHeader->isChecked())
	{
		ui->frameTransfersTool->setMaximumHeight(65);
	}
	else if (checked && ui->toolButtonDownloadsToolHeader->isChecked())
	{
		ui->frameTransfersTool->setMaximumHeight(185);
	}
	else
	{
		ui->frameTransfersTool->setMaximumHeight(125);
	}
}

void MainWindow::on_toolButtonConnectionToolHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameConnectionTool->setMaximumHeight(119);
	} else {
		ui->frameConnectionTool->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonTorrentsToolHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameTorrentsTool->setMaximumHeight(99);
	} else {
		ui->frameTorrentsTool->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonSearchHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameSearchTool->setMaximumHeight(124);
	} else {
		ui->frameSearchTool->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonNetworksHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameSearchNetworks->setMaximumHeight(87);
	} else {
		ui->frameSearchNetworks->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonAnyFileHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonResultsHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameResults->setMaximumHeight(118);
	} else {
		ui->frameResults->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonRoomsHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameRooms->setMaximumHeight(16777215);
	} else {
		ui->frameRooms->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonFriendsHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameFriends->setMaximumHeight(16777215);
	} else {
		ui->frameFriends->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonUsersHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->frameChatUsers->setMaximumHeight(16777215);
	} else {
		ui->frameChatUsers->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonApplicationHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonArchiveHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonAudioHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonBittorrentHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonBookHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonImageHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonPresentationHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonROMHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonSkinHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonSourceCodeHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonSpreadsheetHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonVideoHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_toolButtonWordProcessingHeader_clicked(bool checked)
{
	if (checked)
	{
		ui->pagesFileType->setMaximumHeight(16777215);
	} else {
		ui->pagesFileType->setMaximumHeight(25);
	}
}

void MainWindow::on_actionShares_triggered()
{
	DialogEditShares *dlgEditShares = new DialogEditShares(this);
	dlgEditShares->show();
}

void MainWindow::on_actionCreate_Torrent_triggered()
{
	DialogCreateTorrent *dlgCreateTorrent = new DialogCreateTorrent(this);
	dlgCreateTorrent->show();
}

void MainWindow::on_actionOpen_Torrent_triggered()
{
	if (Settings.Bittorrent.UseSaveDialog)
	{
		DialogOpenTorrent *dlgOpenTorrent = new DialogOpenTorrent(this);
		dlgOpenTorrent->show();
	}
}

void MainWindow::on_actionURL_Download_triggered()
{
	DialogAddDownload *dlgAddDownload = new DialogAddDownload(this);
	dlgAddDownload->show();
}

void MainWindow::on_actionOpen_Download_Folder_triggered()
{
	Functions.FolderOpen(Settings.Downloads.CompletePath);
}

void MainWindow::on_actionImport_Partials_triggered()
{
	DialogDownloadsImport *dlgDownloadsImport = new DialogDownloadsImport(this);
	dlgDownloadsImport->show();
}

void MainWindow::on_actionScheduler_triggered()
{
	DialogScheduler *dlgScheduler = new DialogScheduler(this);
	dlgScheduler->show();
}

void MainWindow::on_actionChoose_Language_triggered()
{
	DialogLanguage *dlgLanguage = new DialogLanguage(this);
	dlgLanguage->show();
}

void MainWindow::on_toolButtonMediaPlugins_clicked()
{
	DialogSelectVisualisation *dlgSelectVisualisation = new DialogSelectVisualisation(this);
	dlgSelectVisualisation->show();
}

void MainWindow::on_toolButtonSearchFilterMore_clicked()
{
	DialogFilterSearch *dlgFilterSearch = new DialogFilterSearch(this);
	dlgFilterSearch->show();
}

void MainWindow::on_toolButtonDownloadsOpenTorrent_clicked()
{
	DialogOpenTorrent *dlgOpenTorrent = new DialogOpenTorrent(this);
	dlgOpenTorrent->show();
}

void MainWindow::on_toolButtonSecurityAddRule_clicked()
{
	DialogAddRule *dlgAddRule = new DialogAddRule(this);
	dlgAddRule->show();
}

void MainWindow::on_toolButtonSecurityModifyRule_clicked()
{
	DialogAddRule *dlgAddRule = new DialogAddRule(this);
	dlgAddRule->show();
}

void MainWindow::on_toolButtonSecuritySubscribeRules_clicked()
{
	DialogSecuritySubscriptions *dlgSecuritySubscriptions = new DialogSecuritySubscriptions(this);
	dlgSecuritySubscriptions->show();
}

void MainWindow::on_toolButtonShareFiles_clicked()
{
	DialogEditShares *dlgEditShares = new DialogEditShares(this);
	dlgEditShares->show();
}

void MainWindow::on_comboBoxSearchFileType_currentIndexChanged(int index)
{
	Settings.MainWindowState.SearchType = index;
}

void MainWindow::on_lineEditSearch_textEdited(QString searchText)
{
	Settings.MainWindowState.HomeSearch = searchText;
}

void MainWindow::on_comboBoxFileType_currentIndexChanged(int index)
{
	Settings.MainWindowState.HomeSearchType = index;
}

void MainWindow::on_toolButtonShowHidePlaylist_toggled(bool checked)
{
	Settings.MainWindowState.PlaylistVisible = checked;
}

void MainWindow::on_horizontalSliderMediaVolume_valueChanged(int value)
{
	Settings.MediaPlayer.Volume = value;
}

void MainWindow::on_toolButtonRepeat_toggled(bool checked)
{
	Settings.MediaPlayer.Repeat = checked;
}

void MainWindow::on_toolButtonShuffle_toggled(bool checked)
{
	Settings.MediaPlayer.Random = checked;
}

void MainWindow::setToolbar(int page)
{
	switch (page)
	{
		case 0:
			ui->actionHome->setChecked(true);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		 case 1:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(true);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		 case 2:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(true);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		 case 3:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(true);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		 case 4:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(true);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		 case 5:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(true);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		 case 6:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(true);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		 case 7:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(true);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		case 8:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(true);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		case 9:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(true);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		case 10:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(true);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		case 11:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(true);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(false);
			break;
		case 12:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(true);
			ui->actionHit_Monitor->setChecked(false);
			break;
		case 13:
			ui->actionHome->setChecked(false);
			ui->actionLibrary->setChecked(false);
			ui->actionMedia->setChecked(false);
			ui->actionSearch->setChecked(false);
			ui->actionTransfers->setChecked(false);
			ui->actionSecurity->setChecked(false);
			ui->actionNetwork->setChecked(false);
			ui->actionChat->setChecked(false);
			ui->actionHostcache->setChecked(false);
			ui->actionDiscovery->setChecked(false);
			ui->actionGraph->setChecked(false);
			ui->actionPacket_Dump->setChecked(false);
			ui->actionSearch_Monitor->setChecked(false);
			ui->actionHit_Monitor->setChecked(true);
			break;
	}
}

void MainWindow::on_toolButtonShowSearch_toggled(bool checked)
{
	Settings.MainWindowState.ShowSearch = checked;
}

void MainWindow::on_actionEDonkey_2000_toggled(bool checked)
{
	Settings.eDonkey2k.Enable = checked;
}

void MainWindow::on_actionGnutella_1_toggled(bool checked)
{
	Settings.Gnutella1.Enable = checked;
}

void MainWindow::on_actionGnutella_2_toggled(bool checked)
{
	Settings.Gnutella2.Enable = checked;
}

void MainWindow::on_actionAres_toggled(bool checked)
{
	Settings.Ares.Enable = checked;
}
