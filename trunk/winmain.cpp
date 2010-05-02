#include "winmain.h"
#include "ui_winmain.h"
#include "QSkinDialog/qskindialog.h"
#include "dialogclosetype.h"
#include "dialogsplash.h"

#include "quazaasettings.h"

WinMain::WinMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WinMain)
{
    ui->setupUi(this);
    pageHome = new WidgetHome();
    ui->stackedWidgetMain->addWidget(pageHome);
    pageLibrary = new WidgetLibrary();
    ui->stackedWidgetMain->addWidget(pageLibrary);
    pageMedia = new WidgetMedia();
    ui->stackedWidgetMain->addWidget(pageMedia);
	pageSearch = new WidgetSearch();
	ui->stackedWidgetMain->addWidget(pageSearch);
	pageTransfers = new WidgetTransfers();
	ui->stackedWidgetMain->addWidget(pageTransfers);
}

WinMain::~WinMain()
{
    delete ui;
}

bool WinMain::event(QEvent *e)
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
					emit hideMain();
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

void WinMain::changeEvent(QEvent *e)
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

void WinMain::quazaaShutdown()
{
	DialogSplash *dlgSplash = new DialogSplash;
	dlgSplash->show();

	dlgSplash->updateProgress(5, tr("Closing Networks..."));
	//Network.Disconnect();

	dlgSplash->updateProgress(5, tr("Saving Settings..."));
	quazaaSettings.saveSettings();

	dlgSplash->updateProgress(10, tr("Saving UI..."));
	/*quazaaSettings.MainWindowChatFriendsTaskVisible = ui->toolButtonChatFriendsHeader->isChecked();
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
	quazaaSettings.saveWindowSettings(this);*/

	dlgSplash->updateProgress(15, tr("Removing Tray Icon..."));
	//trayIcon->~QSystemTrayIcon();

	dlgSplash->close();
	emit closed();
}

void WinMain::on_actionHome_triggered()
{
    ui->stackedWidgetMain->setCurrentIndex(0);
}

void WinMain::on_actionLibrary_triggered()
{
    ui->stackedWidgetMain->setCurrentIndex(1);
}

void WinMain::on_actionMedia_triggered()
{
    ui->stackedWidgetMain->setCurrentIndex(2);
}

void WinMain::on_actionSearch_triggered()
{
	ui->stackedWidgetMain->setCurrentIndex(3);
}

void WinMain::on_actionTransfers_triggered()
{
	ui->stackedWidgetMain->setCurrentIndex(4);
}

void WinMain::on_actionSecurity_triggered()
{
	ui->stackedWidgetMain->setCurrentIndex(5);
}

void WinMain::on_actionActivity_triggered()
{
	ui->stackedWidgetMain->setCurrentIndex(6);
}
