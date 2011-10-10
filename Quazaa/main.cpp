/*
** main.cpp
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <QtGui/QApplication>

#include "winmain.h"
#include "quazaaglobals.h"
#include "quazaasettings.h"
#include "dialoglanguage.h"
#include "timedsignalqueue.h"
#include "qtsingleapplication/src/QtSingleApplication"

#include "geoiplist.h"
#include "network.h"
#include "queryhashmaster.h"
#include "dialogsplash.h"
#include "dialoglanguage.h"
#include "wizardquickstart.h"
#include "sharemanager.h"
#include "commonfunctions.h"

#include "Security/security.h"

#ifdef Q_OS_LINUX
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif // Q_OS_LINUX

#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const char *msg)
{
	switch (type) {
	case QtDebugMsg:
		fprintf(stderr, "Debug: %s\n", msg);
		break;
	case QtWarningMsg:
		fprintf(stderr, "Warning: %s\n", msg);
		break;
	case QtCriticalMsg:
		fprintf(stderr, "Critical: %s\n", msg);
		break;
	case QtFatalMsg:
		fprintf(stderr, "Fatal: %s\n", msg);
		abort();
	}
}

QuazaaGlobals quazaaGlobals;

int main(int argc, char *argv[])
{
	//qInstallMsgHandler(myMessageOutput);

	QtSingleApplication theApp( argc, argv );

	// Check if the application is already running
	if( theApp.sendMessage( "Show App" ) )
	{
		return 0;
	}

	qsrand( time( 0 ) );

#ifdef Q_OS_LINUX

	rlimit sLimit;
	memset( &sLimit, 0, sizeof( rlimit ) );
	getrlimit( RLIMIT_NOFILE, &sLimit );

	sLimit.rlim_cur = sLimit.rlim_max;

	if( setrlimit( RLIMIT_NOFILE, &sLimit ) == 0 )
	{
		qDebug() << "Successfully raised resource limits";
	}
	else
	{
		qDebug() << "Cannot set resource limits";
	}

#endif // Q_OS_LINUX

	theApp.setApplicationName(    QuazaaGlobals::APPLICATION_NAME() );
	theApp.setApplicationVersion( QuazaaGlobals::APPLICATION_VERSION_STRING() );
	theApp.setOrganizationDomain( QuazaaGlobals::APPLICATION_ORGANIZATION_DOMAIN() );
	theApp.setOrganizationName(   QuazaaGlobals::APPLICATION_ORGANIZATION_NAME() );

	// Setup Qt elements of signal queue necessary for operation
	signalQueue.setup();

	//Initialize multilanguage support
	quazaaSettings.loadLanguageSettings();
	quazaaSettings.translator.load( quazaaSettings.Language.File );
	qApp->installTranslator( &quazaaSettings.translator );

	//Create splash window
	DialogSplash* dlgSplash = new DialogSplash();
	dlgSplash->show();

	dlgSplash->updateProgress( 1, QObject::tr( "Loading settings..." ) );
	qApp->processEvents();

	//Initialize Settings
	quazaaSettings.loadSettings();

	//Check if this is Quazaa's first run
	dlgSplash->updateProgress( 5, QObject::tr( "Checking for first run..." ) );
	qApp->processEvents();
	if ( quazaaSettings.isFirstRun() )
	{
		DialogLanguage* dlgLanguage = new DialogLanguage();
		dlgLanguage->exec();

		dlgSplash->updateProgress( 10, QObject::tr( "Running first run wizard..." ) );
		quazaaSettings.saveFirstRun( false );
		quazaaSettings.saveSettings();
		quazaaSettings.saveProfile();

		WizardQuickStart* wzrdQuickStart = new WizardQuickStart();
		wzrdQuickStart->exec();
	}

	// Load Security Manager
	dlgSplash->updateProgress( 15, QObject::tr( "Loading Security Manager..." ) );
	qApp->processEvents();
	if(!securityManager.load()) systemLog.postLog(LogSeverity::Information, QObject::tr("Security data file was not available."));

	//Load profile
	dlgSplash->updateProgress( 20, QObject::tr( "Loading Profile..." ) );
	qApp->processEvents();
	quazaaSettings.loadProfile();

	//Load the networks
	//dlgSplash->updateProgress( 25, QObject::tr( "Loading Networks..." ) );
	//qApp->processEvents();

	//initialize geoip list
	GeoIP.loadGeoIP();

	//Load the library
	dlgSplash->updateProgress( 50, QObject::tr( "Loading Library..." ) );
	qApp->processEvents();
	QueryHashMaster.Create();
	ShareManager.Start();

	dlgSplash->updateProgress( 80, QObject::tr( "Loading User Interface..." ) );
	qApp->processEvents();

	MainWindow = new CWinMain();
	if ( quazaaSettings.WinMain.Visible )
	{
		MainWindow->show();
	}

	dlgSplash->updateProgress( 90, QObject::tr( "Loading Tray Icon..." ) );
	qApp->processEvents();
	MainWindow->loadTrayIcon();

	// Make the main window show if the user tried to open another instance
	QObject::connect( &theApp, SIGNAL( messageReceived( const QString& ) ),
					  MainWindow, SLOT( showOnTop() ));
	theApp.setActivationWindow( MainWindow );

	dlgSplash->updateProgress( 100, QObject::tr( "Welcome to Quazaa!" ) );
	qApp->processEvents();
	dlgSplash->close();
	dlgSplash->deleteLater();
	dlgSplash = 0;

	// Start networks if needed
	if( quazaaSettings.Gnutella2.Enable )
	{
		Network.Connect();
	}

	return theApp.exec();
}
