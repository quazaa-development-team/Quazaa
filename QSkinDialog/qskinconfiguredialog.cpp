/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: qskinconfiguredialog.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : An example configuration dialog for QSkinDialog.
 ********************************************************************************************************/
#include "qskinconfiguredialog.h"
#include "ui_qskinconfiguredialog.h"
#include "dialogskinpreview.h"
#include "qskinsettings.h"

QSkinConfigureDialog::QSkinConfigureDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QSkinConfigureDialog)
{
	ui->setupUi(this);

	QDir dir(QApplication::applicationDirPath() + "/Skin/");
	QFileInfoList skinDirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
	foreach(QFileInfo i, skinDirs)
	{
		QListWidgetItem *item = new QListWidgetItem(i.fileName());
		ui->listWidgetSkins->addItem(item);
	}

	connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SIGNAL(closed()));
	connect(this, SIGNAL(skinChanged()), &skinSettings, SIGNAL(skinChanged()));
}

QSkinConfigureDialog::~QSkinConfigureDialog()
{
	delete ui;
}

void QSkinConfigureDialog::changeEvent(QEvent *e)
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

void QSkinConfigureDialog::on_listWidgetSkins_itemClicked(QListWidgetItem* item)
{
	ui->pushButtonOK->setEnabled(true);
	QSettings reader((QApplication::applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk"), QSettings::IniFormat);
	skinFile = (QApplication::applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk");
	tempSkinName = reader.value("skinName", "").toString();
	tempSkinAuthor = reader.value("skinAuthor", "").toString();
	tempSkinVersion = reader.value("skinVersion", "").toString();
	tempSkinDescription = reader.value("skinDescription", "").toString();
	tempWindowFrameTopLeftStyleSheet = reader.value("windowFrameTopLeftStyleSheet", "border-image: url(:/Resource/frameTopLeft.png);").toString();
	tempWindowFrameLeftStyleSheet = reader.value("windowFrameLeftStyleSheet", "border-image: url(:/Resource/frameLeft.png); border-left: 1; border-top: 10;").toString();
	tempWindowFrameBottomLeftStyleSheet = reader.value("windowFrameBottomLeftStyleSheet", "border-image: url(:/Resource/frameBottomLeft.png);").toString();
	tempWindowFrameTopStyleSheet = reader.value("windowFrameTopStyleSheet", "QFrame#windowFrameTop { border-left: 85; padding-left: -85; border-image: url(:/Resource/frameTop.png); }").toString();
	tempWindowFrameBottomStyleSheet = reader.value("windowFrameBottomStyleSheet", "border-image: url(:/Resource/frameBottom.png); border-bottom: 1;").toString();
	tempWindowFrameTopRightStyleSheet = reader.value("windowFrameTopRightStyleSheet", "border-image: url(:/Resource/frameTopRight.png);").toString();
	tempWindowFrameRightStyleSheet = reader.value("windowFrameRightStyleSheet", "QFrame { border-image: url(:/Resource/frameRight.png); border-right: 1; border-top: 10; }").toString();
	tempWindowFrameBottomRightStyleSheet = reader.value("windowFrameBottomRightStyleSheet", "border-image: url(:/Resource/frameBottomRight.png);").toString();
	tempTitlebarFrameStyleSheet = reader.value("titlebarFrameStyleSheet", "QFrame { background-color: transparent; }").toString();
	tempTitlebarButtonsFrameStyleSheet = reader.value("titlebarButtonsFrameStyleSheet", "QFrame#titlebarButtonsFrame { padding-top: -1; padding-bottom: 10; }").toString();
	tempMinimizeButtonStyleSheet = reader.value("minimizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/minButton.png); } QToolButton:hover { border-image: url(:/Resource/minButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/minButtonD.png); }").toString();
	tempMaximizeButtonStyleSheet = reader.value("maximizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/maxButton.png); } QToolButton:hover { border-image: url(:/Resource/maxButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/maxButtonD.png); } QToolButton:checked { border-image: url(:/Resource/restoreButton.png); } QToolButton:checked:hover { border-image: url(:/Resource/restoreButtonH.png); } QToolButton:checked:disabled { border-image: url(:/Resource/restoreButtonD.png); }").toString();
	tempCloseButtonStyleSheet = reader.value("closeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/quitButton.png); } QToolButton:hover { border-image: url(:/Resource/quitButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/quitButtonD.png); }").toString();
	tempWindowTextStyleSheet = reader.value("windowTextStyleSheet", "padding-left: -2px; padding-right: -2px; padding-bottom: 2px; font-weight: bold; font-size: 16px; color: rgb(255, 255, 255);").toString();
	tempWindowIconVisible = reader.value("windowIconVisible", true).toBool();
	tempWindowIconSize = reader.value("windowIconSize", QSize(20, 20)).toSize();

	ui->lineEditAuthor->setText(tempSkinAuthor);
	ui->lineEditVersion->setText(tempSkinVersion);
	ui->plainTextEditDescription->setPlainText(tempSkinDescription);
}

void QSkinConfigureDialog::on_pushButtonPreview_clicked()
{
	if (ui->listWidgetSkins->currentRow() != -1)
	{
		DialogSkinPreview *dlgSkinPreview = new DialogSkinPreview(true, true, this);
		dlgSkinPreview->loadSkin((QApplication::applicationDirPath() + "/Skin/" + ui->listWidgetSkins->currentItem()->text() + "/" + ui->listWidgetSkins->currentItem()->text() + ".qsk"));
		dlgSkinPreview->exec();
	}
}

void QSkinConfigureDialog::on_pushButtonOK_clicked()
{
	QSettings m_qSettings("Qt", "QSkinDialog");
	m_qSettings.setValue("skinFile", skinFile);

	skinSettings.skinName = tempSkinName;
	skinSettings.skinAuthor = tempSkinAuthor;
	skinSettings.skinVersion = tempSkinVersion;
	skinSettings.skinDescription = tempSkinDescription;
	skinSettings.windowFrameTopLeftStyleSheet = tempWindowFrameTopLeftStyleSheet;
	skinSettings.windowFrameLeftStyleSheet = tempWindowFrameLeftStyleSheet;
	skinSettings.windowFrameBottomLeftStyleSheet = tempWindowFrameBottomLeftStyleSheet;
	skinSettings.windowFrameTopStyleSheet = tempWindowFrameTopStyleSheet;
	skinSettings.windowFrameBottomStyleSheet = tempWindowFrameBottomStyleSheet;
	skinSettings.windowFrameTopRightStyleSheet = tempWindowFrameTopRightStyleSheet;
	skinSettings.windowFrameRightStyleSheet = tempWindowFrameRightStyleSheet;
	skinSettings.windowFrameBottomRightStyleSheet = tempWindowFrameBottomRightStyleSheet;
	skinSettings.titlebarFrameStyleSheet = tempTitlebarFrameStyleSheet;
	skinSettings.titlebarButtonsFrameStyleSheet = tempTitlebarButtonsFrameStyleSheet;
	skinSettings.minimizeButtonStyleSheet = tempMinimizeButtonStyleSheet;
	skinSettings.maximizeButtonStyleSheet = tempMaximizeButtonStyleSheet;
	skinSettings.closeButtonStyleSheet = tempCloseButtonStyleSheet;
	skinSettings.windowTextStyleSheet = tempWindowTextStyleSheet;
	skinSettings.windowIconVisible = tempWindowIconVisible;
	skinSettings.windowIconSize = tempWindowIconSize;

	emit skinChanged();
	emit closed();
}
