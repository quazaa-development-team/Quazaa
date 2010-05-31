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

void WidgetTransfers::saveWidget()
{
	quazaaSettings.WinMain.TransfersSplitter = ui->splitterTransfers->saveState();
	panelDownloads->saveWidget();
	panelUploads->saveWidget();
}

void WidgetTransfers::on_splitterTransfers_customContextMenuRequested(QPoint pos)
{
	if (ui->splitterTransfers->handle(1)->underMouse())
	{
		if (ui->splitterTransfers->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.TransfersSplitterRestoreTop = ui->splitterTransfers->sizes()[0];
			quazaaSettings.WinMain.TransfersSplitterRestoreBottom = ui->splitterTransfers->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterTransfers->sizes()[0] + ui->splitterTransfers->sizes()[1]);
			newSizes.append(0);
			ui->splitterTransfers->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreBottom);
			ui->splitterTransfers->setSizes(sizesList);
		}
	}
}
