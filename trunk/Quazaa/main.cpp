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
#include "persistentsettings.h"
#include "dialoglanguage.h"

int main(int argc, char *argv[])
{
	QApplication theApp(argc, argv);
	Globals.InitializeGlobals();
	QApplication::setApplicationName(Globals.Application.Name);
	QApplication::setApplicationVersion(Globals.Application.VersionString);
	QApplication::setOrganizationDomain(Globals.Application.OrganizationDomain);
	QApplication::setOrganizationName(Globals.Application.OrganizationName);


	QSettings m_qSettings(Globals.Application.OrganizationName, Globals.Application.Name);
	QSharedMemory applicationRunning("QuazaaAlreadyRunning");
	QMessageBox infoBox;

	//Initialize multilanguage support
	Settings.loadLanguage();
	m_qSettings.beginGroup("Basic");
	Settings.Basic.FirstRun = m_qSettings.value("FirstRun", true).toBool();
	m_qSettings.endGroup();
	if (Settings.Basic.FirstRun)
	{
		DialogLanguage *dlgLanguage = new DialogLanguage();
		dlgLanguage->exec();
	}
	if (!Settings.Basic.DefaultLanguage)
	{
		QFile languageFile(Settings.Basic.LanguageFile);
		QFileInfo languageFileInfo(languageFile);
		QTranslator *translator;

		if (languageFile.open(QIODevice::ReadOnly))
		{
			QString Language = languageFile.readAll();
			translator->load(languageFileInfo.fileName(), languageFileInfo.filePath());
			theApp.installTranslator(translator);
		}
	}
	// Check if the application is already running
	if (applicationRunning.attach()) {
		infoBox.setText(QObject::tr("Quazaa is already running. Only one instance of Quazaa can be running at a time."));
		infoBox.exec();
		return 0;
	} else {
		bool quazaaRunning = true;
		QBuffer buffer;
		buffer.open(QBuffer::ReadWrite);
		QDataStream out(&buffer);
		out << quazaaRunning;
		int size = buffer.size();
		if (!applicationRunning.create(size))
		{
			infoBox.setText(QObject::tr("Unable to create shared memory segment."));
			infoBox.exec();
		} else {
		applicationRunning.lock();
		char *to = (char*)applicationRunning.data();
		const char *from = buffer.data().data();
		memcpy(to, from, qMin(applicationRunning.size(), size));
		applicationRunning.unlock();
		}
	}
	theApp.processEvents();
	MainWindow winMain;
	bool m_bIsVisible;

	m_qSettings.beginGroup("MainWindow");
	m_bIsVisible = m_qSettings.value("WindowVisible", true).toBool();
	m_qSettings.endGroup();
	if ( m_bIsVisible )
	{
		winMain.show();
	}
	return theApp.exec();
	applicationRunning.detach();
}
