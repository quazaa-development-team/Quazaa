#include "widgethome.h"
#include "ui_widgethome.h"
#include "dialogadddownload.h"
#include "dialogopentorrent.h"
#include "dialogsettings.h"
#include "dialogwizard.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetHome::WidgetHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetHome)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	connect(ui->labelLibraryTaskLink, SIGNAL(linkActivated(QString)), this, SIGNAL(triggerLibrary()));
	connect(ui->labelWelcomeSecurityLink, SIGNAL(linkActivated(QString)), this, SIGNAL(triggerSecurity()));
	connect(ui->labelTransfersTaskLink, SIGNAL(linkActivated(QString)), this, SIGNAL(triggerTransfers()));
	skinChangeEvent();
	ui->splitterHome->restoreState(quazaaSettings.WinMain.HomeSplitter);
	ui->lineEditWelcomeSearch->setText(quazaaSettings.WinMain.HomeSearchString);
	ui->toolButtonHomeConnectionTaskHeader->setChecked(quazaaSettings.WinMain.HomeConnectionTaskVisible);
	ui->toolButtonHomeTransfersTaskDownloadsHeader->setChecked(quazaaSettings.WinMain.HomeDownloadsTaskVisible);
	ui->toolButtonHomeLibraryTaskHeader->setChecked(quazaaSettings.WinMain.HomeLibraryTaskVisible);
	ui->lineEditWelcomeSearch->setText(quazaaSettings.WinMain.HomeSearchString);
	ui->toolButtonHomeTorrentsTaskHeader->setChecked(quazaaSettings.WinMain.HomeTorrentsTaskVisible);
	ui->toolButtonHomeTransfersTaskUploadsHeader->setChecked(quazaaSettings.WinMain.HomeUploadsTaskVisible);
}

WidgetHome::~WidgetHome()
{
	delete ui;
}

void WidgetHome::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void WidgetHome::skinChangeEvent()
{
	ui->scrollAreaHomeSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonHomeLibraryTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeConnectionTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeLibraryTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeTorrentsTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeTransfersTaskDownloadsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->toolButtonHomeTransfersTaskUploadsHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->frameLibraryTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameConnectionTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameTorrentsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameTransfersTask->setStyleSheet(skinSettings.sidebarTaskBackground);
}

void WidgetHome::on_labelLibraryTaskHashFaster_linkActivated(QString link)
{

}

void WidgetHome::on_labelTorrentsTaskSeedTorrentLink_linkActivated(QString link)
{

}

void WidgetHome::on_labelWelcomeURLDownloadLink_linkActivated(QString link)
{
	QSkinDialog *dlgSkinAddDownload = new QSkinDialog(false, true, false, this);
	DialogAddDownload *dlgAddDownload = new DialogAddDownload;

	dlgSkinAddDownload->addChildWidget(dlgAddDownload);

	connect(dlgAddDownload, SIGNAL(closed()), dlgSkinAddDownload, SLOT(close()));
	dlgSkinAddDownload->show();
}

void WidgetHome::on_labelWelcomeOpenTorrentLink_linkActivated(QString link)
{
	QSkinDialog *dlgSkinOpenTorrent = new QSkinDialog(false, true, false, this);
	DialogOpenTorrent *dlgOpenTorrent = new DialogOpenTorrent;

	dlgSkinOpenTorrent->addChildWidget(dlgOpenTorrent);

	connect(dlgOpenTorrent, SIGNAL(closed()), dlgSkinOpenTorrent, SLOT(close()));
	dlgSkinOpenTorrent->show();
}

void WidgetHome::on_labelWelcomeSkinLink_linkActivated(QString link)
{
	QSkinDialog *dlgSkinSettings = new QSkinDialog(false, true, false, this);
	DialogSettings *dlgSettings = new DialogSettings;

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSettings->switchSettingsPage(3);
	dlgSkinSettings->show();
}

void WidgetHome::on_labelWelcomeWizardLink_linkActivated(QString link)
{
	QSkinDialog *dlgSkinWizard = new QSkinDialog(false, true, false, this);
	DialogWizard *dlgWizard = new DialogWizard();

	dlgSkinWizard->addChildWidget(dlgWizard);

	connect(dlgWizard, SIGNAL(closed()), dlgSkinWizard, SLOT(close()));
	dlgSkinWizard->show();
}

void WidgetHome::on_labelWelcomeUserGuideLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl("https://sourceforge.net/apps/mediawiki/quazaa/index.php?title=Manual", QUrl::TolerantMode));
}

void WidgetHome::saveState()
{
	quazaaSettings.WinMain.HomeSplitter = ui->splitterHome->saveState();
	quazaaSettings.WinMain.HomeSearchString = ui->lineEditWelcomeSearch->text();
	quazaaSettings.WinMain.HomeConnectionTaskVisible = ui->toolButtonHomeConnectionTaskHeader->isChecked();
	quazaaSettings.WinMain.HomeDownloadsTaskVisible = ui->toolButtonHomeTransfersTaskDownloadsHeader->isChecked();
	quazaaSettings.WinMain.HomeLibraryTaskVisible = ui->toolButtonHomeLibraryTaskHeader->isChecked();
	quazaaSettings.WinMain.HomeSearchString = ui->lineEditWelcomeSearch->text();
	quazaaSettings.WinMain.HomeTorrentsTaskVisible = ui->toolButtonHomeTorrentsTaskHeader->isChecked();
	quazaaSettings.WinMain.HomeUploadsTaskVisible = ui->toolButtonHomeTransfersTaskUploadsHeader->isChecked();
}
