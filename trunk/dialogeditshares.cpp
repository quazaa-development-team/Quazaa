//
// dialogeditshares.cpp
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

#include "dialogeditshares.h"
#include "ui_dialogeditshares.h"
#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"
#include <QFileDialog>

DialogEditShares::DialogEditShares(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogEditShares)
{
	m_ui->setupUi(this);
	m_ui->listWidgetShares->addItems(quazaaSettings.LibraryShares);
	m_ui->pushButtonOk->setEnabled(false);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogEditShares::~DialogEditShares()
{
	delete m_ui;
}

void DialogEditShares::changeEvent(QEvent *e)
{
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogEditShares::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogEditShares::on_pushButtonOk_clicked()
{
	quazaaSettings.LibraryShares.clear();
	for (int m_iSharesRow = 0; m_iSharesRow < m_ui->listWidgetShares->count(); ++m_iSharesRow)
	{
		m_ui->listWidgetShares->setCurrentRow(m_iSharesRow);
		quazaaSettings.LibraryShares.append(m_ui->listWidgetShares->currentItem()->text());
	}
	quazaaSettings.LibraryShares.removeDuplicates();
	emit closed();
	close();
}

void DialogEditShares::on_pushButtonAdd_clicked()
{
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QString directory = QFileDialog::getExistingDirectory(this,
							tr("Select A Folder To Share"),
							quazaaSettings.DownloadsCompletePath,
							options);
	if (!directory.isEmpty())
	{
		m_ui->listWidgetShares->addItem(directory);
		m_ui->pushButtonOk->setEnabled(true);
	}
}

void DialogEditShares::on_pushButtonRemove_clicked()
{
	if(m_ui->listWidgetShares->currentRow() != -1)
	{
		m_ui->listWidgetShares->takeItem(m_ui->listWidgetShares->currentRow());
		m_ui->pushButtonOk->setEnabled(true);
	}
}

void DialogEditShares::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
