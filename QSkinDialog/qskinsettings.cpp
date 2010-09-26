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

	// Main Window Frame
	windowFrameTopLeftStyleSheet = reader.value("windowFrameTopLeftStyleSheet", "border-image: url(:/Resource/frameTopLeft.png);").toString();
	windowFrameLeftStyleSheet = reader.value("windowFrameLeftStyleSheet", "border-image: url(:/Resource/frameLeft.png); border-left: 1; border-top: 10;").toString();
	windowFrameBottomLeftStyleSheet = reader.value("windowFrameBottomLeftStyleSheet", "border-image: url(:/Resource/frameBottomLeft.png);").toString();
	windowFrameTopStyleSheet = reader.value("windowFrameTopStyleSheet", "").toString();
	windowFrameBottomStyleSheet = reader.value("windowFrameBottomStyleSheet", "border-image: url(:/Resource/frameBottom.png); border-bottom: 1;").toString();
	windowFrameTopRightStyleSheet = reader.value("windowFrameTopRightStyleSheet", "border-image: url(:/Resource/frameTopRight.png);").toString();
	windowFrameRightStyleSheet = reader.value("windowFrameRightStyleSheet", "QFrame { border-image: url(:/Resource/frameRight.png); border-right: 1; border-top: 10; }").toString();
	windowFrameBottomRightStyleSheet = reader.value("windowFrameBottomRightStyleSheet", "border-image: url(:/Resource/frameBottomRight.png);").toString();
	titlebarButtonsFrameStyleSheet = reader.value("titlebarButtonsFrameStyleSheet", "/*These move the buttons up so they aren't displayed in the center of the titlebar\n   Remove these to center your buttons on the titlebar.*/\nQFrame#titlebarButtonsFrame {\n	padding-top: -1;\n	padding-bottom: 10;\n	border-image: url(:/Resource/titlebarButtonsFrame.png);\n}").toString();
	minimizeButtonStyleSheet = reader.value("minimizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/minButton.png); } QToolButton:hover { border-image: url(:/Resource/minButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/minButtonD.png); }").toString();
	maximizeButtonStyleSheet = reader.value("maximizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/maxButton.png); } QToolButton:hover { border-image: url(:/Resource/maxButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/maxButtonD.png); } QToolButton:checked { border-image: url(:/Resource/restoreButton.png); } QToolButton:checked:hover { border-image: url(:/Resource/restoreButtonH.png); } QToolButton:checked:disabled { border-image: url(:/Resource/restoreButtonD.png); }").toString();
	closeButtonStyleSheet = reader.value("closeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/quitButton.png); } QToolButton:hover { border-image: url(:/Resource/quitButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/quitButtonD.png); }").toString();
	windowFrameTopSpacerStyleSheet = reader.value("windowFrameTopSpacerStyleSheet", "QFrame#windowFrameTopSpacer {\n	border-image: url(:/Resource/frameTop.png);\n}").toString();
	windowTextStyleSheet = reader.value("windowTextStyleSheet", "border-image: url(:/Resource/windowTextBackground.png);\npadding-left: -2px;\npadding-right: -2px;\npadding-bottom: 2px;\nfont-weight: bold;\nfont-size: 16px;\ncolor: rgb(255, 255, 255);").toString();
	windowIconFrameStyleSheet = reader.value("windowIconFrameStyleSheet", "QFrame#windowIconFrame {\n	border-image: url(:/Resource/windowIconFrame.png);\n}").toString();
	windowIconVisible = reader.value("windowIconVisible", true).toBool();
	windowIconSize = reader.value("windowIconSize", QSize(20, 20)).toSize();

	// Child Window Frame
	childWindowFrameTopLeftStyleSheet = reader.value("childWindowFrameTopLeftStyleSheet", "border-image: url(:/Resource/frameTopLeft.png);").toString();
	childWindowFrameLeftStyleSheet = reader.value("childWindowFrameLeftStyleSheet", "border-image: url(:/Resource/frameLeft.png); border-left: 1; border-top: 10;").toString();
	childWindowFrameBottomLeftStyleSheet = reader.value("childWindowFrameBottomLeftStyleSheet", "border-image: url(:/Resource/frameBottomLeft.png);").toString();
	childWindowFrameTopStyleSheet = reader.value("childWindowFrameTopStyleSheet", "").toString();
	childWindowFrameBottomStyleSheet = reader.value("childWindowFrameBottomStyleSheet", "border-image: url(:/Resource/frameBottom.png); border-bottom: 1;").toString();
	childWindowFrameTopRightStyleSheet = reader.value("childWindowFrameTopRightStyleSheet", "border-image: url(:/Resource/frameTopRight.png);").toString();
	childWindowFrameRightStyleSheet = reader.value("childWindowFrameRightStyleSheet", "QFrame { border-image: url(:/Resource/frameRight.png); border-right: 1; border-top: 10; }").toString();
	childWindowFrameBottomRightStyleSheet = reader.value("childWindowFrameBottomRightStyleSheet", "border-image: url(:/Resource/frameBottomRight.png);").toString();
	childTitlebarButtonsFrameStyleSheet = reader.value("childTitlebarButtonsFrameStyleSheet", "/*These move the buttons up so they aren't displayed in the center of the titlebar\n   Remove these to center your buttons on the titlebar.*/\nQFrame#titlebarButtonsFrame {\n	padding-top: -1;\n	padding-bottom: 10;\n	border-image: url(:/Resource/titlebarButtonsFrame.png);\n}").toString();
	childMinimizeButtonStyleSheet = reader.value("childMinimizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/minButton.png); } QToolButton:hover { border-image: url(:/Resource/minButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/minButtonD.png); }").toString();
	childMaximizeButtonStyleSheet = reader.value("childMaximizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/maxButton.png); } QToolButton:hover { border-image: url(:/Resource/maxButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/maxButtonD.png); } QToolButton:checked { border-image: url(:/Resource/restoreButton.png); } QToolButton:checked:hover { border-image: url(:/Resource/restoreButtonH.png); } QToolButton:checked:disabled { border-image: url(:/Resource/restoreButtonD.png); }").toString();
	childCloseButtonStyleSheet = reader.value("childCloseButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/quitButton.png); } QToolButton:hover { border-image: url(:/Resource/quitButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/quitButtonD.png); }").toString();
	childWindowFrameTopSpacerStyleSheet = reader.value("childWindowFrameTopSpacerStyleSheet", "QFrame#windowFrameTopSpacer {\n	border-image: url(:/Resource/frameTop.png);\n}").toString();
	childWindowTextStyleSheet = reader.value("childWindowTextStyleSheet", "border-image: url(:/Resource/windowTextBackground.png);\npadding-left: -2px;\npadding-right: -2px;\npadding-bottom: 2px;\nfont-weight: bold;\nfont-size: 16px;\ncolor: rgb(255, 255, 255);").toString();
	childWindowIconFrameStyleSheet = reader.value("childWindowIconFrameStyleSheet", "QFrame#windowIconFrame {\n	border-image: url(:/Resource/windowIconFrame.png);\n}").toString();
	childWindowIconVisible = reader.value("childWindowIconVisible", true).toBool();
	childWindowIconSize = reader.value("childWindowIconSize", QSize(20, 20)).toSize();

	// Splash Screen
	splashBackground = reader.value("splashBackground", "QFrame {\n	border-image: url(:/Resource/Splash.png) repeat;\n}").toString();
	splashLogo = reader.value("splashLogo", "background-color: transparent;\nmin-height: 172px;\nmax-height: 172px;\nmin-width: 605px;\nmax-width: 605px;\nborder-image: url(:/Resource/QuazaaLogo.png);").toString();
	splashFooter = reader.value("splashFooter", "QFrame {\n	border-image: url(:/Resource/HeaderBackground.png) repeat;\n	min-height: 28;\n	max-height: 28;\n	min-width: 605px;\n	max-width: 605px;\n}").toString();
	splashProgress = reader.value("splashProgress", "font-weight: bold;\nmax-height: 10px;\nmin-height: 10px;\nmax-width: 300px;\nmin-width: 300px;").toString();
	splashStatus = reader.value("splashStatus", "font-weight: bold;\ncolor: white;\nbackground-color: transparent;\npadding-left: 5px;").toString();

	// Standard Items
	standardItems = reader.value("standardItems", "").toString();

	// ListViews
	listViews = reader.value("listViews", "").toString();

	// Sidebar
	sidebarBackground = reader.value("sidebarBackground", "QFrame {\n	 background-color: rgb(199, 202, 255);\n}").toString();
	sidebarTaskBackground = reader.value("sidebarTaskBackground", "QFrame {\n	background-color: rgb(161, 178, 231);\n}").toString();
	sidebarTaskHeader = reader.value("sidebarTaskHeader", "QToolButton {\n	background-color: rgb(78, 124, 179);\n	color: rgb(255, 255, 255);\n	border: none;\n	font-size: 16px;\n	font-weight: bold;\n}\n\nQToolButton:hover {\n	background-color: rgb(56, 90, 129);\n}").toString();
	sidebarUnclickableTaskHeader = reader.value("sidebarUnclickableTaskHeader", "QToolButton {\n	background-color: rgb(78, 124, 179);\n	color: rgb(255, 255, 255);\n	border: none;\n	font-size: 16px;\n	font-weight: bold;\n}").toString();
	addSearchButton = reader.value("addSearchButton", "").toString();

	// Toolbars
	toolbars = reader.value("toolbars", "").toString();
	navigationToolbar = reader.value("navigationToolbar", "").toString();
	mainMenuToolbar = reader.value("mainMenuToolbar", "QMenuBar::item:!selected,  QMenuBar::item:!pressed { background: transparent; }").toString();

	// Headers
	libraryViewHeader = reader.value("libraryViewHeader", "").toString();
	genericHeader = reader.value("genericHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	homeHeader = reader.value("homeHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	libraryHeader = reader.value("libraryHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	mediaHeader = reader.value("mediaHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	searchHeader = reader.value("searchHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	transfersHeader = reader.value("transfersHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	securityHeader = reader.value("securityHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	activityHeader = reader.value("activityHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	chatHeader = reader.value("chatHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();
	dialogHeader = reader.value("dialogHeader", "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);").toString();

	// Media
	seekSlider = reader.value("seekSlider", "QSlider::groove:horizontal {	\n	border: 1px solid rgb(82, 111, 174); \n	height: 22px; \n	background: black; \n	margin: 3px 0; \n}	\n\nQSlider::handle:horizontal { \n	background: qlineargradient(spread:pad, x1:0.510526, y1:0, x2:0.511, y2:1, stop:0 rgba(206, 215, 255, 255), stop:0.184211 rgba(82, 107, 192, 255), stop:0.342105 rgba(55, 80, 167, 255), stop:0.484211 rgba(17, 26, 148, 255), stop:0.636842 rgba(0, 0, 0, 255), stop:0.8 rgba(24, 46, 171, 255), stop:0.984211 rgba(142, 142, 255, 255)); \n	border: 1px solid rgb(82, 111, 174); \n	border-radius: 0px; \n	width: 4px; \n	margin: 1px 0; /* handle is placed by default on the contents rect of the groove. Expand outside the groove */ \n}").toString();
	volumeSlider = reader.value("volumeSlider", "QSlider::groove:horizontal { \n	border: 1px solid rgb(0, 61, 89); \n	height: 3px;  \n	background: black; \n	margin: 2px 0; \n} \n\nQSlider::handle:horizontal { \n	background: qlineargradient(spread:pad, x1:0.510526, y1:0, x2:0.511, y2:1, stop:0 rgba(206, 215, 255, 255), stop:0.184211 rgba(82, 107, 192, 255), stop:0.342105 rgba(55, 80, 167, 255), stop:0.484211 rgba(17, 26, 148, 255), stop:0.636842 rgba(0, 0, 0, 255), stop:0.8 rgba(24, 46, 171, 255), stop:0.984211 rgba(142, 142, 255, 255)); \n	border: 1px solid rgb(82, 111, 174); \n	width: 6px; \n	margin: -4px 0; \n} ").toString();
	mediaToolbar = reader.value("mediaToolbar", "").toString();

	// Chat
	chatWelcome = reader.value("chatWelcome", "QFrame {\n	background-color: rgb(78, 124, 179);\n	color: rgb(255, 255, 255);\n}").toString();
	chatToolbar = reader.value("chatToolbar", "QFrame {\n	background-color: rgb(199, 202, 255);\n}").toString();

	// Specialised Tab Widgets
	libraryNavigator = reader.value("libraryNavigator", "QTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid transparent;\n }\n\n QTabWidget::tab-bar {\n     left: 5px;  /* move to the right by 5px */\n }\n\n /* Style the tab using the tab sub-control. Note that\n     it reads QTabBar _not_ QTabWidget */\n QTabBar::tab {\n     background:transparent;\n     border: 1px solid transparent;\n     padding: 4px;\n }\n\n QTabBar::tab:selected, QTabBar::tab:hover {\n     border: 1px solid rgb(78, 96, 255);\n     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);\n }").toString();
	tabSearches = reader.value("tabSearches", "QTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid #C2C7CB;\n }\n\nQTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid transparent;\n }\n\n QTabWidget::tab-bar {\n     left: 5px;  /* move to the right by 5px */\n }\n\n /* Style the tab using the tab sub-control. Note that\n     it reads QTabBar _not_ QTabWidget */\n QTabBar::tab {\n     background:transparent;\n     border: 1px solid transparent;\n     padding: 4px;\n }\n\n QTabBar::tab:selected, QTabBar::tab:hover {\n     border: 1px solid rgb(78, 96, 255);\n     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);\n }").toString();

	// Colors
	logColorInformation = reader.value("logColorInformation", "").toString();
	logWeightInformation = reader.value("logWeightInformation", "").toString();
	logColorSecurity = reader.value("logColorSecurity", QColor(qRgb(170,170,0))).value<QColor>();
	logWeightSecurity = reader.value("logWeightSecurity", "font-weight:600;").toString();
	logColorNotice = reader.value("logColorNotice", QColor(qRgb(0,170,0))).value<QColor>();
	logWeightNotice = reader.value("logWeightNotice", "font-weight:600;").toString();
	logColorDebug = reader.value("logColorDebug", QColor(qRgb(117,117,117))).value<QColor>();
	logWeightDebug = reader.value("logWeightDebug", "").toString();
	logColorWarning = reader.value("logColorWarning", QColor(qRgb(255,0,0))).value<QColor>();
	logWeightWarning = reader.value("logWeightWarning", "").toString();
	logColorError = reader.value("logColorError", QColor(qRgb(170,0,0))).value<QColor>();
	logWeightError = reader.value("logWeightError", "font-weight:600;").toString();
	logColorCritical = reader.value("logColorCritical", QColor(qRgb(255,0,0))).value<QColor>();
	logWeightCritical = reader.value("logWeightCritical", "font-weight:600;").toString();
	listsColorNormal = reader.value("listsColorNormal", QColor(0,0,0)).value<QColor>();
	listsWeightNormal = reader.value("listsWeightNormal", qApp->font().weight()).toInt();
	listsColorActive = reader.value("listsColorActive", QColor(0, 0, 180)).value<QColor>();
	listsWeightActive = reader.value("listsWeightActive", qApp->font().weight()).toInt();
	listsColorSpecial = reader.value("listsColorSpecial", QColor(0, 0, 180)).value<QColor>();
	listsWeightSpecial = reader.value("listsWeightSpecial", qApp->font().weight()).toInt();
	listsColorHighlighted = reader.value("listsColorHighlighted", QColor(0, 0, 180)).value<QColor>();
	listsWeightHighlighted = reader.value("listsWeightHighlighted", qApp->font().weight()).toInt();
	chatMessagesColorNormal = reader.value("chatMessagesColorNormal", QColor(0,0,0)).value<QColor>();
	chatMessagesWeightNormal = reader.value("chatMessagesWeightNormal", "").toString();
	chatMessagesColorHighlighted = reader.value("chatMessagesColorHighlighted", QColor(0, 0, 180)).value<QColor>();
	chatMessagesWeightHighlighted = reader.value("chatMessagesWeightHighlighted", "").toString();
	chatMessagesColorServer = reader.value("chatMessagesColorServer", QColor(0,0,255)).value<QColor>();
	chatMessagesWeightServer = reader.value("chatMessagesWeightServer", "").toString();
	chatMessagesColorTopics = reader.value("chatMessagesColorTopics", QColor(125,220,80)).value<QColor>();
	chatMessagesWeightTopics = reader.value("chatMessagesWeightTopics", "").toString();
	chatMessagesColorNotices = reader.value("chatMessagesColorNotices", QColor(255,0,0)).value<QColor>();
	chatMessagesWeightNotices = reader.value("chatMessagesWeightNotices", "font-weight:600;").toString();
	chatMessagesColorActions = reader.value("chatMessagesColorActions", QColor(0,144,0)).value<QColor>();
	chatMessagesWeightActions = reader.value("chatMessagesWeightActions", "").toString();
	chatUsersColorNormal = reader.value("chatUsersColorNormal", QColor(0,0,0)).value<QColor>();
	chatUsersWeightNormal = reader.value("chatUsersWeightNormal", qApp->font().weight()).toInt();
	chatUsersColorOwner = reader.value("chatUsersColorOwner", QColor(151,13,120)).value<QColor>();
	chatUsersWeightOwner = reader.value("chatUsersWeightOwner", qApp->font().weight()).toInt();
	chatUsersColorAdministrator = reader.value("chatUsersColorAdministrator", QColor(128,0,0)).value<QColor>();
	chatUsersWeightAdministrator = reader.value("chatUsersWeightAdministrator", qApp->font().weight()).toInt();
	chatUsersColorOperator = reader.value("chatUsersColorOperator", QColor(0,155,0)).value<QColor>();
	chatUsersWeightOperator = reader.value("chatUsersWeightOperator", qApp->font().weight()).toInt();
	chatUsersColorHalfOperator = reader.value("chatUsersColorHalfOperator", QColor(78,78,255)).value<QColor>();
	chatUsersWeightHalfOperator = reader.value("chatUsersWeightHalfOperator", qApp->font().weight()).toInt();
	chatUsersColorVoiced = reader.value("chatUsersColorVoiced", QColor(208,197,16)).value<QColor>();
	chatUsersWeightVoiced = reader.value("chatUsersWeightVoiced", qApp->font().weight()).toInt();
}

void QSkinSettings::saveSkin(QString fileName)
{
	QSettings writer(fileName, QSettings::IniFormat);
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
	writer.setValue("windowIconSize", windowIconSize);

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
	writer.setValue("childWindowIconSize", childWindowIconSize);

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
