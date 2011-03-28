/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: dialogskinpreview.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : A preview dialog for a given skin
 ********************************************************************************************************/

#include "dialogskinpreview.h"
#include "ui_dialogskinpreview.h"

DialogSkinPreview::DialogSkinPreview(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogSkinPreview)
{
	ui->setupUi(this);

	this->setWindowTitle(tr("Skin Preview"));
	this->setWindowIcon(QIcon(":/Resource/Quazaa48.png"));
}

DialogSkinPreview::~DialogSkinPreview()
{
	delete ui;
}

void DialogSkinPreview::changeEvent(QEvent* e)
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

void DialogSkinPreview::loadSkin(QString file)
{
	QSettings reader(file, QSettings::IniFormat);
	ui->lineEditName->setText(reader.value("skinName", "").toString());
	ui->lineEditAuthor->setText(reader.value("skinAuthor", "").toString());
	ui->lineEditVersion->setText(reader.value("skinVersion", "").toString());
	ui->plainTextEditDescription->setPlainText(reader.value("skinDescription", "").toString());
	setStyleSheet(reader.value("standardItems", "").toString());
}
