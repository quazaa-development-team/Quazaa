//
// widgettransfers.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
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

#include "widgettransfers.h"
#include "ui_widgettransfers.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetTransfers::WidgetTransfers(QWidget* parent) :
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

void WidgetTransfers::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);
	switch(e->type())
	{
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
	Q_UNUSED(pos);

	if(ui->splitterTransfers->handle(1)->underMouse())
	{
		if(ui->splitterTransfers->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.TransfersSplitterRestoreTop = ui->splitterTransfers->sizes()[0];
			quazaaSettings.WinMain.TransfersSplitterRestoreBottom = ui->splitterTransfers->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterTransfers->sizes()[0] + ui->splitterTransfers->sizes()[1]);
			newSizes.append(0);
			ui->splitterTransfers->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreBottom);
			ui->splitterTransfers->setSizes(sizesList);
		}
	}
}

void WidgetTransfers::on_toolButtonUploadsHeader_clicked()
{
	if(ui->splitterTransfers->sizes()[0] > 0)
	{
		quazaaSettings.WinMain.TransfersSplitterRestoreTop = ui->splitterTransfers->sizes()[0];
		quazaaSettings.WinMain.TransfersSplitterRestoreBottom = ui->splitterTransfers->sizes()[1];
		QList<int> newSizes;
		newSizes.append(0);
		newSizes.append(ui->splitterTransfers->sizes()[0] + ui->splitterTransfers->sizes()[1]);
		ui->splitterTransfers->setSizes(newSizes);
	}
	else
	{
		QList<int> sizesList;
		sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreTop);
		sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreBottom);
		ui->splitterTransfers->setSizes(sizesList);
	}
}

void WidgetTransfers::on_toolButtonDownloadsHeader_clicked()
{
	if(ui->splitterTransfers->sizes()[1] > 0)
	{
		quazaaSettings.WinMain.TransfersSplitterRestoreTop = ui->splitterTransfers->sizes()[0];
		quazaaSettings.WinMain.TransfersSplitterRestoreBottom = ui->splitterTransfers->sizes()[1];
		QList<int> newSizes;
		newSizes.append(ui->splitterTransfers->sizes()[0] + ui->splitterTransfers->sizes()[1]);
		newSizes.append(0);
		ui->splitterTransfers->setSizes(newSizes);
	}
	else
	{
		QList<int> sizesList;
		sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreTop);
		sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreBottom);
		ui->splitterTransfers->setSizes(sizesList);
	}
}
