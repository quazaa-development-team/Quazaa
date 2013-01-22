/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
* Copyright (C) 2010-2013 SmokeX <smokexjc@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "mainircwindow.h"
#include "application.h"
#include "connectionwizard.h"
#include "multisessiontabwidget.h"
#include "sessiontreewidget.h"
#include "sessiontabwidget.h"
#include "sessiontreeitem.h"
#include "connectioninfo.h"
#include "messageview.h"
#include "homepage.h"
#include "overlay.h"
#include "toolbar.h"
#include "session.h"
#include "qtdocktile.h"
#include "dialogircsettings.h"
#include "quazaasettings.h"
#include <QSettings>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QFile>

MainIrcWindow::MainIrcWindow(QWidget* parent) : QMainWindow(parent),
	treeWidget(0), trayIcon(0), dockTile(0)
{
	tabWidget = new MultiSessionTabWidget(this);
	qRegisterMetaTypeStreamOperators<ConnectionInfo>("ConnectionInfo");
	qRegisterMetaTypeStreamOperators<ConnectionInfos>("ConnectionInfos");

	connect(tabWidget, SIGNAL(newTabRequested()), this, SLOT(connectTo()), Qt::QueuedConnection);
	connect(tabWidget, SIGNAL(alerted(MessageView*, IrcMessage*)), this, SLOT(alert(MessageView*, IrcMessage*)));
	connect(tabWidget, SIGNAL(highlighted(MessageView*, IrcMessage*)), this, SLOT(highlight(MessageView*, IrcMessage*)));
	connect(tabWidget, SIGNAL(splitterChanged(QByteArray)), this, SLOT(splitterChanged(QByteArray)));
	connect(tabWidget, SIGNAL(sessionAdded(Session*)), this, SLOT(sessionAdded(Session*)));
	connect(tabWidget, SIGNAL(sessionRemoved(Session*)), this, SLOT(sessionRemoved(Session*)));

	HomePage* homePage = new HomePage(tabWidget);
    connect(homePage, SIGNAL(connectRequested()), this, SLOT(connectTo()));
	tabWidget->insertTab(0, homePage, tr("Home"));

	QSplitter* splitter = new QSplitter(this);
	splitter->setHandleWidth(1);
	splitter->addWidget(tabWidget);
	setCentralWidget(splitter);

	if (QtDockTile::isAvailable())
		dockTile = new QtDockTile(this);

	QShortcut* shortcut = new QShortcut(QKeySequence(tr("Ctrl+Q")), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

#ifdef Q_WS_MAC
	QMenu* menu = new QMenu(this);
	menuBar()->addMenu(menu);

	QAction* action = new QAction(tr("Connect"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(connectTo()));
	menu->addAction(action);

	action = new QAction(tr("Settings"), this);
	action->setMenuRole(QAction::PreferencesRole);
	connect(action, SIGNAL(triggered()), qApp, SLOT(showSettings()));
	menu->addAction(action);

	action = new QAction(tr("About %1").arg(Application::applicationName()), this);
	action->setMenuRole(QAction::AboutRole);
	connect(action, SIGNAL(triggered()), qApp, SLOT(aboutApplication()));
	menu->addAction(action);

	action = new QAction(tr("About Qt"), this);
	action->setMenuRole(QAction::AboutQtRole);
	connect(action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	menu->addAction(action);
#endif // Q_WS_MAC

	quazaaSettings.loadChat();

	applySettings();
	connect(&quazaaSettings, SIGNAL(chatSettingsChanged()), this, SLOT(applySettings()));

	QFile file(":Resource/stylesheet.css");
	if (file.open(QFile::ReadOnly | QIODevice::Text))
		setStyleSheet(QString::fromUtf8(file.readAll()));

	if(quazaaSettings.Chat.ConnectOnStartup)
		QTimer::singleShot(1000, this, SLOT(initialize()));
}

MainIrcWindow::~MainIrcWindow()
{
}

QSize MainIrcWindow::sizeHint() const
{
	return QSize(800, 600);
}

void MainIrcWindow::connectTo(const QString& host, quint16 port, const QString& nick, const QString& password)
{
	ConnectionInfo conn;
	conn.host = host;
	conn.port = port;
	conn.nick = nick;
	conn.pass = password;
	connectTo(conn);
}

void MainIrcWindow::connectTo(const ConnectionInfo& connection)
{
	ConnectionWizard wizard;
	wizard.setConnection(connection);

	if (!connection.host.isEmpty() && !connection.nick.isEmpty())
		connectToImpl(connection);
	else if (wizard.exec())
		connectToImpl(wizard.connection());
}

void MainIrcWindow::connectToImpl(const ConnectionInfo& connection)
{
	Session* session = Session::fromConnection(connection, this);
	session->setEncoding(Application::encoding());
    qDebug() << "User name is:" << session->userName();
    if (session->userName().isEmpty())
        session->setUserName("quazaa");
	if (!session->hasQuit() && session->ensureNetwork())
		session->open();
    tabWidget->addSession(session);

	connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateSession()));
	connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateSession()));
	updateSession(session);

    SessionTabWidget* tab = tabWidget->sessionWidget(session);
    connect(tab, SIGNAL(viewAdded(MessageView*)), this, SLOT(viewAdded(MessageView*)));
    connect(tab, SIGNAL(viewRemoved(MessageView*)), this, SLOT(viewRemoved(MessageView*)));
    connect(tab, SIGNAL(viewRenamed(MessageView*)), this, SLOT(viewRenamed(MessageView*)));
    connect(tab, SIGNAL(viewActivated(MessageView*)), this, SLOT(viewActivated(MessageView*)));
    connect(tab, SIGNAL(editSession(Session*)), this, SLOT(editSession(Session*)));

    if (!quazaaSettings.WinMain.ChatUserListSplitter.isEmpty())
        tab->restoreSplitter(quazaaSettings.WinMain.ChatUserListSplitter);
}

void MainIrcWindow::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);
}

void MainIrcWindow::changeEvent(QEvent* event)
{
	QMainWindow::changeEvent(event);
	if (event->type() == QEvent::ActivationChange) {
		if (isActiveWindow()) {
			if (dockTile)
				dockTile->setBadge(0);
		}
	}
}

void MainIrcWindow::saveWidget()
{
	if (treeWidget)
    {
        quazaaSettings.WinMain.ChatTreeWidget = treeWidget->saveState();
        quazaaSettings.WinMain.ChatTreeWidgetSplitter = static_cast<QSplitter*>(centralWidget())->saveState();
    }

    ConnectionInfos connections;
    QList<Session*> sessions = tabWidget->sessions();
    foreach(Session * session, sessions) {
        connections += session->toConnection();
        session->quit();
    }
    quazaaSettings.Chat.Connections.setValue(QVariant::fromValue(connections));
    quazaaSettings.saveChatConnections();
}

void MainIrcWindow::initialize()
{
    quazaaSettings.loadChatConnections();
    ConnectionInfos connections = quazaaSettings.Chat.Connections.value<ConnectionInfos>();

    foreach(const ConnectionInfo & connection, connections)
    connectToImpl(connection);

    if (connections.isEmpty())
        connectTo(ConnectionInfo());
}

void MainIrcWindow::editSession(Session* session)
{
    ConnectionWizard wizard;
    wizard.setConnection(session->toConnection());
    if (wizard.exec())
        session->initFrom(wizard.connection());
}

void MainIrcWindow::applySettings()
{
    tabWidget->applySettings();
    if (quazaaSettings.Chat.Layout == "tree") {
        if (!treeWidget)
            createTree();
        // TODO: Change method to use quazaaSettings
        treeWidget->applySettings();
        if((tabWidget->currentIndex() == 0) && (tabWidget->count() > 0))
            tabWidget->setCurrentIndex(1);
    } else if (treeWidget) {
        treeWidget->parentWidget()->deleteLater();
        treeWidget = 0;
    }
    // refresh stylesheet (required for styles depending on dynamic properties)

    QString css = styleSheet();
    setStyleSheet("");
    setStyleSheet(css);
}

void MainIrcWindow::showSettings()
{
    DialogIrcSettings *dlgIrcSettings = new DialogIrcSettings(this);

    dlgIrcSettings->exec();
}

void MainIrcWindow::alert(MessageView* view, IrcMessage* message)
{
    Q_UNUSED(message);
    if (!isActiveWindow()) {
        QApplication::alert(this);
        if (dockTile)
            dockTile->setBadge(dockTile->badge() + 1);
    }

    if (treeWidget) {
        SessionTreeItem* item = treeWidget->sessionItem(view->session());
        if (view->viewType() != MessageView::ServerView)
            item = item->findChild(view->receiver());
        if (item) {
            item->setAlerted(true);
            treeWidget->alert(item);
        }
    }
}

void MainIrcWindow::highlight(MessageView* view, IrcMessage* message)
{
    Q_UNUSED(message);
    if (treeWidget) {
        SessionTreeItem* item = treeWidget->sessionItem(view->session());
        if (view->viewType() != MessageView::ServerView)
            item = item->findChild(view->receiver());
        if (item)
            item->setHighlighted(true);
    }
}

void MainIrcWindow::viewAdded(MessageView* view)
{
    if (!quazaaSettings.WinMain.ChatUserListSplitter.isEmpty())
        view->restoreSplitter(quazaaSettings.WinMain.ChatUserListSplitter);

    if (treeWidget) {
        Session* session = view->session();
        treeWidget->addView(view);
        	treeWidget->restoreState(quazaaSettings.WinMain.ChatTreeWidget);
        treeWidget->expandItem(treeWidget->sessionItem(session));
    }
}

void MainIrcWindow::viewRemoved(MessageView* view)
{
    if (treeWidget) {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(sender());
        if (tab)
            treeWidget->removeView(view);
    }
}

void MainIrcWindow::viewRenamed(MessageView* view)
{
    if (treeWidget)
        treeWidget->renameView(view);
}

void MainIrcWindow::viewActivated(MessageView* view)
{
    if (treeWidget)
        treeWidget->setCurrentView(view);
}

void MainIrcWindow::closeTreeItem(SessionTreeItem* item)
{
    SessionTabWidget* tab = tabWidget->sessionWidget(item->session());
    if (tab)
        tab->closeView(tab->indexOf(item->view()));
}

void MainIrcWindow::currentTreeItemChanged(Session* session, const QString& view)
{
    SessionTabWidget* tab = tabWidget->sessionWidget(session);
    if (tab) {
        tabWidget->setCurrentWidget(tab);
        if (view.isEmpty())
            tab->setCurrentIndex(0);
        else
            tab->openView(view);
    }
}

void MainIrcWindow::splitterChanged(const QByteArray& state)
{
    quazaaSettings.WinMain.ChatUserListSplitter = state;
}

void MainIrcWindow::sessionAdded(Session* session)
{
    if (treeWidget) {
        if (SessionTabWidget* tab = tabWidget->sessionWidget(session)) {
            if (MessageView* view = qobject_cast<MessageView*>(tab->widget(0))) {
                treeWidget->addView(view);
                treeWidget->parentWidget()->show();
            }
        }
    }
}

void MainIrcWindow::sessionRemoved(Session* session)
{
    if (treeWidget) {
        if (SessionTabWidget* tab = tabWidget->sessionWidget(session)) {
            if (MessageView* view = qobject_cast<MessageView*>(tab->widget(0)))
                treeWidget->removeView(view);
        }
        treeWidget->parentWidget()->setVisible(!tabWidget->sessions().isEmpty());
    }
}

void MainIrcWindow::updateSession(Session* session)
{
    if (!session)
        session = qobject_cast<Session*>(sender());
    SessionTabWidget* tab = tabWidget->sessionWidget(session);
    if (tab) {
        if (!tab->session()->isConnected()) {
            QObject* overlay = tab->property("_communi_overlay_").value<QObject*>();
            if (!overlay) {
                overlay = new Overlay(tab);
                tab->setProperty("_communi_overlay_", QVariant::fromValue(overlay));
                connect(overlay, SIGNAL(refresh()), tab->session(), SLOT(reconnect()));
            }
            overlay->setProperty("visible", true);
            overlay->setProperty("busy", tab->session()->isActive());
            overlay->setProperty("refresh", !tab->session()->isActive());
        } else {
            delete tab->property("_communi_overlay_").value<QObject*>();
            tab->setProperty("_communi_overlay_", QVariant());
        }
    }
}

void MainIrcWindow::addView()
{
    SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidget->currentWidget());
    if (tab)
        QMetaObject::invokeMethod(tab, "onNewTabRequested");
}

void MainIrcWindow::createTree()
{
    QSplitter* splitter = static_cast<QSplitter*>(centralWidget());
    QWidget* container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    container->setVisible(!tabWidget->sessions().isEmpty());

    treeWidget = new SessionTreeWidget(container);
    treeWidget->setFocusPolicy(Qt::NoFocus);

    connect(treeWidget, SIGNAL(editSession(Session*)), this, SLOT(editSession(Session*)));
    connect(treeWidget, SIGNAL(closeItem(SessionTreeItem*)), this, SLOT(closeTreeItem(SessionTreeItem*)));
    connect(treeWidget, SIGNAL(currentViewChanged(Session*, QString)), this, SLOT(currentTreeItemChanged(Session*, QString)));

    foreach (Session* session, tabWidget->sessions()) {
        SessionTabWidget* tab = tabWidget->sessionWidget(session);
        if (MessageView* view = qobject_cast<MessageView*>(tab->widget(0)))
            treeWidget->addView(view);
        for (int i = 1; i < tab->count() - 1; ++i) {
            MessageView* view = qobject_cast<MessageView*>(tab->widget(i));
            if (view)
                treeWidget->addView(view);
        }
    }
    treeWidget->expandAll();

    ToolBar* toolBar = new ToolBar(container);
    connect(toolBar, SIGNAL(settingsTriggered()), this, SLOT(showSettings()));
    connect(toolBar, SIGNAL(connectTriggered()), this, SLOT(connectTo()));
    connect(toolBar, SIGNAL(joinTriggered()), this, SLOT(addView()));

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->addWidget(treeWidget);
    layout->addWidget(toolBar);
    layout->setSpacing(0);
    layout->setMargin(0);

    container->setMinimumWidth(toolBar->sizeHint().width());
    splitter->insertWidget(0, container);
    splitter->setStretchFactor(1, 1);

    if (!quazaaSettings.WinMain.ChatTreeWidgetSplitter.isEmpty())
        splitter->restoreState(quazaaSettings.WinMain.ChatTreeWidgetSplitter);
}
