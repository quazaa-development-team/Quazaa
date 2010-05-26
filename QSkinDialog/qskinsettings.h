/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: mainwindow.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : This is the header for where the style sheets for he current skin will actually be
 *				  held, loaded and saved. It also passes the skinChanged() signal to the dialogs.
 ********************************************************************************************************/
#ifndef QSKINSETTINGS_H
#define QSKINSETTINGS_H

#include <QObject>
#include <QSize>
#include <QColor>

class QSkinSettings : public QObject
{
Q_OBJECT
public:
	explicit QSkinSettings(QObject *parent = 0);
	QString skinName;
	QString skinAuthor;
	QString skinVersion;
	QString skinDescription;

	// Main Window Frame
	QString windowFrameTopLeftStyleSheet;
	QString windowFrameLeftStyleSheet;
	QString windowFrameBottomLeftStyleSheet;
	QString windowFrameTopStyleSheet;
	QString windowFrameBottomStyleSheet;
	QString windowFrameTopRightStyleSheet;
	QString windowFrameRightStyleSheet;
	QString windowFrameBottomRightStyleSheet;
	QString titlebarButtonsFrameStyleSheet;
	QString minimizeButtonStyleSheet;
	QString maximizeButtonStyleSheet;
	QString closeButtonStyleSheet;
	QString windowTextStyleSheet;
	QString windowIconFrameStyleSheet;
	QString windowFrameTopSpacerStyleSheet;
	bool windowIconVisible;
	QSize windowIconSize;

	// Child Window Frame
	QString childWindowFrameTopLeftStyleSheet;
	QString childWindowFrameLeftStyleSheet;
	QString childWindowFrameBottomLeftStyleSheet;
	QString childWindowFrameTopStyleSheet;
	QString childWindowFrameBottomStyleSheet;
	QString childWindowFrameTopRightStyleSheet;
	QString childWindowFrameRightStyleSheet;
	QString childWindowFrameBottomRightStyleSheet;
	QString childTitlebarButtonsFrameStyleSheet;
	QString childMinimizeButtonStyleSheet;
	QString childMaximizeButtonStyleSheet;
	QString childCloseButtonStyleSheet;
	QString childWindowTextStyleSheet;
	QString childWindowIconFrameStyleSheet;
	QString childWindowFrameTopSpacerStyleSheet;
	bool childWindowIconVisible;
	QSize childWindowIconSize;

	// Splash Screen
	QString splashBackground;
	QString splashLogo;
	QString splashFooter;
	QString splashProgress;
	QString splashStatus;

	// Standard Items
	QString standardItems;

	// Sidebar
	QString sidebarBackground;
	QString sidebarTaskBackground;
	QString sidebarTaskHeader;
	QString sidebarUnclickableTaskHeader;
	QString addSearchButton;

	// Toolbars
	QString toolbars;
	QString navigationToolbar;
	QString mainMenuToolbar;

	// Headers
	QString libraryViewHeader;
	QString genericHeader;
	QString homeHeader;
	QString libraryHeader;
	QString mediaHeader;
	QString searchHeader;
	QString transfersHeader;
	QString securityHeader;
	QString activityHeader;
	QString chatHeader;
	QString dialogHeader;

	// Media
	QString seekSlider;
	QString volumeSlider;
	QString mediaToolbar;

	// Chat
	QString chatWelcome;
	QString chatToolbar;

	// Specialised Tab Widgets
	QString libraryNavigator;
	QString tabSearches;

	// Log colors
	QColor logColorInformation;
	QString logWeightInformation;
	QColor logColorSecurity;
	QString logWeightSecurity;
	QColor logColorNotice;
	QString logWeightNotice;
	QColor logColorDebug;
	QString logWeightDebug;
	QColor	logColorWarning;
	QString logWeightWarning;
	QColor logColorError;
	QString logWeightError;
	QColor logColorCritical;
	QString logWeightCritical;

signals:
	void skinChanged();

public slots:
	void loadSkin(QString fileName = "");
	void saveSkin(QString fileName = "");
};

extern QSkinSettings skinSettings;

#endif // QSKINSETTINGS_H
