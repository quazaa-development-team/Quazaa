#include "widgetmedia.h"
#include "ui_widgetmedia.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetMedia::WidgetMedia(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetMedia)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.MediaToolbars);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->actionMediaPlaylistToggle->setChecked(quazaaSettings.WinMain.MediaPlaylistVisible);
	ui->splitterMediaPlaylist->restoreState(quazaaSettings.WinMain.MediaSplitter);

	/*mediaPlayer = new vlcMediaPlayer(ui->seekSlider, ui->volumeSlider, ui->frameMediaWindow, ui->tableWidgetMediaPlaylistTask,
									 ui->actionMediaPlay, ui->actionMediaStop, ui->actionMediaRewind, ui->actionMediaNextTrack,
									 ui->actionMediaOpen, ui->actionMediaRepeat , ui->actionMediaShuffle,
									 ui->actionMediaFullscreen, ui->actionMediaSettings, ui->actionMediaMute,
									 ui->toolButtonMediaPlaylistAdd, ui->toolButtonMediaPlaylistRemove,
									 ui->toolButtonMediaPlaylistClear,ui->toolButtonMediaPlaylistOpenPlaylist,
									 ui->toolButtonMediaPlaylistAddPlaylist,
									 ui->toolButtonMediaPlaylistSavePlaylist, this);*/
	ui->actionMediaRepeat->setChecked(quazaaSettings.Media.Repeat);
	ui->actionMediaShuffle->setChecked(quazaaSettings.Media.Shuffle);
}

WidgetMedia::~WidgetMedia()
{
	delete ui;
}

void WidgetMedia::changeEvent(QEvent *e)
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

void WidgetMedia::skinChangeEvent()
{
	ui->toolButtonMediaPlaylistTaskHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->frameMediaPlaylistSidebar->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->toolBarPlayControls->setStyleSheet(skinSettings.mediaToolbar);
	ui->toolBarSettings->setStyleSheet(skinSettings.mediaToolbar);
	ui->toolBarVolume->setStyleSheet(skinSettings.mediaToolbar);
}

void WidgetMedia::saveWidget()
{
	quazaaSettings.WinMain.MediaToolbars = saveState();
	quazaaSettings.WinMain.MediaPlaylistVisible = ui->actionMediaPlaylistToggle->isChecked();
	quazaaSettings.WinMain.MediaSplitter = ui->splitterMediaPlaylist->saveState();
}

/*void WidgetMedia::on_actionMediaOpen_triggered()
{
	mediaPlayer->openFile();
}

void WidgetMedia::on_volumeSlider_valueChanged(int value)
{
	quazaaSettings.Media.Volume = value;
}*/

void WidgetMedia::on_actionMediaRepeat_triggered(bool checked)
{
	quazaaSettings.Media.Repeat = checked;
}
