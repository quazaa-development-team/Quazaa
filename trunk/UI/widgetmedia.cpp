#include "widgetmedia.h"
#include "ui_widgetmedia.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetMedia::WidgetMedia(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetMedia)
{
    ui->setupUi(this);
	seekSlider = new QSlider();
	seekSlider->setOrientation(Qt::Horizontal);
	volumeSlider = new QSlider();
	volumeSlider->setOrientation(Qt::Horizontal);
	volumeSlider->setMaximumWidth(100);
	ui->toolBarPlayControls->addWidget(seekSlider);
	ui->toolBarVolume->addWidget(volumeSlider);
	restoreState(quazaaSettings.WinMain.MediaToolbars);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterMedia->restoreState(quazaaSettings.WinMain.MediaSplitter);

	/*mediaPlayer = new vlcMediaPlayer(seekSlider, volumeSlider, ui->frameMediaWindow, ui->tableWidgetMediaPlaylistTask,
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
	seekSlider->setStyleSheet(skinSettings.seekSlider);
	volumeSlider->setStyleSheet(skinSettings.volumeSlider);
	ui->toolButtonMediaPlaylistTaskHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->frameMediaPlaylistSidebar->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->toolBarPlayControls->setStyleSheet(skinSettings.mediaToolbar);
	ui->toolBarSettings->setStyleSheet(skinSettings.mediaToolbar);
	ui->toolBarVolume->setStyleSheet(skinSettings.mediaToolbar);
}

void WidgetMedia::saveWidget()
{
	quazaaSettings.WinMain.MediaToolbars = saveState();
	quazaaSettings.WinMain.MediaSplitter = ui->splitterMedia->saveState();
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

void WidgetMedia::on_actionMediaShuffle_triggered(bool checked)
{
	quazaaSettings.Media.Shuffle = checked;
}

void WidgetMedia::on_splitterMedia_customContextMenuRequested(QPoint pos)
{
	if (ui->splitterMedia->handle(1)->underMouse())
	{
		if (ui->splitterMedia->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.MediaSplitterRestoreLeft = ui->splitterMedia->sizes()[0];
			quazaaSettings.WinMain.MediaSplitterRestoreRight = ui->splitterMedia->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterMedia->sizes()[0] + ui->splitterMedia->sizes()[1]);
			newSizes.append(0);
			ui->splitterMedia->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreRight);
			ui->splitterMedia->setSizes(sizesList);
		}
	}
}

void WidgetMedia::on_toolButtonMediaPlaylistTaskHeader_clicked()
{
	if (ui->splitterMedia->sizes()[0] > 0)
	{
		quazaaSettings.WinMain.MediaSplitterRestoreLeft = ui->splitterMedia->sizes()[0];
		quazaaSettings.WinMain.MediaSplitterRestoreRight = ui->splitterMedia->sizes()[1];
		QList<int> newSizes;
		newSizes.append(0);
		newSizes.append(ui->splitterMedia->sizes()[0] + ui->splitterMedia->sizes()[1]);
		ui->splitterMedia->setSizes(newSizes);
	} else {
		QList<int> sizesList;
		sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreLeft);
		sizesList.append(quazaaSettings.WinMain.MediaSplitterRestoreRight);
		ui->splitterMedia->setSizes(sizesList);
	}
}
