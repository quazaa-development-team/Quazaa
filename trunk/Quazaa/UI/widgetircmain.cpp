/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
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
#include "skinsettings.h"

#include "wizardircconnection.h"
#include "settingswizard.h"
#include "sessiontabwidget.h"
#include "maintabwidget.h"
#include "sharedtimer.h"
#include "welcomepage.h"
#include "connectioninfo.h"
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
	quazaaSettings.loadChat();
	qRegisterMetaTypeStreamOperators<ConnectionInfo>("Connection");
	qRegisterMetaTypeStreamOperators<ConnectionInfos>("Connections");

	connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(initialize()));
	connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnect()));
	if(quazaaSettings.Chat.ConnectOnStartup)
	{
		QTimer::singleShot(1000, this, SLOT(initialize()));
	}
	setSkin();
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

	ConnectionInfos connections;
	for (int i = 0; tabWidgetMain && i < tabWidgetMain->count(); ++i)
	{
		SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidgetMain->widget(i));
		if (tab)
		{
			connections += tab->session()->toConnection();
			tab->session()->quit();
		}
	}

	if(!connections.isEmpty())
	{
		quazaaSettings.Chat.Connections.setValue(QVariant::fromValue(connections));
		quazaaSettings.saveChatConnections();
	}
}

void WidgetIRCMain::on_actionChatSettings_triggered()
{
	SettingsWizard wizard(qApp->activeWindow());
	wizard.exec();
}

void WidgetIRCMain::on_actionEditMyProfile_triggered()
{
	DialogProfile* dlgProfile = new DialogProfile(this);
	dlgProfile->show();
}

void WidgetIRCMain::connectTo(const QString& host, quint16 port, const QString& nick, const QString& password)
{
	ConnectionInfo conn;
	conn.host = host;
	conn.port = port;
	conn.nick = nick;
	conn.pass = password;
	connectTo(conn);
}

void WidgetIRCMain::connectTo(const ConnectionInfo& connection)
{
	WizardIrcConnection* wizard = new WizardIrcConnection(this);
	wizard->setConnection(connection);

	if (!connection.host.isEmpty() && !connection.nick.isEmpty())
	{
		connectToImpl(connection);
		ui->actionConnect->setEnabled(false);
		ui->actionDisconnect->setEnabled(true);
	}
	else if (wizard->exec())
	{
		connectToImpl(wizard->connection());
		ui->actionConnect->setEnabled(false);
		ui->actionDisconnect->setEnabled(true);
	}
}

void WidgetIRCMain::connectToImpl(const ConnectionInfo &connection)
{
	if (!tabWidgetMain)
		createTabbedView();

	Session* session = Session::fromConnection(connection, this);
	session->setUserName(quazaaSettings.Profile.IrcUserName);
	if (session->ensureNetwork())
		session->open();

	SessionTabWidget* tab = new SessionTabWidget(session, tabWidgetMain);
	if (connection.name.isEmpty())
	connect(tab, SIGNAL(titleChanged(QString)), tabWidgetMain, SLOT(setSessionTitle(QString)));
	connect(tab, SIGNAL(inactiveStatusChanged(bool)), tabWidgetMain, SLOT(setInactive(bool)));
	connect(tab, SIGNAL(sessionAlerted(SessionTabWidget*,bool)), tabWidgetMain, SLOT(alertTab(SessionTabWidget*,bool)));
	connect(tab, SIGNAL(sessionHighlighted(SessionTabWidget*,bool)), tabWidgetMain, SLOT(highlightTab(SessionTabWidget*,bool)));
	connect(tab, SIGNAL(currentChanged(int)), this, SLOT(msgTabChanged(int)));

	int index = tabWidgetMain->addTab(tab, connection.name.isEmpty() ? session->host() : connection.name);
	tabWidgetMain->setCurrentIndex(index);
	tabWidgetMain->setTabInactive(index, !session->isActive());
}

void WidgetIRCMain::initialize()
{
	quazaaSettings.loadChatConnections();
	ConnectionInfos connections = quazaaSettings.Chat.Connections.value<ConnectionInfos>();

	foreach (const ConnectionInfo& connection, connections)
		connectToImpl(connection);

	if (connections.isEmpty())
	{
		connectTo(ConnectionInfo());
	} else {
		ui->actionConnect->setEnabled(false);
		ui->actionDisconnect->setEnabled(true);
	}
}

void WidgetIRCMain::disconnect()
{
	ConnectionInfos connections;
	while (1 < tabWidgetMain->count())
	{
		SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidgetMain->widget(0));
		if (tab)
		{
			ConnectionInfo connection = tab->session()->toConnection();
			connections += connection;
			tab->quit();
			tabWidgetMain->removeTab(0);
		}
	}

	if(!connections.isEmpty())
	{
		quazaaSettings.Chat.Connections.setValue(QVariant::fromValue(connections));
		quazaaSettings.saveChatConnections();
	}

	ui->actionConnect->setEnabled(true);
	ui->actionDisconnect->setEnabled(false);
}

void WidgetIRCMain::tabActivated(int index)
{
	if (index == -1)
	{
		createWelcomeView();
	}
	else if (index < tabWidgetMain->count() - 1)
	{
		SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidgetMain->widget(index));
		if (tab)
		{
			setWindowFilePath(tab->tabText(tab->currentIndex()));
			QMetaObject::invokeMethod(tab, "delayedTabReset");
			emit msgTabChanged(tab);
		}
	}
}

void WidgetIRCMain::createWelcomeView()
{
	WelcomePage* welcomePage = new WelcomePage(this);
	connect(welcomePage, SIGNAL(connectRequested()), this, SLOT(connectTo()));
	connect(welcomePage->connectButton, SIGNAL(clicked()), this, SLOT(initialize()));
	setCentralWidget(welcomePage);
	tabWidgetMain = 0;
}

void WidgetIRCMain::createTabbedView()
{
	tabWidgetMain = new MainTabWidget(this);
	setCentralWidget(tabWidgetMain);
	connect(tabWidgetMain, SIGNAL(newTabRequested()), this, SLOT(onNewTabRequested()), Qt::QueuedConnection);
	connect(tabWidgetMain, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
}

void WidgetIRCMain::onNewTabRequested()
{
	if (tabWidgetMain->count() == 1)
		createWelcomeView();
	else
		connectTo();
}

void WidgetIRCMain::setSkin()
{

}

void WidgetIRCMain::msgTabChanged(int)
{
	SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(sender());

	if( tab )
		emit msgTabChanged(tab);
}

SessionTabWidget* WidgetIRCMain::currentSession()
{
	return qobject_cast<SessionTabWidget*>(tabWidgetMain->currentWidget());
}
