//
// dialogabout.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
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

#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "quazaaglobals.h"
#include "dialoggplview.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>
#include "QSkinDialog/qskindialog.h"
#include "QSkinDialog/qskinsettings.h"

DialogAbout::DialogAbout(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogAbout)
{
	m_ui->setupUi(this);
	m_ui->labelVersion->setText(m_ui->labelVersion->text().replace("&lt;version&gt;", quazaaGlobals.ApplicationVersionString()));
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogAbout::~DialogAbout()
{
	delete m_ui;
}

void DialogAbout::changeEvent(QEvent *e)
{
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogAbout::on_pushButtonOK_clicked()
{
	emit closed();
	close();
}

void DialogAbout::on_labelCopyright_linkActivated(QString link)
{
		link.clear();
	QSkinDialog *dlgSkinGPLView = new QSkinDialog(false, true, false, this);
	DialogGPLView *dlgGPLView = new DialogGPLView(this);

	dlgSkinGPLView->addChildWidget(dlgGPLView);

	connect(dlgGPLView, SIGNAL(closed()), dlgSkinGPLView, SLOT(close()));
	dlgSkinGPLView->show();
}

void DialogAbout::on_labelQuazaaLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_labelQtLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_labelGimpLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_toolButtonAboutQt_clicked()
{
	QMessageBox aboutQt;
	aboutQt.aboutQt(this, tr("About Qt"));
}

void DialogAbout::on_labelShareazaLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_labelHaliteLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_labelFrostwireLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_labelEMuleLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_labelAresLink_linkActivated(QString link)
{
	QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
}

void DialogAbout::on_textBrowserCredits_anchorClicked(QUrl link)
{
	QDesktopServices::openUrl(link);
}

void DialogAbout::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
