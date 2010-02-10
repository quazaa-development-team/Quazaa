//
// main.cpp
//
// Copyright © Quazaa Development Team, 2009.
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
#include "mainwindow.h"
#include "quazaaglobals.h"
#include "quazaasettings.h"
#include "dialoglanguage.h"
#include "qtsingleapplication/src/qtsingleapplication.h"

QuazaaGlobals quazaaGlobals;

int main(int argc, char *argv[])
{
	QtSingleApplication theApp(argc, argv);
	// Check if the application is already running
	if (theApp.sendMessage("Show App"))
		return 0;

	QApplication::setApplicationName(quazaaGlobals.ApplicationName());
	QApplication::setApplicationVersion(quazaaGlobals.ApplicationVersionString());
	QApplication::setOrganizationDomain(quazaaGlobals.ApplicationOrganizationDomain());
	QApplication::setOrganizationName(quazaaGlobals.ApplicationOrganizationName());

	//Initialize multilanguage support
	quazaaSettings.loadLanguageSettings();
	if (quazaaSettings.FirstRun())
	{
		DialogLanguage *dlgLanguage = new DialogLanguage();
		dlgLanguage->exec();
	}
	if (!quazaaSettings.LanguageDefault)
	{
		QFile languageFile(quazaaSettings.LanguageFile);
		QFileInfo languageFileInfo(languageFile);
		QTranslator translator;

		if (languageFile.open(QIODevice::ReadOnly))
		{
			translator.load(languageFileInfo.fileName(), languageFileInfo.filePath());
			theApp.installTranslator(&translator);
		}
	}

	theApp.processEvents();
	QSkinDialog skinWinMain(true,true,true);
	MainWindow *winMain = new MainWindow();
	skinWinMain.addChildWidget(winMain);

	// Make the main window show if the user tried to open another instance
	QObject::connect(&theApp, SIGNAL(messageReceived(const QString&)),
						  &skinWinMain, SLOT(restore()));
	theApp.setActivationWindow(&skinWinMain);
	QObject::connect(&skinWinMain, SIGNAL(needToShow()), &theApp, SLOT(activateWindow()));

	skinWinMain.connect(winMain, SIGNAL(closed()), &skinWinMain, SLOT(close()));
	skinWinMain.connect(winMain, SIGNAL(hideMain()), &skinWinMain, SLOT(hide()));
	skinWinMain.connect(winMain, SIGNAL(showMain()), &skinWinMain, SLOT(show()));
	skinWinMain.connect(&skinWinMain, SIGNAL(mainClose()), winMain, SLOT(close()));
	// Load mainwindow visible state from the registry

	quazaaSettings.loadSkinWindowSettings(&skinWinMain);
	if ( quazaaSettings.MainWindowVisible )
	{
		skinWinMain.show();
	}
	return theApp.exec();
}
