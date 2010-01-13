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

class QSkinSettings : public QObject
{
Q_OBJECT
public:
	explicit QSkinSettings(QObject *parent = 0);
	QString skinName;
	QString skinAuthor;
	QString skinVersion;
	QString skinDescription;
	QString windowFrameTopLeftStyleSheet;
	QString windowFrameLeftStyleSheet;
	QString windowFrameBottomLeftStyleSheet;
	QString windowFrameTopStyleSheet;
	QString windowFrameBottomStyleSheet;
	QString windowFrameTopRightStyleSheet;
	QString windowFrameRightStyleSheet;
	QString windowFrameBottomRightStyleSheet;
	QString titlebarFrameStyleSheet;
	QString titlebarButtonsFrameStyleSheet;
	QString minimizeButtonStyleSheet;
	QString maximizeButtonStyleSheet;
	QString closeButtonStyleSheet;
	QString windowTextStyleSheet;
	bool windowIconVisible;
	QSize windowIconSize;

signals:
	void skinChanged();

public slots:
	void loadSkin(QString fileName = "");
	void saveSkin(QString fileName = "");
};

extern QSkinSettings skinSettings;

#endif // QSKINSETTINGS_H
