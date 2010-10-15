#include "widgetextendeditems.h"
#include "ui_widgetextendeditems.h"

#include "../QSkinDialog/qskinsettings.h"

WidgetExtendedItems::WidgetExtendedItems(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetExtendedItems)
{
	ui->setupUi(this);
	ui->toolBarMainMenu->addWidget(ui->menuBarPreview);
	seekSlider = new QSlider();
	seekSlider->setOrientation(Qt::Horizontal);
	volumeSlider = new QSlider();
	volumeSlider->setOrientation(Qt::Horizontal);
	labelSeekSlider = new QLabel();
	labelSeekSlider->setText("Seek Slider:");
	labelVolumeSlider = new QLabel();
	labelVolumeSlider->setText("Volume Slider:");
	ui->toolBarMedia->addWidget(labelSeekSlider);
	ui->toolBarMedia->addWidget(seekSlider);
	ui->toolBarMedia->addWidget(labelVolumeSlider);
	ui->toolBarMedia->addWidget(volumeSlider);
}

WidgetExtendedItems::~WidgetExtendedItems()
{
	delete ui;
}

void WidgetExtendedItems::changeEvent(QEvent *e)
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

void WidgetExtendedItems::skinChangeEvent()
{
	this->setStyleSheet(skinSettings.standardItems);
	ui->scrollAreaSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonSidebarTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->frameSidebarTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->toolButtonUnclickableSidebarTaskHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonNewSearch->setStyleSheet(skinSettings.addSearchButton);
	ui->frameChatWelcome->setStyleSheet(skinSettings.chatWelcome);
	ui->toolBarChat->setStyleSheet(skinSettings.chatToolbar);
	ui->tabWidgetLibraryNavigator->setStyleSheet(skinSettings.libraryNavigator);
	ui->toolFrameLibraryStatus->setStyleSheet(skinSettings.libraryViewHeader);
	ui->frameSearches->setStyleSheet(skinSettings.tabSearches);
	ui->toolBarStandard->setStyleSheet(skinSettings.toolbars);
	ui->toolBarMedia->setStyleSheet(skinSettings.mediaToolbar);
	seekSlider->setStyleSheet(skinSettings.seekSlider);
	volumeSlider->setStyleSheet(skinSettings.volumeSlider);
	ui->toolBarMainMenu->setStyleSheet(skinSettings.mainMenuToolbar);
}
