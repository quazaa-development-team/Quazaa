//
// widgethome.cpp
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

#include "widgethome.h"
#include "ui_widgethome.h"
#include "dialogadddownload.h"
#include "dialogopentorrent.h"
#include "dialogsettings.h"
#include "dialogwizard.h"
#include "widgetsearchtemplate.h"
#include "systemlog.h"

#include "quazaasettings.h"
 

WidgetHome::WidgetHome(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::WidgetHome)
{
	ui->setupUi(this);
	ui->comboBoxHomeFileType->setView(new QListView());
	connect(ui->labelLibraryTaskLink, SIGNAL(linkActivated(QString)), this, SIGNAL(triggerLibrary()));
	connect(ui->labelWelcomeSecurityLink, SIGNAL(linkActivated(QString)), this, SIGNAL(triggerSecurity()));
	connect(ui->labelTransfersTaskLink, SIGNAL(linkActivated(QString)), this, SIGNAL(triggerTransfers()));
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

void WidgetHome::changeEvent(QEvent* e)
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

void WidgetHome::on_labelLibraryTaskHashFaster_linkActivated(QString link)
{
	Q_UNUSED(link);
}

void WidgetHome::on_labelTorrentsTaskSeedTorrentLink_linkActivated(QString link)
{
	Q_UNUSED(link);
}

void WidgetHome::on_labelWelcomeURLDownloadLink_linkActivated(QString link)
{
	Q_UNUSED(link);
	DialogAddDownload* dlgAddDownload = new DialogAddDownload;
	dlgAddDownload->show();
}

void WidgetHome::on_labelWelcomeOpenTorrentLink_linkActivated(QString link)
{
	Q_UNUSED(link);
	DialogOpenTorrent* dlgOpenTorrent = new DialogOpenTorrent;
	dlgOpenTorrent->show();
}

void WidgetHome::on_labelWelcomeSkinLink_linkActivated(QString link)
{
	Q_UNUSED(link);
	DialogSettings* dlgSettings = new DialogSettings(this, SettingsPage::Skins);
	dlgSettings->show();
}

void WidgetHome::on_labelWelcomeWizardLink_linkActivated(QString link)
{
	Q_UNUSED(link);
	DialogWizard* dlgWizard = new DialogWizard();
	dlgWizard->show();
}

void WidgetHome::on_labelWelcomeUserGuideLink_linkActivated(QString link)
{
	Q_UNUSED(link);

	QDesktopServices::openUrl(QUrl("https://sourceforge.net/apps/mediawiki/quazaa/index.php?title=Manual", QUrl::TolerantMode));
}

void WidgetHome::saveWidget()
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

void WidgetHome::on_toolButtonWelcomeSearch_clicked()
{
	QString m_sSearchString = ui->lineEditWelcomeSearch->text();
	emit requestSearch(&m_sSearchString);
}

void WidgetHome::mouseDoubleClickEvent(QMouseEvent*)
{


}

void WidgetHome::on_splitterHome_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterHome->handle(1)->underMouse())
	{
		if(ui->splitterHome->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.HomeSplitterRestoreLeft = ui->splitterHome->sizes()[0];
			quazaaSettings.WinMain.HomeSplitterRestoreRight = ui->splitterHome->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterHome->sizes()[0] + ui->splitterHome->sizes()[1]);
			ui->splitterHome->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.HomeSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.HomeSplitterRestoreRight);
			ui->splitterHome->setSizes(sizesList);
		}
	}
}
