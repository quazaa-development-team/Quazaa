//
// dialognewskin.cpp
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

#include "dialognewskin.h"
#include "ui_dialognewskin.h"

DialogNewSkin::DialogNewSkin(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogNewSkin)
{
	ui->setupUi(this);
}

DialogNewSkin::~DialogNewSkin()
{
	delete ui;
}

void DialogNewSkin::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogNewSkin::on_pushButtonOK_clicked()
{
	name = ui->lineEditName->text();
	author = ui->lineEditAuthor->text();
	version = ui->lineEditVersion->text();
	description = ui->plainTextEditDescription->toPlainText();
	accept();
}

void DialogNewSkin::on_pushButtonCancel_clicked()
{
	reject();
}
