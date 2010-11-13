//
// dialogopenskin.cpp
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

#include "dialogopenskin.h"
#include "ui_dialogopenskin.h"
#include "../QSkinDialog/qskinsettings.h"
#include <QDir>
#include <QSettings>

DialogOpenSkin::DialogOpenSkin(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogOpenSkin)
{
	ui->setupUi(this);

	QDir dir(qApp->applicationDirPath() + "/Skin/");
	QFileInfoList skinDirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
	foreach(QFileInfo i, skinDirs)
	{
		QListWidgetItem* item = new QListWidgetItem(i.fileName());
		ui->listWidgetSkins->addItem(item);
	}
}

DialogOpenSkin::~DialogOpenSkin()
{
	delete ui;
}

void DialogOpenSkin::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogOpenSkin::on_pushButtonCancel_clicked()
{
	reject();
}

void DialogOpenSkin::on_pushButtonOK_clicked()
{
	QFile* m_skinFile = new QFile(skinFile);
	if(m_skinFile->exists(skinFile))
	{
		accept();
	}
	else
	{
		reject();
	}
}

void DialogOpenSkin::on_listWidgetSkins_itemClicked(QListWidgetItem* item)
{
	QSettings reader((qApp->applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk"), QSettings::IniFormat);
	skinFile = (qApp->applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk");
	ui->labelSkinAuthor->setText(reader.value("skinAuthor", "").toString());
	ui->labelSkinVersion->setText(reader.value("skinVersion", "").toString());
	ui->plainTextEditSkinDescription->setPlainText(reader.value("skinDescription", "").toString());
}
