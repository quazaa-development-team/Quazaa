/*
** $Id: widgetchatmiddle.cpp 779 2011-10-27 17:44:44Z brov $
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

#include "widgetircsidebars.h"
#include "ui_widgetircsidebars.h"
#include "widgetircmain.h"
#include "ui_widgetircmain.h"
#include "dialogsettings.h"
#include "dialogprofile.h"
#include "systemlog.h"
#include "chatconverter.h"

#include "quazaasettings.h"

#include "connectionwizard.h"
#include "sessiontabwidget.h"
#include "maintabwidget.h"
#include "sharedtimer.h"
#include "welcomepage.h"
#include "connection.h"
#include "session.h"

#include <irccommand.h>

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetIRCMain::WidgetIRCMain(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetIRCMain)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.ChatToolbars);
	createWelcomeView();
	qRegisterMetaTypeStreamOperators<Connection>("Connection");
	qRegisterMetaTypeStreamOperators<Connections>("Connections");

	widgetChatInput = new WidgetChatInput(this, true);
	ui->horizontalLayoutTextInput->addWidget(widgetChatInput);

	QTimer::singleShot(1000, this, SLOT(initialize()));
}

WidgetIRCMain::~WidgetIRCMain()
{
	delete ui;
}

void WidgetIRCMain::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		case QEvent::ActivationChange:
			if (isActiveWindow())
			{
				if (tabWidgetMain)
				{
					SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidgetMain->currentWidget());
					if (tab)
						QMetaObject::invokeMethod(tab, "delayedTabReset");
				}
			}
			break;
		default:
			break;

	}
}

void WidgetIRCMain::saveWidget()
{
	quazaaSettings.saveSettings();
	quazaaSettings.WinMain.ChatToolbars = saveState();
}

void WidgetIRCMain::on_actionChatSettings_triggered()
{
	DialogSettings* dlgSettings = new DialogSettings(this, SettingsPage::Chat);
	dlgSettings->show();
}

void WidgetIRCMain::on_actionEditMyProfile_triggered()
{
	DialogProfile* dlgProfile = new DialogProfile(this);
	dlgProfile->show();
}

void WidgetIRCMain::connectTo(const QString& host, quint16 port, const QString& nick, const QString& password)
{
	Connection conn;
	conn.host = host;
	conn.port = port;
	conn.nick = nick;
	conn.pass = password;
	connectTo(conn);
}

void WidgetIRCMain::connectTo(const Connection& connection)
{
	ConnectionWizard wizard;
	wizard.setConnection(connection);

	if (!connection.host.isEmpty() && !connection.nick.isEmpty())
		connectToImpl(connection);
	else if (wizard.exec())
		connectToImpl(wizard.connection());
}

void WidgetIRCMain::connectToImpl(const Connection& connection)
{
	if (!tabWidgetMain)
		createTabbedView();

	Session* session = new Session(this);
	session->connectTo(connection);

	SessionTabWidget* tab = new SessionTabWidget(session, tabWidgetMain);
	if (connection.name.isEmpty())
	connect(tab, SIGNAL(titleChanged(QString)), tabWidgetMain, SLOT(setSessionTitle(QString)));
	connect(tab, SIGNAL(alertStatusChanged(bool)), tabWidgetMain, SLOT(activateAlert(bool)));
	connect(tab, SIGNAL(highlightStatusChanged(bool)), tabWidgetMain, SLOT(activateHighlight(bool)));

	int index = tabWidgetMain->addTab(tab, connection.name.isEmpty() ? session->host() : connection.name);
	tabWidgetMain->setCurrentIndex(index);
}

void WidgetIRCMain::closeEvent(QCloseEvent* event)
{
	Connections connections;
	for (int i = 0; tabWidgetMain && i < tabWidgetMain->count(); ++i)
	{
		SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidgetMain->widget(i));
		if (tab)
		{
			Connection connection = tab->session()->connection();
			connection.nick = tab->session()->nickName();
			connection.channels = tab->channels();
			connections += connection;
			tab->quit();
		}
	}
	quazaaSettings.Chat.Connections = QVariant::fromValue(connections);
	quazaaSettings.saveChatConnections();

	QMainWindow::closeEvent(event);
}

void WidgetIRCMain::initialize()
{
	quazaaSettings.loadChatConnections();
	Connections connections = quazaaSettings.Chat.Connections.value<Connections>();

	foreach (const Connection& connection, connections)
		connectToImpl(connection);

	if (connections.isEmpty())
		connectTo(Connection());
}

void WidgetIRCMain::tabActivated(int index)
{
	if (index == -1)
	{
		createWelcomeView();
	}
	else if (index < tabWidgetMain->count() - 1)
	{
		QTabWidget* tab = qobject_cast<QTabWidget*>(tabWidgetMain->widget(index));
		if (tab)
		{
			setWindowFilePath(tab->tabText(tab->currentIndex()));
			QMetaObject::invokeMethod(tab, "delayedTabReset");
		}
	}
}

void WidgetIRCMain::createWelcomeView()
{
	WelcomePage* welcomePage = new WelcomePage(this);
	connect(welcomePage, SIGNAL(connectRequested()), this, SLOT(connectTo()));
	setCentralWidget(welcomePage);
	tabWidgetMain = 0;
}

void WidgetIRCMain::createTabbedView()
{
	tabWidgetMain = new MainTabWidget(this);
	setCentralWidget(tabWidgetMain);
	connect(tabWidgetMain, SIGNAL(newTabRequested()), this, SLOT(onNewTabRequested()), Qt::QueuedConnection);
	connect(tabWidgetMain, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
	connect(tabWidgetMain, SIGNAL(alertStatusChanged(bool)), this, SLOT(activateAlert(bool)));
}

void WidgetIRCMain::onNewTabRequested()
{
	if (tabWidgetMain->count() == 1)
		createWelcomeView();
	else
		connectTo();
}


