/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: main.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : An example of how to use the skin dialog from within main
 ********************************************************************************************************/

#include <QtGui/QApplication>
#include <QObject>
#include "qskindialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QSkinDialog w;
	MainWindow *winMain = new MainWindow();
	w.addChildWidget(winMain);
	w.connect(winMain, SIGNAL(closed()), &w, SLOT(close()));
	w.show();
	return a.exec();
}
