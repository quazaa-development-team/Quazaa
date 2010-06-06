//
// main.cpp
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

#include <QtGui/QApplication>
#include <QSettings>
#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>
#include <QMessageBox>
#include <QtCore>
#include "winmain.h"
#include "quazaaglobals.h"
#include "quazaasettings.h"
#include "dialoglanguage.h"
#include "qtsingleapplication/src/qtsingleapplication.h"

#define IRC_NO_DEPRECATED

QuazaaGlobals quazaaGlobals;

int main(int argc, char *argv[])
{
	QtSingleApplication theApp(argc, argv);
	// Check if the application is already running
	if (theApp.sendMessage("Show App"))
		return 0;

	theApp.setApplicationName(quazaaGlobals.ApplicationName());
	theApp.setApplicationVersion(quazaaGlobals.ApplicationVersionString());
	theApp.setOrganizationDomain(quazaaGlobals.ApplicationOrganizationDomain());
	theApp.setOrganizationName(quazaaGlobals.ApplicationOrganizationName());

	QSkinDialog skinWinMain(true,true,true);
	WinMain *winMain = new WinMain();
	skinWinMain.addChildWidget(winMain);

	// Make the main window show if the user tried to open another instance
	// Windows programming guidelines state the window cannot activate without
	// direct user interaction (Qt follows Microsoft's rediculous rules that even
	// Microsoft doesn't follow.) due to popup issues etc. (We never had this
	// problem with Visual C++ because it lets you cheat and is the source for
	// popup malware.) so it will make the icon in the task bar blink instead.
	QObject::connect(&theApp, SIGNAL(messageReceived(const QString&)),
						  &skinWinMain, SLOT(restore()));
	theApp.setActivationWindow(&skinWinMain);
	QObject::connect(&skinWinMain, SIGNAL(needToShow()), &theApp, SLOT(activateWindow()));

	skinWinMain.connect(winMain, SIGNAL(closed()), &skinWinMain, SLOT(close()));
	skinWinMain.connect(winMain, SIGNAL(hideMain()), &skinWinMain, SLOT(hide()));
	skinWinMain.connect(winMain, SIGNAL(showMain()), &skinWinMain, SLOT(show()));
	skinWinMain.connect(&skinWinMain, SIGNAL(mainClose()), winMain, SLOT(close()));
	// Load WinMain visible state from the registry

	quazaaSettings.loadSkinWindowSettings(&skinWinMain);
		if ( quazaaSettings.WinMain.Visible )
	{
		skinWinMain.show();
	}
	return theApp.exec();
}
