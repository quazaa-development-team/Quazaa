/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "dialogeditshares.h"
#include "ui_dialogeditshares.h"
#include "quazaasettings.h"
#include "skinsettings.h"

#include <QtWidgets/QFileDialog>

#ifdef _DEBUG
#include "debug_new.h"
#endif

DialogEditShares::DialogEditShares(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogEditShares)
{
	ui->setupUi(this);
	ui->listWidgetShares->addItems(quazaaSettings.Library.Shares);
	ui->pushButtonOk->setEnabled(false);
	setSkin();
}

DialogEditShares::~DialogEditShares()
{
	delete ui;
}

void DialogEditShares::changeEvent(QEvent* e)
{
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
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
	quazaaSettings.Library.Shares.clear();
	for(int m_iSharesRow = 0; m_iSharesRow < ui->listWidgetShares->count(); ++m_iSharesRow)
	{
		ui->listWidgetShares->setCurrentRow(m_iSharesRow);
		quazaaSettings.Library.Shares.append(ui->listWidgetShares->currentItem()->text());
	}
	quazaaSettings.Library.Shares.removeDuplicates();
	emit closed();
	close();
}

void DialogEditShares::on_pushButtonAdd_clicked()
{
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QDir directory(QFileDialog::getExistingDirectory(this,
													 tr("Select A Folder To Share"),
													 quazaaSettings.Downloads.CompletePath,
													 options));
	if(directory.exists())
	{
		ui->listWidgetShares->addItem(directory.canonicalPath());
		ui->pushButtonOk->setEnabled(true);
	}
}

void DialogEditShares::on_pushButtonRemove_clicked()
{
	if(ui->listWidgetShares->currentRow() != -1)
	{
		ui->listWidgetShares->takeItem(ui->listWidgetShares->currentRow());
		ui->pushButtonOk->setEnabled(true);
	}
}

void DialogEditShares::setSkin()
{

}
