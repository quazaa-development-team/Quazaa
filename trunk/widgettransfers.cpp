#include "widgettransfers.h"
#include "ui_widgettransfers.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetTransfers::WidgetTransfers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetTransfers)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterTransfers->restoreState(quazaaSettings.WinMain.TransfersSplitter);
	panelDownloads = new WidgetDownloads();
	ui->verticalLayoutDownloads->addWidget(panelDownloads);
	panelUploads = new WidgetUploads();
	ui->verticalLayoutUploads->addWidget(panelUploads);
}

WidgetTransfers::~WidgetTransfers()
{
	quazaaSettings.WinMain.TransfersSplitter = ui->splitterTransfers->saveState();
	delete ui;
}

void WidgetTransfers::changeEvent(QEvent *e)
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

void WidgetTransfers::skinChangeEvent()
{
	ui->toolButtonDownloadsHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonUploadsHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
}
