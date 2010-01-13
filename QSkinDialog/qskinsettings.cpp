/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: mainwindow.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : This is where the style sheets for he current skin will actually be
 *				  held, loaded and saved.
 ********************************************************************************************************/
#include "qskinsettings.h"
#include <QFileDialog>
#include <QApplication>
#include <QSettings>

QSkinSettings skinSettings;

QSkinSettings::QSkinSettings(QObject *parent) :
	QObject(parent)
{
}

void QSkinSettings::loadSkin(QString fileName)
{
	QSettings reader(fileName, QSettings::IniFormat);
	skinName = reader.value("skinName", "").toString();
	skinAuthor = reader.value("skinAuthor", "").toString();
	skinVersion = reader.value("skinVersion", "").toString();
	skinDescription = reader.value("skinDescription", "").toString();
	windowFrameTopLeftStyleSheet = reader.value("windowFrameTopLeftStyleSheet", "border-image: url(:/Resource/frameTopLeft.png);").toString();
	windowFrameLeftStyleSheet = reader.value("windowFrameLeftStyleSheet", "border-image: url(:/Resource/frameLeft.png); border-left: 1; border-top: 10;").toString();
	windowFrameBottomLeftStyleSheet = reader.value("windowFrameBottomLeftStyleSheet", "border-image: url(:/Resource/frameBottomLeft.png);").toString();
	windowFrameTopStyleSheet = reader.value("windowFrameTopStyleSheet", "QFrame#windowFrameTop { border-left: 85; padding-left: -85; border-image: url(:/Resource/frameTop.png); }").toString();
	windowFrameBottomStyleSheet = reader.value("windowFrameBottomStyleSheet", "border-image: url(:/Resource/frameBottom.png); border-bottom: 1;").toString();
	windowFrameTopRightStyleSheet = reader.value("windowFrameTopRightStyleSheet", "border-image: url(:/Resource/frameTopRight.png);").toString();
	windowFrameRightStyleSheet = reader.value("windowFrameRightStyleSheet", "QFrame { border-image: url(:/Resource/frameRight.png); border-right: 1; border-top: 10; }").toString();
	windowFrameBottomRightStyleSheet = reader.value("windowFrameBottomRightStyleSheet", "border-image: url(:/Resource/frameBottomRight.png);").toString();
	titlebarFrameStyleSheet = reader.value("titlebarFrameStyleSheet", "QFrame { background-color: transparent; }").toString();
	titlebarButtonsFrameStyleSheet = reader.value("titlebarButtonsFrameStyleSheet", "QFrame#titlebarButtonsFrame { padding-top: -1; padding-bottom: 10; }").toString();
	minimizeButtonStyleSheet = reader.value("minimizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/minButton.png); } QToolButton:hover { border-image: url(:/Resource/minButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/minButtonD.png); }").toString();
	maximizeButtonStyleSheet = reader.value("maximizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/maxButton.png); } QToolButton:hover { border-image: url(:/Resource/maxButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/maxButtonD.png); } QToolButton:checked { border-image: url(:/Resource/restoreButton.png); } QToolButton:checked:hover { border-image: url(:/Resource/restoreButtonH.png); } QToolButton:checked:disabled { border-image: url(:/Resource/restoreButtonD.png); }").toString();
	closeButtonStyleSheet = reader.value("closeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/quitButton.png); } QToolButton:hover { border-image: url(:/Resource/quitButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/quitButtonD.png); }").toString();
	windowTextStyleSheet = reader.value("windowTextStyleSheet", "padding-left: -2px; padding-right: -2px; padding-bottom: 2px; font-weight: bold; font-size: 16px; color: rgb(255, 255, 255);").toString();
	windowIconVisible = reader.value("windowIconVisible", true).toBool();
	windowIconSize = reader.value("windowIconSize", QSize(20, 20)).toSize();
}

void QSkinSettings::saveSkin(QString fileName)
{
	QSettings writer(fileName, QSettings::IniFormat);
	writer.setValue("skinName", skinName);
	writer.setValue("skinAuthor", skinAuthor);
	writer.setValue("skinVersion", skinVersion);
	writer.setValue("skinDescription", skinDescription);
	writer.setValue("windowFrameTopLeftStyleSheet", windowFrameTopLeftStyleSheet);
	writer.setValue("windowFrameLeftStyleSheet", windowFrameLeftStyleSheet);
	writer.setValue("windowFrameBottomLeftStyleSheet", windowFrameBottomLeftStyleSheet);
	writer.setValue("windowFrameTopStyleSheet", windowFrameTopStyleSheet);
	writer.setValue("windowFrameBottomStyleSheet", windowFrameBottomStyleSheet);
	writer.setValue("windowFrameTopRightStyleSheet", windowFrameTopRightStyleSheet);
	writer.setValue("windowFrameRightStyleSheet", windowFrameRightStyleSheet);
	writer.setValue("windowFrameBottomRightStyleSheet", windowFrameBottomRightStyleSheet);
	writer.setValue("titlebarFrameStyleSheet", titlebarFrameStyleSheet);
	writer.setValue("titlebarButtonsFrameStyleSheet", titlebarButtonsFrameStyleSheet);
	writer.setValue("minimizeButtonStyleSheet", minimizeButtonStyleSheet);
	writer.setValue("maximizeButtonStyleSheet", maximizeButtonStyleSheet);
	writer.setValue("closeButtonStyleSheet", closeButtonStyleSheet);
	writer.setValue("windowTextStyleSheet", windowTextStyleSheet);
	writer.setValue("windowIconVisible", windowIconVisible);
	writer.setValue("windowIconSize", windowIconSize);
}
