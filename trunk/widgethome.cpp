#include "widgethome.h"
#include "ui_widgethome.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetHome::WidgetHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetHome)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
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
	quazaaSettings.WinMain.HomeSearchString = ui->lineEditWelcomeSearch->text();
	quazaaSettings.WinMain.HomeConnectionTaskVisible = ui->toolButtonHomeConnectionTaskHeader->isChecked();
	quazaaSettings.WinMain.HomeDownloadsTaskVisible = ui->toolButtonHomeTransfersTaskDownloadsHeader->isChecked();
	quazaaSettings.WinMain.HomeLibraryTaskVisible = ui->toolButtonHomeLibraryTaskHeader->isChecked();
	quazaaSettings.WinMain.HomeSearchString = ui->lineEditWelcomeSearch->text();
	quazaaSettings.WinMain.HomeTorrentsTaskVisible = ui->toolButtonHomeTorrentsTaskHeader->isChecked();
	quazaaSettings.WinMain.HomeUploadsTaskVisible = ui->toolButtonHomeTransfersTaskUploadsHeader->isChecked();
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
