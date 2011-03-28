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

QSkinSettings::QSkinSettings(QObject* parent) :
	QObject(parent)
{
}

void QSkinSettings::loadSkin(QString fileName)
{
	QSettings reader(fileName, QSettings::IniFormat);
	reader.sync();
	skinName = reader.value("skinName", "").toString();
	skinAuthor = reader.value("skinAuthor", "").toString();
	skinVersion = reader.value("skinVersion", "").toString();
	skinDescription = reader.value("skinDescription", "").toString();

	// Main Window Frame
	windowFrameTopLeftStyleSheet = reader.value("windowFrameTopLeftStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-top: 2px solid darkgrey;\nborder-left: 2px solid darkgrey;\nborder-top-left-radius: 4px;\nmin-height: 14px;\nmax-height: 14px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	windowFrameLeftStyleSheet = reader.value("windowFrameLeftStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-left: 2px solid darkgrey;\nmin-width: 6px;\nmax-width: 6px;").toString();
	windowFrameBottomLeftStyleSheet = reader.value("windowFrameBottomLeftStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-bottom: 2px solid darkgrey;\nborder-left: 2px solid darkgrey;\nborder-bottom-left-radius: 4px;\nmin-height: 6px;\nmax-height: 6px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	windowFrameTopStyleSheet = reader.value("windowFrameTopStyleSheet", "QFrame#windowFrameTop {\n\tbackground-color: lightgrey;\n\tborder: 0px solid transparent;\n\tborder-top: 2px solid darkgrey;\n\tpadding-top: 3px;\n\tpadding-bottom: 3px;\n\tmin-height: 30px;\n\tmax-height: 30px;\n}").toString();
	windowFrameBottomStyleSheet = reader.value("windowFrameBottomStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-bottom: 2px solid darkgrey;\nmin-height: 6px;\nmax-height: 6px;").toString();
	windowFrameTopRightStyleSheet = reader.value("windowFrameTopRightStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-top: 2px solid darkgrey;\nborder-right: 2px solid darkgrey;\nborder-top-right-radius: 4px;\nmin-height: 14px;\nmax-height: 14px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	windowFrameRightStyleSheet = reader.value("windowFrameRightStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-right: 2px solid darkgrey;\nmin-width: 6px;\nmax-width: 6px;").toString();
	windowFrameBottomRightStyleSheet = reader.value("windowFrameBottomRightStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-bottom: 2px solid darkgrey;\nborder-right: 2px solid darkgrey;\nborder-bottom-right-radius: 4px;\nmin-height: 6px;\nmax-height: 6px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	titlebarButtonsFrameStyleSheet = reader.value("titlebarButtonsFrameStyleSheet", "QFrame {\n\tborder: 2px inset darkgrey;\n\tborder-radius: 6px;\n\tpadding-left: 3px;\n\tpadding-right: 3px;\n\tmargin-right: 2px;\n}").toString();
	minimizeButtonStyleSheet = reader.value("minimizeButtonStyleSheet", "QToolButton { \n\tborder: 2px outset darkgrey;\n\tborder-radius: 4px;\n\tbackground-color: lightgrey; \n\timage: url(:/Resource/minButton.png);\n\tmargin-right: 2px;\n\tmin-width: 17px;\n\tmax-width: 17px;\n\tmin-height: 17px;\n\tmax-height: 17px;\n} \n\nQToolButton:hover {\n\tbackground-color: white;\n\timage: url(:/Resource/minButtonH.png); \n} \n\nQToolButton:disabled { \n\tborder-image: url(:/Resource/minButtonD.png); \n}").toString();
	maximizeButtonStyleSheet = reader.value("maximizeButtonStyleSheet", "QToolButton {\n\tborder: 2px outset darkgrey;\n\tborder-radius: 4px;\n\tbackground-color: lightgrey; \n\timage: url(:/Resource/maxButton.png);\n\tmin-width: 17px;\n\tmax-width: 17px;\n\tmin-height: 17px;\n\tmax-height: 17px;\n}\n\nQToolButton:hover {\n\tbackground-color: white;\n\timage: url(:/Resource/maxButtonH.png);\n}\n\nQToolButton:disabled {\n\tborder-image: url(:/Resource/maxButtonD.png);\n}\n\nQToolButton:checked { /*The button will be checked when maximized is set*/\n\timage: url(:/Resource/restoreButton.png);\n}\n\nQToolButton:checked:hover {\n\timage: url(:/Resource/restoreButtonH.png);\n}\n\nQToolButton:checked:disabled {\n\tborder-image: url(:/Resource/restoreButtonD.png);\n}").toString();
	closeButtonStyleSheet = reader.value("closeButtonStyleSheet", "QToolButton {\n\tborder: 2px outset darkgrey;\n\tborder-radius: 4px;\n\tbackground-color: lightgrey; \n\timage: url(:/Resource/closeButton.png);\n\tmargin-left: 2px;\n\tmin-width: 17px;\n\tmax-width: 17px;\n\tmin-height: 17px;\n\tmax-height: 17px;\n}\n\nQToolButton:hover {\n\tbackground-color: white;\n\timage: url(:/Resource/closeButtonH.png);\n}\n\nQToolButton:disabled {\n\tborder-image: url(:/Resource/closeButtonD.png);\n}").toString();
	windowFrameTopSpacerStyleSheet = reader.value("windowFrameTopSpacerStyleSheet", "QFrame#windowFrameTopSpacer {\n\tborder: 0px solid transparent;\n}").toString();
	windowTextStyleSheet = reader.value("windowTextStyleSheet", "border: 0px solid transparent;\npadding-left: -2px;\npadding-right: -2px;\npadding-bottom: 2px;\nfont-weight: bold;\nfont-size: 16px;\ncolor: black;").toString();
	windowIconFrameStyleSheet = reader.value("windowIconFrameStyleSheet", "QFrame#windowIconFrame {\n\tborder-left: 10px;\n\tborder-right: 11px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 0px solid transparent;\n\tqproperty-iconSize: 25px 25px;\n}").toString();
	windowIconVisible = reader.value("windowIconVisible", true).toBool();

	// Child Window Frame
	childWindowFrameTopLeftStyleSheet = reader.value("childWindowFrameTopLeftStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-top: 2px solid darkgrey;\nborder-left: 2px solid darkgrey;\nborder-top-left-radius: 4px;\nmin-height: 14px;\nmax-height: 14px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	childWindowFrameLeftStyleSheet = reader.value("childWindowFrameLeftStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-left: 2px solid darkgrey;\nmin-width: 6px;\nmax-width: 6px;").toString();
	childWindowFrameBottomLeftStyleSheet = reader.value("childWindowFrameBottomLeftStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-bottom: 2px solid darkgrey;\nborder-left: 2px solid darkgrey;\nborder-bottom-left-radius: 4px;\nmin-height: 6px;\nmax-height: 6px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	childWindowFrameTopStyleSheet = reader.value("childWindowFrameTopStyleSheet", "QFrame#windowFrameTop {\n\tbackground-color: lightgrey;\n\tborder: 0px solid transparent;\n\tborder-top: 2px solid darkgrey;\n\tpadding-top: 3px;\n\tpadding-bottom: 3px;\n\tmin-height: 30px;\n\tmax-height: 30px;\n}").toString();
	childWindowFrameBottomStyleSheet = reader.value("childWindowFrameBottomStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-bottom: 2px solid darkgrey;\nmin-height: 6px;\nmax-height: 6px;").toString();
	childWindowFrameTopRightStyleSheet = reader.value("childWindowFrameTopRightStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-top: 2px solid darkgrey;\nborder-right: 2px solid darkgrey;\nborder-top-right-radius: 4px;\nmin-height: 14px;\nmax-height: 14px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	childWindowFrameRightStyleSheet = reader.value("childWindowFrameRightStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-right: 2px solid darkgrey;\nmin-width: 6px;\nmax-width: 6px;").toString();
	childWindowFrameBottomRightStyleSheet = reader.value("childWindowFrameBottomRightStyleSheet", "background-color: lightgrey;\nborder: 0px solid transparent;\nborder-bottom: 2px solid darkgrey;\nborder-right: 2px solid darkgrey;\nborder-bottom-right-radius: 4px;\nmin-height: 6px;\nmax-height: 6px;\nmin-width: 6px;\nmax-width: 6px;").toString();
	childTitlebarButtonsFrameStyleSheet = reader.value("childTitlebarButtonsFrameStyleSheet", "QFrame {\n\tborder: 2px inset darkgrey;\n\tborder-radius: 6px;\n\tpadding-left: 3px;\n\tpadding-right: 3px;\n\tmargin-right: 2px;\n}").toString();
	childMinimizeButtonStyleSheet = reader.value("childMinimizeButtonStyleSheet", "QToolButton { \n\tborder: 2px outset darkgrey;\n\tborder-radius: 4px;\n\tbackground-color: lightgrey; \n\timage: url(:/Resource/minButton.png);\n\tmargin-right: 2px;\n\tmin-width: 17px;\n\tmax-width: 17px;\n\tmin-height: 17px;\n\tmax-height: 17px;\n} \n\nQToolButton:hover {\n\tbackground-color: white;\n\timage: url(:/Resource/minButtonH.png); \n} \n\nQToolButton:disabled { \n\tborder-image: url(:/Resource/minButtonD.png); \n}").toString();
	childMaximizeButtonStyleSheet = reader.value("childMaximizeButtonStyleSheet", "QToolButton {\n\tborder: 2px outset darkgrey;\n\tborder-radius: 4px;\n\tbackground-color: lightgrey; \n\timage: url(:/Resource/maxButton.png);\n\tmin-width: 17px;\n\tmax-width: 17px;\n\tmin-height: 17px;\n\tmax-height: 17px;\n}\n\nQToolButton:hover {\n\tbackground-color: white;\n\timage: url(:/Resource/maxButtonH.png);\n}\n\nQToolButton:disabled {\n\tborder-image: url(:/Resource/maxButtonD.png);\n}\n\nQToolButton:checked { /*The button will be checked when maximized is set*/\n\timage: url(:/Resource/restoreButton.png);\n}\n\nQToolButton:checked:hover {\n\timage: url(:/Resource/restoreButtonH.png);\n}\n\nQToolButton:checked:disabled {\n\tborder-image: url(:/Resource/restoreButtonD.png);\n}").toString();
	childCloseButtonStyleSheet = reader.value("childCloseButtonStyleSheet", "QToolButton {\n\tborder: 2px outset darkgrey;\n\tborder-radius: 4px;\n\tbackground-color: lightgrey; \n\timage: url(:/Resource/closeButton.png);\n\tmargin-left: 2px;\n\tmin-width: 17px;\n\tmax-width: 17px;\n\tmin-height: 17px;\n\tmax-height: 17px;\n}\n\nQToolButton:hover {\n\tbackground-color: white;\n\timage: url(:/Resource/closeButtonH.png);\n}\n\nQToolButton:disabled {\n\tborder-image: url(:/Resource/closeButtonD.png);\n}").toString();
	childWindowFrameTopSpacerStyleSheet = reader.value("childWindowFrameTopSpacerStyleSheet", "QFrame#windowFrameTopSpacer {\n\tborder: 0px solid transparent;\n}").toString();
	childWindowTextStyleSheet = reader.value("childWindowTextStyleSheet", "border: 0px solid transparent;\npadding-left: -2px;\npadding-right: -2px;\npadding-bottom: 2px;\nfont-weight: bold;\nfont-size: 16px;\ncolor: black;").toString();
	childWindowIconFrameStyleSheet = reader.value("childWindowIconFrameStyleSheet", "QFrame#windowIconFrame {\n\tborder-left: 10px;\n\tborder-right: 11px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 0px solid transparent;\n\tqproperty-iconSize: 20px 20px;\n}").toString();
	childWindowIconVisible = reader.value("childWindowIconVisible", true).toBool();

	// Splash Screen
	splashBackground = reader.value("splashBackground", "QFrame#frameSplashBackground {\n\tbackground-color: lightgrey;\n\tborder: 4px outset darkgrey;\n\tborder-radius: 4px;\n}").toString();
	splashLogo = reader.value("splashLogo", "background-color: transparent;\nmin-height: 172px;\nmax-height: 172px;\nmin-width: 605px;\nmax-width: 605px;\nborder-image: url(:/Resource/QuazaaLogo.png);").toString();
	splashFooter = reader.value("splashFooter", "QFrame#frameSplashFooter {\n\tbackground-color: lightgrey;\n\tborder: 2px inset darkgrey;\n\tmin-height: 28;\n\tmax-height: 28;\n}").toString();
	splashProgress = reader.value("splashProgress", "QProgressBar {\n\tfont-weight: bold;\n\tcolor: black;\n\tmax-height: 12px;\n\tmin-height: 14px;\n\tmax-width: 220px;\n\tmin-width: 220px;\n\tmargin-right: 4px;\n\tborder: 2px inset darkgrey;\n\ttext-align: center;\n\tborder-radius: 4px;\n\tbackground-color: transparent;\n}\n\nQProgressBar::chunk {\n\tbackground-color: darkgrey;\n}").toString();
	splashStatus = reader.value("splashStatus", "font-weight: bold;\ncolor: black;\nbackground-color: transparent;\npadding-left: 5px;").toString();

	// Standard Items
	standardItems = reader.value("standardItems", "* {color: black;} QDialog {background-color: rgb(236, 236, 236);} .QWidget {background-color: lightgrey;} QTabWidget::pane {background-color: rgb(236, 236, 236);border: 1px solid darkgrey;} QTabWidget::tab-bar {left: 5px;} QTabBar::tab {border: 2px outset darkgrey;border-bottom: none;border-radius: 4px;border-bottom-left-radius: none;border-bottom-right-radius: none;background-color: lightgrey;padding: 2px; } QTabBar::tab:selected, QTabBar::tab:hover {border-color: rgb(124, 126, 124);background-color: white; } QGroupBox {border: 1px solid darkgrey;color: black;margin-top: 6px;padding-top: 6px;} QGroupBox:title {subcontrol-origin: margin;subcontrol-position: top center;padding: 0 1px;} QGroupBox QLabel {alignment: top center;\n}\n\nQMenu {\n\t background-color: lightgrey;\n     border: 1px solid rgb(124, 126, 124);\n }\n\n QMenu::item {\npadding: 2px 25px 2px 20px;\n     background-color: transparent;\n }\n\n QMenu::item:selected { \n\tborder: 1px solid rgb(124, 126, 124);\n\tcolor: black;\n\tbackground-color: rgb(217, 217, 217);\n }\n\nQMenu::right-arrow:selected {\n\timage: url(:/Resource/Right-Arrow.png);\n }\n\nQMenu:left-arrow:selected {\n\timage: url(:/Resource/Left-Arrow.png);\n}\n\nQMenu::icon:checked { \n     border: 1px inset rgb(124, 126, 124);\n     position: absolute;\n\tbackground-color: rgb(210, 222, 242);\n }\n\n QMenu::separator {\n     height: 1px;\n     background: rgb(124, 126, 124);\n     margin-left: 10px;\n     margin-right: 5px;\n }\n\nQSplitter::handle {\n\tborder: 2px outset darkgrey;\n\tbackground-color: lightgrey;\n}\n\n QSplitter::handle:horizontal {\n\tborder-top: 0px;\n\tborder-bottom: 0px;\n     width: 4px;\n }\n\n QSplitter::handle:vertical {\n\tborder-left: 0px;\n\tborder-right: 0px;\n     height: 4px;\n }\n\n QSplitter::handle:pressed {\n      background-color: rgb(236, 236, 236);\n }\n\n QStatusBar {\n\tborder: 2px inset grey;\n\tbackground-color: lightgrey;\n }\n\n QStatusBar QLabel {\n    border: 0px solid grey;\n\tborder-left: 2px solid rgb(184, 184, 184);\n }\n\nQToolButton {\n\tborder: 2px outset darkgrey;\n\tbackground-color: lightgrey;\n\tborder-radius: 4px;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tbackground-color: white;\n}\n\nQToolButton:disabled {\n\tcolor: rgb(140, 140, 140);\n\tborder-color: darkgrey;\n\tbackground-color: rgb(190, 190, 190);\n}\n\nQPushButton {\n\tborder: 2px outset darkgrey;\n\tbackground-color: lightgrey;\n\tborder-radius: 4px;\n\tmin-width: 58;\n\tpadding: 5px;\n}\n\nQPushButton:hover, QPushButton:checked {\n\tbackground-color: white;\n}\n\nQPushButton:disabled {\n\tcolor: rgb(140, 140, 140);\n\tborder-color: darkgrey;\n\tbackground-color: rgb(190, 190, 190);\n}\n\nQProgressBar {\n\tfont-weight: bold;\n\tcolor: black;\n\tmax-height: 12px;\n\tmin-height: 14px;\n\tmax-width: 220px;\n\tmin-width: 220px;\n\tmargin-right: 4px;\n\tborder: 2px inset darkgrey;\n\ttext-align: center;\n\tborder-radius: 4px;\n\tbackground-color: transparent;\n}\n\nQProgressBar::chunk {\n\tbackground-color: darkgrey;\n}\n\n QRadioButton::indicator {\n     width: 13px;\n     height: 13px;\n }\n\n QRadioButton::indicator::unchecked {\n     image: url(:/Resource/radiobuttonUnchecked.png);\n }\n\n QRadioButton::indicator:unchecked:hover {\n     image: url(:/Resource/radiobuttonUncheckedHover.png);\n }\n\n QRadioButton::indicator:unchecked:pressed {\n     image: url(:/Resource/radiobuttonUncheckedPressed.png);\n }\n\n QRadioButton::indicator::checked {\n     image: url(:/Resource/radiobuttonChecked.png);\n }\n\n QRadioButton::indicator:checked:hover {\n     image: url(:/Resource/radiobuttonCheckedHover.png);\n }\n\n QRadioButton::indicator:checked:pressed {\n     image: url(:/Resource/radiobuttonCheckedPressed.png);\n }\n\n QCheckBox {\n     spacing: 5px;\n }\n\n QCheckBox::indicator {\n     width: 13px;\n     height: 13px;\n }\n\n QCheckBox::indicator:unchecked {\n     image: url(:/Resource/checkboxUnchecked.png);\n }\n\n QCheckBox::indicator:unchecked:hover {\n     image: url(:/Resource/checkboxUncheckedHover.png);\n }\n\n QCheckBox::indicator:unchecked:pressed {\n     image: url(:/Resource/checkboxUncheckedPressed.png);\n }\n\n QCheckBox::indicator:checked {\n     image: url(:/Resource/checkboxChecked.png);\n }\n\n QCheckBox::indicator:checked:hover {\n     image: url(:/Resource/checkboxCheckedHover.png);\n }\n\n QCheckBox::indicator:checked:pressed {\n     image: url(:/Resource/checkboxCheckedPressed.png);\n }\n\n QCheckBox::indicator:indeterminate:hover {\n     image: url(:/images/checkbox_indeterminate_hover.png);\n }\n\n QCheckBox::indicator:indeterminate:pressed {\n     image: url(:/images/checkbox_indeterminate_pressed.png);\n }\n\n QLineEdit {\n\tborder: 1px solid darkgrey;\n     padding: 0 8px;\n     background: white;\n     selection-background-color: darkgray;\n }\n\n\n QLineEdit[echoMode=\"2\"] {\n     lineedit-password-character: 9679;\n }\n\n\n QLineEdit:read-only {\n\tborder-color: rgb(245, 245, 245);\n    background: rgb(245, 245, 245);\n}\n\nQTextEdit {\n\tborder: 1px solid rgb(214, 212, 215);\n     background: white;\n     selection-background-color: darkgray;\n }\n\n QTextEdit:read-only {\n\tborder-color: rgb(245, 245, 245);\n    background: rgb(245, 245, 245);\n}\n\nQPlainTextEdit {\n\tborder: 1px solid darkgrey;\n     background: white;\n     selection-background-color: darkgray;\n }\n\n QPlainTextEdit:read-only {\n\tborder-color: rgb(245, 245, 245);\n    background: rgb(245, 245, 245);\n} \n\n\nQLabel[frameShape=\"6\"] {\n\tborder: 1px solid darkgrey;\n}\n\n QScrollBar:horizontal {\n     border: 1px solid rgb(234, 232, 235);\n     background: rgb(206, 206, 206);\n     height: 15px;\n     margin: 0px 20px 0 20px;\n }\n\n QScrollBar::handle:horizontal {\n     border: 2px outset darkgrey;\n\tborder-radius: 4px;\n     background: lightgrey;\n     min-width: 20px;\n }\n\n QScrollBar::handle:horizontal:hover {\n     background:rgb(240, 240, 240);\n }\n\n QScrollBar::add-line:horizontal {\n     border: 2px outset darkgrey;\n\tborder-radius: 4px;\n     background: lightgrey;\n     width: 20px;\n     subcontrol-position: right;\n     subcontrol-origin: margin;\n }\n\n QScrollBar::add-line:horizontal:hover {\n     background:rgb(240, 240, 240);\n }\n\n QScrollBar::sub-line:horizontal {\n     border: 2px outset darkgrey;\n\tborder-radius: 4px;\n     background: lightgrey;\n     width: 20px;\n     subcontrol-position: left;\n     subcontrol-origin: margin;\n }\n\n QScrollBar::sub-line:horizontal:hover {\n     background:rgb(240, 240, 240);\n }\n\n QScrollBar:left-arrow:horizontal  { \n\timage: url(:/Resource/scrollLeftArrow.png);\n }\n\nQScrollBar::right-arrow:horizontal {\n\timage: url(:/Resource/scrollRightArrow.png);\n}\n\n QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {\n     background: none;\n }\n\n  QScrollBar:vertical {\n     border: 1px solid rgb(234, 232, 235);\n     background: lightgrey;\n     width: 15px;\n     margin: 20px 0 20px 0;\n  }\n\n  QScrollBar::handle:vertical {\n     border: 2px outset darkgrey;\n\tborder-radius: 4px;\n     background: rgb(217, 217, 217);\n     min-height: 20px;\n  }\n\n QScrollBar::handle:vertical:hover {\n     background:rgb(240, 240, 240);\n }\n\n  QScrollBar::add-line:vertical {\n     border: 2px outset darkgrey;\n\tborder-radius: 4px;\n     background: lightgrey;\n     height: 20px;\n     subcontrol-position: bottom;\n     subcontrol-origin: margin;\n  }\n\n QScrollBar::add-line:vertical:hover {\n     background:rgb(240, 240, 240);\n }\n\n  QScrollBar::sub-line:vertical {\n     border: 2px outset darkgrey;\n\tborder-radius: 4px;\n     background: lightgrey;\n      height: 20px;\n      subcontrol-position: top;\n      subcontrol-origin: margin;\n  }\n\n QScrollBar::sub-line:vertical:hover {\n     background:rgb(240, 240, 240);\n }\n\n  QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {\n      background: none;\n  }\n\n QScrollBar:up-arrow:vertical  { \n\timage: url(:/Resource/scrollUpArrow.png);\n }\n\nQScrollBar::down-arrow:vertical {\n\timage: url(:/Resource/scrollDownArrow.png);\n}\n\n QSpinBox {\n     padding-right: 15px; \n\tborder: 1px solid darkgrey;\n }\n\n QSpinBox::up-button {\n     subcontrol-origin: border;\n     subcontrol-position: top right; \n\n     width: 16px; \n     border-image: url(:/Resource/spinUp.png) 1;\n     border-width: 1px;\n }\n\n QSpinBox::up-button:hover {\n     border-image: url(:/Resource/spinUpHover.png) 1;\n }\n\n QSpinBox::up-button:pressed {\n     border-image: url(:/Resource/spinUpPressed.png) 1;\n }\n\n QSpinBox::up-button:off {\n     border-image:url(:/Resource/spinUpOff.png) 1;\n }\n\n QSpinBox::up-arrow {\n     image: url(:/Resource/spinUpArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QSpinBox::up-arrow:disabled, QSpinBox::up-arrow:off { \n    image: url(:/Resource/spinUpArrowDisabled.png);\n }\n\n QSpinBox::down-button {\n     subcontrol-origin: border;\n     subcontrol-position: bottom right; \n     width: 16px;\n     border-image: url(:/Resource/spinDown.png) 1;\n     border-width: 1px;\n     border-top-width: 0;\n }\n\n QSpinBox::down-button:hover {\n     border-image: url(:/Resource/spinDownHover.png) 1;\n }\n\n QSpinBox::down-button:pressed {\n     border-image: url(:/Resource/spinDownPressed.png) 1;\n }\n\n QSpinBox::down-button:off {\n     border-image:url(:/Resource/spinDownOff.png) 1;\n }\n\n QSpinBox::down-arrow {\n     image: url(:/Resource/spinDownArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QSpinBox::down-arrow:disabled,\n QSpinBox::down-arrow:off { \n    image: url(:/Resource/spinDownArrowDisabled.png);\n }\n\n QDoubleSpinBox {\n     padding-right: 15px; \n\tborder: 1px solid darkgrey;\n }\n\n QDoubleSpinBox::up-button {\n     subcontrol-origin: border;\n     subcontrol-position: top right; \n\n     width: 16px; \n     border-image: url(:/Resource/spinUp.png) 1;\n     border-width: 1px;\n }\n\n QDoubleSpinBox::up-button:hover {\n     border-image: url(:/Resource/spinUpHover.png) 1;\n }\n\n QDoubleSpinBox::up-button:pressed {\n     border-image: url(:/Resource/spinUpPressed.png) 1;\n }\n\n QDoubleSpinBox::up-button:off {\n     border-image:url(:/Resource/spinUpOff.png) 1;\n }\n\n QDoubleSpinBox::up-arrow {\n     image: url(:/Resource/spinUpArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QDoubleSpinBox::up-arrow:disabled, QSpinBox::up-arrow:off { \n    image: url(:/Resource/spinUpArrowDisabled.png);\n }\n\n QDoubleSpinBox::down-button {\n     subcontrol-origin: border;\n     subcontrol-position: bottom right; \n     width: 16px;\n     border-image: url(:/Resource/spinDown.png) 1;\n     border-width: 1px;\n     border-top-width: 0;\n }\n\n QDoubleSpinBox::down-button:hover {\n     border-image: url(:/Resource/spinDownHover.png) 1;\n }\n\n QDoubleSpinBox::down-button:pressed {\n     border-image: url(:/Resource/spinDownPressed.png) 1;\n }\n\n QDoubleSpinBox::down-button:off {\n     border-image:url(:/Resource/spinDownOff.png) 1;\n }\n\n QDoubleSpinBox::down-arrow {\n     image: url(:/Resource/spinDownArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QDoubleSpinBox::down-arrow:disabled,\n QDoubleSpinBox::down-arrow:off { \n    image: url(:/Resource/spinDownArrowDisabled.png);\n }\n\n\n QTimeEdit {\n     padding-right: 15px; \n\tborder: 1px solid darkgrey;\n }\n\n QTimeEdit::up-button {\n     subcontrol-origin: border;\n     subcontrol-position: top right; \n\n     width: 16px; \n     border-image: url(:/Resource/spinUp.png) 1;\n     border-width: 1px;\n }\n\n QTimeEdit::up-button:hover {\n     border-image: url(:/Resource/spinUpHover.png) 1;\n }\n\n QTimeEdit::up-button:pressed {\n     border-image: url(:/Resource/spinUpPressed.png) 1;\n }\n\n QTimeEdit::up-button:off {\n     border-image:url(:/Resource/spinUpOff.png) 1;\n }\n\n QTimeEdit::up-arrow {\n     image: url(:/Resource/spinUpArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QTimeEdit::up-arrow:disabled, QTimeEdit::up-arrow:off { \n    image: url(:/Resource/spinUpArrowDisabled.png);\n }\n\n QTimeEdit::down-button {\n     subcontrol-origin: border;\n     subcontrol-position: bottom right; \n     width: 16px;\n     border-image: url(:/Resource/spinDown.png) 1;\n     border-width: 1px;\n     border-top-width: 0;\n }\n\n QTimeEdit::down-button:hover {\n     border-image: url(:/Resource/spinDownHover.png) 1;\n }\n\n QTimeEdit::down-button:pressed {\n     border-image: url(:/Resource/spinDownPressed.png) 1;\n }\n\n QTimeEdit::down-button:off {\n     border-image:url(:/Resource/spinDownOff.png) 1;\n }\n\n QTimeEdit::down-arrow {\n     image: url(:/Resource/spinDownArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QTimeEdit::down-arrow:disabled,\n QTimeEdit::down-arrow:off { \n    image: url(:/Resource/spinDownArrowDisabled.png);\n }\n\n QDateTimeEdit {\n     padding-right: 15px; \n\tborder: 1px solid darkgrey;\n }\n\n QDateTimeEdit::up-button {\n     subcontrol-origin: border;\n     subcontrol-position: top right; \n\n     width: 16px; \n     border-image: url(:/Resource/spinUp.png) 1;\n     border-width: 1px;\n }\n\n QDateTimeEdit::up-button:hover {\n     border-image: url(:/Resource/spinUpHover.png) 1;\n }\n\n QDateTimeEdit::up-button:pressed {\n     border-image: url(:/Resource/spinUpPressed.png) 1;\n }\n\n QDateTimeEdit::up-button:off {\n     border-image:url(:/Resource/spinUpOff.png) 1;\n }\n\n QDateTimeEdit::up-arrow {\n     image: url(:/Resource/spinUpArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QDateTimeEdit::up-arrow:disabled, QDateTimeEdit::up-arrow:off { \n    image: url(:/Resource/spinUpArrowDisabled.png);\n }\n\n QDateTimeEdit::down-button {\n     subcontrol-origin: border;\n     subcontrol-position: bottom right; \n     width: 16px;\n     border-image: url(:/Resource/spinDown.png) 1;\n     border-width: 1px;\n     border-top-width: 0;\n }\n\n QDateTimeEdit::down-button:hover {\n     border-image: url(:/Resource/spinDownHover.png) 1;\n }\n\n QDateTimeEdit::down-button:pressed {\n     border-image: url(:/Resource/spinDownPressed.png) 1;\n }\n\n QDateTimeEdit::down-button:off {\n     border-image:url(:/Resource/spinDownOff.png) 1;\n }\n\n QDateTimeEdit::down-arrow {\n     image: url(:/Resource/spinDownArrow.png);\n     width: 7px;\n     height: 7px;\n }\n\n QDateTimeEdit::down-arrow:disabled,\n QDateTimeEdit::down-arrow:off { \n    image: url(:/Resource/spinDownArrowDisabled.png);\n }\n\n QSlider::groove:horizontal {\n     border: 1px solid #999999;\n     height: 2px; \n     position: absolute;\n     background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);\n     margin: 6px 0;\n }\n\n QSlider::handle:horizontal {\n\tborder: 1px solid darkgrey;\n\tbackground-color: lightgrey;\n     width: 8px;\n     margin: -8 0px;\n     position: absolute;\n }\n\n QComboBox {\n     border: 1px solid darkgrey;\n     padding: 1px 18px 1px 3px;\n     min-width: 6em;\n }\n\n QComboBox:editable {\n     background: white;\n }\n\n QComboBox:!editable {\n     background: white;\n}\n\n QComboBox::drop-down {\n\tbackground: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n                                 stop: 0 #D3D3D3, stop: 0.4 #D8D8D8,\n                                 stop: 0.5 #DDDDDD, stop: 1.0 #E1E1E1);\n     subcontrol-origin: padding;\n     subcontrol-position: top right;\n     width: 15px;\n\n     border-left: 1px solid darkgrey;\n }\n\nQComboBox::drop-down:editable {\n      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n                                  stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,\n                                  stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);\n }\n\n /* QComboBox gets the \"on\" state when the popup is open */\n QComboBox:!editable:on, QComboBox::drop-down:editable:on {\n     background: white;\n }\n\n QComboBox:on { /* shift the text when the popup opens */\n     padding-top: 3px;\n     padding-left: 4px;\n }\n\n QComboBox::drop-down:hover {\n     background:rgb(240, 240, 240);\n }\n\n\n QComboBox::down-arrow {\n\timage: url(:/Resource/scrollDownArrow.png);\n }\n\n QComboBox::down-arrow:on { /* shift the arrow when popup is open */\n     top: 1px;\n     left: 1px;\n }\n\n QComboBox QAbstractItemView {\n     border: 2px solid rgb(214, 212, 215);\n\tcolor: black;\n\tselection-color: black;\n     selection-background-color: lightgrey;\n }").toString();

	// ListViews
	listViews = reader.value("listViews", "QAbstractItemView {\n\tborder: 1px solid darkgrey;\n\tcolor: black;\n\tselection-color: black;\n\tselection-background-color: lightgrey;\n\tbackground-color: white;\n}\n\nQHeaderView::section {\n\tbackground-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n\t\tstop: 0 #D3D3D3, stop: 0.4 #D8D8D8,\n\t\tstop: 0.5 #DDDDDD, stop: 1.0 #E1E1E1);\n\tcolor: black;\n\tpadding-left: 4px;\n\tborder: 1px solid rgb(181, 181, 181);\n}\n\n QHeaderView::section:checked\n {\n     background-color: darkgrey;\n }\n\n /* style the sort indicator */\n QHeaderView::down-arrow {\n     image: url(:/Resource/scrollDownArrow.png);\n }\n\n QHeaderView::up-arrow {\n     image: url(:/Resource/scrollUpArrow.png);\n }").toString();

	// Sidebar
	sidebarBackground = reader.value("sidebarBackground", "QFrame {\n\t background-color: rgb(236, 236, 236);\n}\n\nQToolButton {\n\tborder: 1px solid transparent;\n\tborder-radius: 4px;\n\tbackground-color: lightgrey;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: white;\n}").toString();
	sidebarTaskBackground = reader.value("sidebarTaskBackground", "QFrame {\n\tbackground-color: lightgrey;\n}").toString();
	sidebarTaskHeader = reader.value("sidebarTaskHeader", "QToolButton {\n\tbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(255, 255, 255, 255), stop:1 rgba(102, 102, 102, 255));\n\tborder: 2px outset grey;\n\tborder-bottom: 0px;\n\tborder-radius: 3px;\n\tborder-bottom-left-radius: 0px;\n\tborder-bottom-right-radius: 0px;\n\tcolor: black;\n\tfont-size: 16px;\n\tfont-weight: bold;\n}\n\nQToolButton:!checked {\n\tborder-bottom: 3px;\n\tborder-bottom-left-radius: 3px;\n\tborder-bottom-right-radius: 3px;\n}\n\nQToolButton:hover {\n\tcolor: rgb(71,71,71);\n}").toString();
	sidebarUnclickableTaskHeader = reader.value("sidebarUnclickableTaskHeader", "QToolButton {\n\tbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(255, 255, 255, 255), stop:1 rgba(102, 102, 102, 255));\n\tborder: 2px outset grey;\n\tborder-bottom: 0px;\n\tborder-radius: 3px;\n\tborder-bottom-left-radius: 0px;\n\tborder-bottom-right-radius: 0px;\n\tcolor: black;\n\tfont-size: 16px;\n\tfont-weight: bold;\n}").toString();
	addSearchButton = reader.value("addSearchButton", "QToolButton {\n\tbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(255, 255, 255, 255), stop:1 rgba(102, 102, 102, 255));\n\tborder: 2px outset grey;\n\tborder-radius: 3px;\n\tcolor: black;\n\tfont-size: 16px;\n\tfont-weight: bold;\n}\n\nQToolButton:hover {\n\tcolor: rgb(71, 71, 71);\n}").toString();

	// Toolbars
	toolbars = reader.value("toolbars", "QToolBar {\n\tborder: 0px solid transparent;\n\tbackground-color: lightgrey;\n}\n\nQToolBar::handle {\n\tborder-image: url(:/Resource/toolbarGrip.png) repeat;\n\tborder-bottom: 1px;\n\tmax-width: 5px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 1px solid transparent;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n}\n\n QToolButton:pressed {\n     background-color: white;\n }\n\n QToolButton[popupMode=\"1\"] { /* only for MenuButtonPopup */\n     padding-right: 20px; /* make way for the popup button */\n }\n\n/* the subcontrols below are used only in the MenuButtonPopup mode */\n QToolButton::menu-button {\n     border: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\nQToolButton::menu-button:hover {\n    border: 1px solid rgb(124, 126, 124);\n\tborder-left-color: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\n QToolButton::menu-arrow:open {\n     top: 1px; left: 1px; /* shift it a bit */\n}").toString();
	navigationToolbar = reader.value("navigationToolbar", "QToolBar {\n\tborder: 0px solid grey;\n\tbackground-color: lightgrey;\n}\n\nQToolBar::handle {\n\tborder-image: url(:/Resource/toolbarGrip.png) repeat;\n\tborder-bottom: 1px;\n\tmax-width: 5px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 1px solid transparent;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n}\n\n QToolButton:pressed {\n     background-color: white;\n }\n\n QToolButton[popupMode=\"1\"] { /* only for MenuButtonPopup */\n     padding-right: 20px; /* make way for the popup button */\n }\n\n/* the subcontrols below are used only in the MenuButtonPopup mode */\n QToolButton::menu-button {\n     border: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\nQToolButton::menu-button:hover {\n    border: 1px solid rgb(124, 126, 124);\n\tborder-left-color: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\n QToolButton::menu-arrow:open {\n     top: 1px; left: 1px; /* shift it a bit */\n }").toString();
	mainMenuToolbar = reader.value("mainMenuToolbar", "QMenuBar::item:!selected,  QMenuBar::item:!pressed {\n\t background: transparent;\n}\n\n QMenuBar::item {\n     spacing: 3px; /* spacing between menu bar items */\n     padding: 4px 4px;\n     background: rgb(230, 230, 230);\n }\n\n QMenuBar::item:selected,  QMenuBar::item:pressed { /* when selected using mouse or keyboard */\n\tborder: 1px solid rgb(124, 126, 124);\n\tpadding-left: 2px;\n\tbackground-color: rgb(251, 251, 251);\n }\n\nQToolBar {\n\tborder: 0px solid grey;\n\tbackground-color: lightgrey;\n}\n\nQToolBar::handle {\n\tborder-image: url(:/Resource/toolbarGrip.png) repeat;\n\tborder-bottom: 1px;\n\tmax-width: 5px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 1px solid transparent;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n}\n\n QToolButton:pressed {\n     background-color: white;\n }\n\n QToolButton[popupMode=\"1\"] { /* only for MenuButtonPopup */\n     padding-right: 20px; /* make way for the popup button */\n }\n\n/* the subcontrols below are used only in the MenuButtonPopup mode */\n QToolButton::menu-button {\n     border: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\nQToolButton::menu-button:hover {\n    border: 1px solid rgb(124, 126, 124);\n\tborder-left-color: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\n QToolButton::menu-arrow:open {\n     top: 1px; left: 1px; /* shift it a bit */\n}").toString();

	// Headers
	libraryViewHeader = reader.value("libraryViewHeader", "QFrame {\n\tborder: 0px solid transparent;\n\tbackground-color: rgb(230, 230, 230);\n}\n\nQToolBar::handle {\n\tborder-image: url(:/Resource/toolbarGrip.png) repeat;\n\tborder-bottom: 1px;\n\tmax-width: 5px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 1px solid transparent;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n}\n\n QToolButton:pressed {\n     background-color: white;\n }\n\n QToolButton[popupMode=\"1\"] { /* only for MenuButtonPopup */\n     padding-right: 20px; /* make way for the popup button */\n }\n\n/* the subcontrols below are used only in the MenuButtonPopup mode */\n QToolButton::menu-button {\n     border: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\nQToolButton::menu-button:hover {\n    border: 1px solid rgb(124, 126, 124);\n\tborder-left-color: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\n QToolButton::menu-arrow:open {\n     top: 1px; left: 1px; /* shift it a bit */\n }").toString();
	genericHeader = reader.value("genericHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	homeHeader = reader.value("homeHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	libraryHeader = reader.value("libraryHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	mediaHeader = reader.value("mediaHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	searchHeader = reader.value("searchHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	transfersHeader = reader.value("transfersHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	securityHeader = reader.value("securityHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	activityHeader = reader.value("activityHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	chatHeader = reader.value("chatHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: black;\nbackground-color: lightgrey;").toString();
	dialogHeader = reader.value("dialogHeader", ".QFrame {\n\tfont-size: 15px;\n\tfont-weight: bold;\n\tcolor: black;\n\tbackground-color: lightgrey;\n\tmin-height: 50px;\n\tmax-height: 50px;\n\tpadding-left: 4px;\n}").toString();

	// Media
	seekSlider = reader.value("seekSlider", "QSlider::groove:horizontal {\n     border: 1px solid #999999;\n     height: 12px; /* the groove expands to the size of the slider by default. by giving it a height, it has a fixed size */\n     position: absolute;\n     background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);\n     margin: 6px 0;\n }\n\n QSlider::handle:horizontal {\n\tborder: 1px solid darkgrey;\n\tbackground-color: lightgrey;\n     width: 8px;\n     position: absolute;\n }").toString();
	volumeSlider = reader.value("volumeSlider", "QSlider::groove:horizontal {\n     border: 1px solid #999999;\n     height: 2px; /* the groove expands to the size of the slider by default. by giving it a height, it has a fixed size */\n     position: absolute;\n     background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);\n     margin: 6px 0;\n }\n\n QSlider::handle:horizontal {\n\tborder: 1px solid darkgrey;\n\tbackground-color: lightgrey;\n     width: 6px;\n     margin: -6 0px; /* handle is placed by default on the contents rect of the groove. Expand outside the groove */\n     position: absolute;\n }").toString();
	mediaToolbar = reader.value("mediaToolbar", "QToolBar {\n\tborder: 0px solid transparent;\n\tbackground-color: lightgrey;\n}\n\nQToolBar::handle {\n\tborder-image: url(:/Resource/toolbarGrip.png) repeat;\n\tborder-bottom: 1px;\n\tmax-width: 5px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 1px solid transparent;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n}\n\n QToolButton:pressed {\n     background-color: white;\n }\n\n QToolButton[popupMode=\"1\"] { /* only for MenuButtonPopup */\n     padding-right: 20px; /* make way for the popup button */\n }\n\n/* the subcontrols below are used only in the MenuButtonPopup mode */\n QToolButton::menu-button {\n     border: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\nQToolButton::menu-button:hover {\n    border: 1px solid rgb(124, 126, 124);\n\tborder-left-color: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\n QToolButton::menu-arrow:open {\n     top: 1px; left: 1px; /* shift it a bit */\n }\n\n").toString();

	// Chat
	chatWelcome = reader.value("chatWelcome", "QFrame {\n\t background-color: rgb(186, 186, 186);\n}").toString();
	chatToolbar = reader.value("chatToolbar", "QToolBar {\n\tborder: 0px solid transparent;\n\tbackground-color: lightgrey;\n}\n\nQToolBar::handle {\n\tborder-image: url(:/Resource/toolbarGrip.png) repeat;\n\tborder-bottom: 1px;\n\tmax-width: 5px;\n}\n\nQToolButton {\n\tbackground-color: transparent;\n\tborder: 1px solid transparent;\n}\n\nQToolButton:hover, QToolButton:checked {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n}\n\n QToolButton:pressed {\n     background-color: white;\n }\n\n QToolButton[popupMode=\"1\"] { /* only for MenuButtonPopup */\n     padding-right: 20px; /* make way for the popup button */\n }\n\n/* the subcontrols below are used only in the MenuButtonPopup mode */\n QToolButton::menu-button {\n     border: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\nQToolButton::menu-button:hover {\n    border: 1px solid rgb(124, 126, 124);\n\tborder-left-color: 1px solid transparent;\n     /* 16px width + 4px for border = 20px allocated above */\n     width: 16px;\n }\n\n QToolButton::menu-arrow:open {\n     top: 1px; left: 1px; /* shift it a bit */\n }\n\n").toString();

	// Specialised Tab Widgets
	libraryNavigator = reader.value("libraryNavigator", "QTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid transparent;\n }\n\n QTabWidget::tab-bar {\n     left: 5px;  /* move to the right by 5px */\n }\n\n /* Style the tab using the tab sub-control. Note that\n     it reads QTabBar _not_ QTabWidget */\n QTabBar::tab {\n     background:transparent;\n     border: 1px solid transparent;\n     padding: 4px;\n }\n\n QTabBar::tab:selected, QTabBar::tab:hover {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n }").toString();
	tabSearches = reader.value("tabSearches", "QTabWidget::pane { /* The tab widget frame */\n    border-top: 2px solid rgb(216, 216, 216);\n\tcolor: rgb(220, 220, 220);\n }\n\nQTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid transparent;\n }\n\n QTabWidget::tab-bar {\n     left: 5px;  /* move to the right by 5px */\n }\n\n /* Style the tab using the tab sub-control. Note that\n     it reads QTabBar _not_ QTabWidget */\n QTabBar::tab {\n     background:transparent;\n     border: 1px solid transparent;\n     padding: 4px;\n }\n\n QTabBar::tab:selected, QTabBar::tab:hover {\n\tborder-color: rgb(124, 126, 124);\n\tbackground-color: rgb(217, 217, 217);\n }\n\nQFrame#frameSearches {\n\tbackground-color: rgb(236, 236, 236);\n}").toString();

	// Colors
	logColorInformation = reader.value("logColorInformation", "").toString();
	logWeightInformation = reader.value("logWeightInformation", "").toString();
	logColorSecurity = reader.value("logColorSecurity", QColor(qRgb(170, 170, 0))).value<QColor>();
	logWeightSecurity = reader.value("logWeightSecurity", "font-weight:600;").toString();
	logColorNotice = reader.value("logColorNotice", QColor(qRgb(0, 170, 0))).value<QColor>();
	logWeightNotice = reader.value("logWeightNotice", "font-weight:600;").toString();
	logColorDebug = reader.value("logColorDebug", QColor(qRgb(117, 117, 117))).value<QColor>();
	logWeightDebug = reader.value("logWeightDebug", "").toString();
	logColorWarning = reader.value("logColorWarning", QColor(qRgb(255, 0, 0))).value<QColor>();
	logWeightWarning = reader.value("logWeightWarning", "").toString();
	logColorError = reader.value("logColorError", QColor(qRgb(170, 0, 0))).value<QColor>();
	logWeightError = reader.value("logWeightError", "font-weight:600;").toString();
	logColorCritical = reader.value("logColorCritical", QColor(qRgb(255, 0, 0))).value<QColor>();
	logWeightCritical = reader.value("logWeightCritical", "font-weight:600;").toString();
	listsColorNormal = reader.value("listsColorNormal", QColor(0, 0, 0)).value<QColor>();
	listsWeightNormal = reader.value("listsWeightNormal", qApp->font().weight()).toInt();
	listsColorActive = reader.value("listsColorActive", QColor(0, 0, 180)).value<QColor>();
	listsWeightActive = reader.value("listsWeightActive", qApp->font().weight()).toInt();
	listsColorSpecial = reader.value("listsColorSpecial", QColor(0, 0, 180)).value<QColor>();
	listsWeightSpecial = reader.value("listsWeightSpecial", qApp->font().weight()).toInt();
	listsColorHighlighted = reader.value("listsColorHighlighted", QColor(0, 0, 180)).value<QColor>();
	listsWeightHighlighted = reader.value("listsWeightHighlighted", qApp->font().weight()).toInt();
	chatMessagesColorNormal = reader.value("chatMessagesColorNormal", QColor(0, 0, 0)).value<QColor>();
	chatMessagesWeightNormal = reader.value("chatMessagesWeightNormal", "").toString();
	chatMessagesColorHighlighted = reader.value("chatMessagesColorHighlighted", QColor(0, 0, 180)).value<QColor>();
	chatMessagesWeightHighlighted = reader.value("chatMessagesWeightHighlighted", "").toString();
	chatMessagesColorServer = reader.value("chatMessagesColorServer", QColor(0, 0, 255)).value<QColor>();
	chatMessagesWeightServer = reader.value("chatMessagesWeightServer", "").toString();
	chatMessagesColorTopics = reader.value("chatMessagesColorTopics", QColor(125, 220, 80)).value<QColor>();
	chatMessagesWeightTopics = reader.value("chatMessagesWeightTopics", "").toString();
	chatMessagesColorNotices = reader.value("chatMessagesColorNotices", QColor(255, 0, 0)).value<QColor>();
	chatMessagesWeightNotices = reader.value("chatMessagesWeightNotices", "font-weight:600;").toString();
	chatMessagesColorActions = reader.value("chatMessagesColorActions", QColor(0, 144, 0)).value<QColor>();
	chatMessagesWeightActions = reader.value("chatMessagesWeightActions", "").toString();
	chatUsersColorNormal = reader.value("chatUsersColorNormal", QColor(0, 0, 0)).value<QColor>();
	chatUsersWeightNormal = reader.value("chatUsersWeightNormal", qApp->font().weight()).toInt();
	chatUsersColorOwner = reader.value("chatUsersColorOwner", QColor(151, 13, 120)).value<QColor>();
	chatUsersWeightOwner = reader.value("chatUsersWeightOwner", qApp->font().weight()).toInt();
	chatUsersColorAdministrator = reader.value("chatUsersColorAdministrator", QColor(128, 0, 0)).value<QColor>();
	chatUsersWeightAdministrator = reader.value("chatUsersWeightAdministrator", qApp->font().weight()).toInt();
	chatUsersColorOperator = reader.value("chatUsersColorOperator", QColor(0, 155, 0)).value<QColor>();
	chatUsersWeightOperator = reader.value("chatUsersWeightOperator", qApp->font().weight()).toInt();
	chatUsersColorHalfOperator = reader.value("chatUsersColorHalfOperator", QColor(78, 78, 255)).value<QColor>();
	chatUsersWeightHalfOperator = reader.value("chatUsersWeightHalfOperator", qApp->font().weight()).toInt();
	chatUsersColorVoiced = reader.value("chatUsersColorVoiced", QColor(208, 197, 16)).value<QColor>();
	chatUsersWeightVoiced = reader.value("chatUsersWeightVoiced", qApp->font().weight()).toInt();
}

void QSkinSettings::saveSkin(QString fileName)
{
	QSettings writer(fileName, QSettings::IniFormat);
	writer.sync();
	writer.setValue("skinName", skinName);
	writer.setValue("skinAuthor", skinAuthor);
	writer.setValue("skinVersion", skinVersion);
	writer.setValue("skinDescription", skinDescription);

	// Window Frame
	writer.setValue("windowFrameTopLeftStyleSheet", windowFrameTopLeftStyleSheet);
	writer.setValue("windowFrameLeftStyleSheet", windowFrameLeftStyleSheet);
	writer.setValue("windowFrameBottomLeftStyleSheet", windowFrameBottomLeftStyleSheet);
	writer.setValue("windowFrameTopStyleSheet", windowFrameTopStyleSheet);
	writer.setValue("windowFrameBottomStyleSheet", windowFrameBottomStyleSheet);
	writer.setValue("windowFrameTopRightStyleSheet", windowFrameTopRightStyleSheet);
	writer.setValue("windowFrameRightStyleSheet", windowFrameRightStyleSheet);
	writer.setValue("windowFrameBottomRightStyleSheet", windowFrameBottomRightStyleSheet);
	writer.setValue("titlebarButtonsFrameStyleSheet", titlebarButtonsFrameStyleSheet);
	writer.setValue("minimizeButtonStyleSheet", minimizeButtonStyleSheet);
	writer.setValue("maximizeButtonStyleSheet", maximizeButtonStyleSheet);
	writer.setValue("closeButtonStyleSheet", closeButtonStyleSheet);
	writer.setValue("windowFrameTopSpacerStyleSheet", windowFrameTopSpacerStyleSheet);
	writer.setValue("windowTextStyleSheet", windowTextStyleSheet);
	writer.setValue("windowIconFrameStyleSheet", windowIconFrameStyleSheet);
	writer.setValue("windowIconVisible", windowIconVisible);

	// Child Window Frame
	writer.setValue("childWindowFrameTopLeftStyleSheet", childWindowFrameTopLeftStyleSheet);
	writer.setValue("childWindowFrameLeftStyleSheet", childWindowFrameLeftStyleSheet);
	writer.setValue("childWindowFrameBottomLeftStyleSheet", childWindowFrameBottomLeftStyleSheet);
	writer.setValue("childWindowFrameTopStyleSheet", childWindowFrameTopStyleSheet);
	writer.setValue("childWindowFrameBottomStyleSheet", childWindowFrameBottomStyleSheet);
	writer.setValue("childWindowFrameTopRightStyleSheet", childWindowFrameTopRightStyleSheet);
	writer.setValue("childWindowFrameRightStyleSheet", childWindowFrameRightStyleSheet);
	writer.setValue("childWindowFrameBottomRightStyleSheet", childWindowFrameBottomRightStyleSheet);
	writer.setValue("childTitlebarButtonsFrameStyleSheet", childTitlebarButtonsFrameStyleSheet);
	writer.setValue("childMinimizeButtonStyleSheet", childMinimizeButtonStyleSheet);
	writer.setValue("childMaximizeButtonStyleSheet", childMaximizeButtonStyleSheet);
	writer.setValue("childCloseButtonStyleSheet", childCloseButtonStyleSheet);
	writer.setValue("childWindowFrameTopSpacerStyleSheet", childWindowFrameTopSpacerStyleSheet);
	writer.setValue("childWindowTextStyleSheet", childWindowTextStyleSheet);
	writer.setValue("childWindowIconFrameStyleSheet", childWindowIconFrameStyleSheet);
	writer.setValue("childWindowIconVisible", childWindowIconVisible);

	// Splash Screen
	writer.setValue("splashBackground", splashBackground);
	writer.setValue("splashLogo", splashLogo);
	writer.setValue("splashFooter", splashFooter);
	writer.setValue("splashProgress", splashProgress);
	writer.setValue("splashStatus", splashStatus);

	// Standard Items
	writer.setValue("standardItems", standardItems);

	// List Views
	writer.setValue("listViews", listViews);

	// Sidebar
	writer.setValue("sidebarBackground", sidebarBackground);
	writer.setValue("sidebarTaskBackground", sidebarTaskBackground);
	writer.setValue("sidebarTaskHeader", sidebarTaskHeader);
	writer.setValue("sidebarUnclickableTaskHeader", sidebarUnclickableTaskHeader);
	writer.setValue("addSearchButton", addSearchButton);

	// Toolbars
	writer.setValue("toolbars", toolbars);
	writer.setValue("navigationToolbar", navigationToolbar);
	writer.setValue("mainMenuToolbar", mainMenuToolbar);

	// Headers
	writer.setValue("libraryViewHeader", libraryViewHeader);
	writer.setValue("genericHeader", genericHeader);
	writer.setValue("homeHeader", homeHeader);
	writer.setValue("libraryHeader", libraryHeader);
	writer.setValue("mediaHeader", mediaHeader);
	writer.setValue("searchHeader", searchHeader);
	writer.setValue("transfersHeader", transfersHeader);
	writer.setValue("securityHeader", securityHeader);
	writer.setValue("activityHeader", activityHeader);
	writer.setValue("chatHeader", chatHeader);
	writer.setValue("dialogHeader", dialogHeader);

	// Media
	writer.setValue("seekSlider", seekSlider);
	writer.setValue("volumeSlider", volumeSlider);
	writer.setValue("mediaToolbar", mediaToolbar);

	// Chat
	writer.setValue("chatWelcome", chatWelcome);
	writer.setValue("chatToolbar", chatToolbar);

	// Specialised Tab Widgets
	writer.setValue("libraryNavigator", libraryNavigator);
	writer.setValue("tabSearches", tabSearches);

	// Colors
	writer.setValue("logColorInformation", logColorInformation);
	writer.setValue("logWeightInformation", logWeightInformation);
	writer.setValue("logColorSecurity", logColorSecurity);
	writer.setValue("logWeightSecurity", logWeightSecurity);
	writer.setValue("logColorNotice", logColorNotice);
	writer.setValue("logWeightNotice", logWeightNotice);
	writer.setValue("logColorDebug", logColorDebug);
	writer.setValue("logWeightDebug", logWeightDebug);
	writer.setValue("logColorWarning", logColorWarning);
	writer.setValue("logWeightWarning", logWeightWarning);
	writer.setValue("logColorError", logColorError);
	writer.setValue("logWeightError", logWeightError);
	writer.setValue("logColorCritical", logColorCritical);
	writer.setValue("logWeightCritical", logWeightCritical);
	writer.setValue("listsColorNormal", listsColorNormal);
	writer.setValue("listsWeightNormal", listsWeightNormal);
	writer.setValue("listsColorActive", listsColorActive);
	writer.setValue("listsWeightActive", listsWeightActive);
	writer.setValue("listsColorSpecial", listsColorSpecial);
	writer.setValue("listsWeightSpecial", listsWeightSpecial);
	writer.setValue("listsColorHighlighted", listsColorHighlighted);
	writer.setValue("listsWeightHighlighted", listsWeightHighlighted);
	writer.setValue("chatMessagesColorNormal", chatMessagesColorNormal);
	writer.setValue("chatMessagesWeightNormal", chatMessagesWeightNormal);
	writer.setValue("chatMessagesColorHighlighted", chatMessagesColorHighlighted);
	writer.setValue("chatMessagesWeightHighlighted", chatMessagesWeightHighlighted);
	writer.setValue("chatMessagesColorServer", chatMessagesColorServer);
	writer.setValue("chatMessagesWeightServer", chatMessagesWeightServer);
	writer.setValue("chatMessagesColorTopics", chatMessagesColorTopics);
	writer.setValue("chatMessagesWeightTopics", chatMessagesWeightTopics);
	writer.setValue("chatMessagesColorNotices", chatMessagesColorNotices);
	writer.setValue("chatMessagesWeightNotices", chatMessagesWeightNotices);
	writer.setValue("chatMessagesColorActions", chatMessagesColorActions);
	writer.setValue("chatMessagesWeightActions", chatMessagesWeightActions);
	writer.setValue("chatUsersColorNormal", chatUsersColorNormal);
	writer.setValue("chatUsersWeightNormal", chatUsersWeightNormal);
	writer.setValue("chatUsersColorOwner", chatUsersColorOwner);
	writer.setValue("chatUsersWeightOwner", chatUsersWeightOwner);
	writer.setValue("chatUsersColorAdministrator", chatUsersColorAdministrator);
	writer.setValue("chatUsersWeightAdministrator", chatUsersWeightAdministrator);
	writer.setValue("chatUsersColorOperator", chatUsersColorOperator);
	writer.setValue("chatUsersWeightOperator", chatUsersWeightOperator);
	writer.setValue("chatUsersColorHalfOperator", chatUsersColorHalfOperator);
	writer.setValue("chatUsersWeightHalfOperator", chatUsersWeightHalfOperator);
	writer.setValue("chatUsersColorVoiced", chatUsersColorVoiced);
	writer.setValue("chatUsersWeightVoiced", chatUsersWeightVoiced);
}
