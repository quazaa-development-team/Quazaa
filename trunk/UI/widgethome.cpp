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
#include "QSkinDialog/qskinsettings.h"

WidgetHome::WidgetHome(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetHome)
{
	ui->setupUi(this);
	ui->comboBoxHomeFileType->setView(new QListView());
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
	if (skinSettings.splashLogo.isEmpty())
	{
		ui->labelHomeLogo->setPixmap(QPixmap(":/Resource/QuazaaLogo.png"));
	} else {
		ui->labelHomeLogo->setPixmap(QPixmap());
		ui->labelHomeLogo->setStyleSheet(skinSettings.splashLogo);
	}
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
	ui->listWidgetConnectionTask->setStyleSheet(skinSettings.listViews);
	ui->listWidgetLibraryTask->setStyleSheet(skinSettings.listViews);
	ui->listWidgetTorrentsTask->setStyleSheet(skinSettings.listViews);
	ui->listWidgetTransfersTaskDownloadsTool->setStyleSheet(skinSettings.listViews);
	ui->listWidgetTransfersTaskUploadsTool->setStyleSheet(skinSettings.listViews);
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

	QSkinDialog *dlgSkinAddDownload = new QSkinDialog(false, true, false, false, this);
	DialogAddDownload *dlgAddDownload = new DialogAddDownload;

	dlgSkinAddDownload->addChildWidget(dlgAddDownload);

	connect(dlgAddDownload, SIGNAL(closed()), dlgSkinAddDownload, SLOT(close()));
	dlgSkinAddDownload->show();
}

void WidgetHome::on_labelWelcomeOpenTorrentLink_linkActivated(QString link)
{
	Q_UNUSED(link);

	QSkinDialog *dlgSkinOpenTorrent = new QSkinDialog(false, true, false, false, this);
	DialogOpenTorrent *dlgOpenTorrent = new DialogOpenTorrent;

	dlgSkinOpenTorrent->addChildWidget(dlgOpenTorrent);

	connect(dlgOpenTorrent, SIGNAL(closed()), dlgSkinOpenTorrent, SLOT(close()));
	dlgSkinOpenTorrent->show();
}

void WidgetHome::on_labelWelcomeSkinLink_linkActivated(QString link)
{
	Q_UNUSED(link);

	QSkinDialog *dlgSkinSettings = new QSkinDialog(true, true, false, false, this);
	DialogSettings *dlgSettings = new DialogSettings(this, SettingsPage::Skins);

	dlgSkinSettings->addChildWidget(dlgSettings);

	connect(dlgSettings, SIGNAL(closed()), dlgSkinSettings, SLOT(close()));
	dlgSkinSettings->show();
}

void WidgetHome::on_labelWelcomeWizardLink_linkActivated(QString link)
{
	Q_UNUSED(link);

	QSkinDialog *dlgSkinWizard = new QSkinDialog(false, true, false, false, this);
	DialogWizard *dlgWizard = new DialogWizard();

	dlgSkinWizard->addChildWidget(dlgWizard);

	connect(dlgWizard, SIGNAL(closed()), dlgSkinWizard, SLOT(close()));
	dlgSkinWizard->show();
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

void WidgetHome::mouseDoubleClickEvent(QMouseEvent *)
{


}

void WidgetHome::on_splitterHome_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if (ui->splitterHome->handle(1)->underMouse())
	{
		if (ui->splitterHome->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.HomeSplitterRestoreLeft = ui->splitterHome->sizes()[0];
			quazaaSettings.WinMain.HomeSplitterRestoreRight = ui->splitterHome->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterHome->sizes()[0] + ui->splitterHome->sizes()[1]);
			ui->splitterHome->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.HomeSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.HomeSplitterRestoreRight);
			ui->splitterHome->setSizes(sizesList);
		}
	}
}
