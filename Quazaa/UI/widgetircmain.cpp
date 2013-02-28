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

#include "widgetircmain.h"
#include "application.h"
#include "wizardircconnection.h"
#include "multisessiontabwidget.h"
#include "sessiontreewidget.h"
#include "sessiontabwidget.h"
#include "sessiontreeitem.h"
#include "connectioninfo.h"
#include "widgetircmessageview.h"
#include "homepage.h"
#include "overlay.h"
#include "toolbar.h"
#include "session.h"
#include "qtdocktile.h"
#include "dialogircsettings.h"
#include "quazaasettings.h"
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QFile>

WidgetIrcMain::WidgetIrcMain(QWidget* parent) : QWidget(parent),
    treeWidget(0), dockTile(0)
{
	tabWidget = new MultiSessionTabWidget(this);
	qRegisterMetaTypeStreamOperators<ConnectionInfo>("ConnectionInfo");
	qRegisterMetaTypeStreamOperators<ConnectionInfos>("ConnectionInfos");

    connect(tabWidget, SIGNAL(newTabRequested()), this, SLOT(connectTo()), Qt::QueuedConnection);
    connect(tabWidget, SIGNAL(splitterChanged(QByteArray)), this, SLOT(splitterChanged(QByteArray)));

	WidgetIrcHomePage* homePage = new WidgetIrcHomePage(tabWidget);
    connect(homePage, SIGNAL(connectRequested()), this, SLOT(initialize()));
	tabWidget->insertTab(0, homePage, tr("Home"));

    splitterIrcMain = new QSplitter(this);
    splitterIrcMain->setHandleWidth(1);
    splitterIrcMain->addWidget(tabWidget);

    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    setLayout(centralLayout);
    centralLayout->addWidget(splitterIrcMain);

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
        QTimer::singleShot(600, this, SLOT(initialize()));
}

WidgetIrcMain::~WidgetIrcMain()
{
}

QSize WidgetIrcMain::sizeHint() const
{
	return QSize(800, 600);
}

void WidgetIrcMain::connectTo(const QString& host, quint16 port, const QString& nick, const QString& password)
{
	ConnectionInfo conn;
	conn.host = host;
	conn.port = port;
	conn.nick = nick;
	conn.pass = password;
	connectTo(conn);
}

void WidgetIrcMain::connectTo(const ConnectionInfo& connection)
{
    WizardIrcConnection wizard;
	wizard.setConnection(connection);

	if (!connection.host.isEmpty() && !connection.nick.isEmpty())
		connectToImpl(connection);
	else if (wizard.exec())
		connectToImpl(wizard.connection());
}

void WidgetIrcMain::connectToImpl(const ConnectionInfo& connection)
{
	Session* session = Session::fromConnection(connection, this);
    session->setEncoding(Application::encoding());
    if (session->userName().isEmpty())
        session->setUserName("quazaa");
	if (!session->hasQuit() && session->ensureNetwork())
		session->open();
    tabWidget->addSession(session);

	connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateSession()));
	connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateSession()));
	updateSession(session);

    SessionTabWidget* tab = tabWidget->sessionWidget(session);
    connect(tab, SIGNAL(viewAdded(WidgetIrcMessageView*)), this, SLOT(viewAdded(WidgetIrcMessageView*)));
    connect(tab, SIGNAL(viewRemoved(WidgetIrcMessageView*)), this, SLOT(viewRemoved(WidgetIrcMessageView*)));
    connect(tab, SIGNAL(viewRenamed(WidgetIrcMessageView*)), this, SLOT(viewRenamed(WidgetIrcMessageView*)));
    connect(tab, SIGNAL(viewActivated(WidgetIrcMessageView*)), this, SLOT(viewActivated(WidgetIrcMessageView*)));
    connect(tab, SIGNAL(editSession(Session*)), this, SLOT(editSession(Session*)));

    if (WidgetIrcMessageView* view = tab->viewAt(0)) {
        treeWidget->addView(view);
        treeWidget->setCurrentView(view);
        treeWidget->parentWidget()->show();
        view->applySettings();
    }

    if (!quazaaSettings.WinMain.ChatUserListSplitter.isEmpty())
        tab->restoreSplitter(quazaaSettings.WinMain.ChatUserListSplitter);
}

void WidgetIrcMain::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
}

void WidgetIrcMain::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
	if (event->type() == QEvent::ActivationChange) {
		if (isActiveWindow()) {
			if (dockTile)
				dockTile->setBadge(0);
		}
	}
}

void WidgetIrcMain::saveWidget()
{
	if (treeWidget)
    {
        quazaaSettings.WinMain.ChatTreeWidget = treeWidget->saveState();
        quazaaSettings.WinMain.ChatTreeWidgetSplitter = splitterIrcMain->saveState();
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

void WidgetIrcMain::initialize()
{
    quazaaSettings.loadChatConnections();
    ConnectionInfos connections = quazaaSettings.Chat.Connections.value<ConnectionInfos>();

    foreach(const ConnectionInfo & connection, connections)
    connectToImpl(connection);

    if (connections.isEmpty())
        connectTo(ConnectionInfo());
}

void WidgetIrcMain::editSession(Session* session)
{
    WizardIrcConnection wizard;
    wizard.setConnection(session->toConnection());
    if (wizard.exec())
        session->initFrom(wizard.connection());
}

void WidgetIrcMain::applySettings()
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

void WidgetIrcMain::showSettings()
{
    DialogIrcSettings *dlgIrcSettings = new DialogIrcSettings(this);

    dlgIrcSettings->exec();
}

void WidgetIrcMain::highlighted(IrcMessage* message)
{
    Q_UNUSED(message);
    if (!isActiveWindow()) {
        QApplication::alert(this);
        if (dockTile)
            dockTile->setBadge(dockTile->badge() + 1);
    }

    if (treeWidget) {
        WidgetIrcMessageView* view = qobject_cast<WidgetIrcMessageView*>(sender());
        if (view) {
            SessionTreeItem* item = treeWidget->sessionItem(view->session());
            if (view->viewType() != WidgetIrcMessageView::ServerView)
                item = item->findChild(view->receiver());
            if (item) {
                item->setHighlighted(true);
                item->setBadge(item->badge() + 1);
            }
        }
    }
}

void WidgetIrcMain::missed(IrcMessage* message)
{
    Q_UNUSED(message);

    WidgetIrcMessageView* view = qobject_cast<WidgetIrcMessageView*>(sender());
    if (view) {
        SessionTreeItem* item = treeWidget->sessionItem(view->session());
        if (view->viewType() != WidgetIrcMessageView::ServerView)
            item = item->findChild(view->receiver());
        if (item)
            item->setBadge(item->badge() + 1);
    }
}

void WidgetIrcMain::viewAdded(WidgetIrcMessageView* view)
{
    connect(view, SIGNAL(highlighted(IrcMessage*)), this, SLOT(highlighted(IrcMessage*)));
    connect(view, SIGNAL(missed(IrcMessage*)), this, SLOT(missed(IrcMessage*)));

    if (!quazaaSettings.WinMain.ChatUserListSplitter.isEmpty())
        view->restoreSplitter(quazaaSettings.WinMain.ChatUserListSplitter);

    if (treeWidget) {
        Session* session = view->session();
        treeWidget->addView(view);
        	treeWidget->restoreState(quazaaSettings.WinMain.ChatTreeWidget);
        treeWidget->expandItem(treeWidget->sessionItem(session));
    }
}

void WidgetIrcMain::viewRemoved(WidgetIrcMessageView* view)
{
    if (treeWidget) {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(sender());
        if (tab)
            treeWidget->removeView(view);
    }
}

void WidgetIrcMain::viewRenamed(WidgetIrcMessageView* view)
{
    if (treeWidget)
        treeWidget->renameView(view);
}

void WidgetIrcMain::viewActivated(WidgetIrcMessageView* view)
{
    if (treeWidget)
        treeWidget->setCurrentView(view);
}

void WidgetIrcMain::closeTreeItem(SessionTreeItem* item)
{
    SessionTabWidget* tab = tabWidget->sessionWidget(item->session());
    if (tab) {
        int index = tab->indexOf(item->view());
        tab->closeView(index);
        if (index == 0) {
            tabWidget->removeSession(tab->session());
            treeWidget->parentWidget()->setVisible(!tabWidget->sessions().isEmpty());
        }
    }
}

void WidgetIrcMain::currentTreeItemChanged(Session* session, const QString& view)
{
    SessionTabWidget* tab = tabWidget->sessionWidget(session);
    if (tab) {
        tabWidget->setCurrentWidget(tab);
        if (view.isEmpty())
            tab->setCurrentIndex(0);
        else
            tab->openView(view);
        }
        setWindowFilePath(view);
}

void WidgetIrcMain::splitterChanged(const QByteArray& state)
{
    quazaaSettings.WinMain.ChatUserListSplitter = state;
}

void WidgetIrcMain::updateSession(Session* session)
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

void WidgetIrcMain::addView()
{
    SessionTabWidget* tab = tabWidget->currentWidget();
    if (tab)
        QMetaObject::invokeMethod(tab, "onNewTabRequested");
}

void WidgetIrcMain::createTree()
{
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
        if (WidgetIrcMessageView* view = qobject_cast<WidgetIrcMessageView*>(tab->widget(0)))
            treeWidget->addView(view);
        for (int i = 1; i < tab->count() - 1; ++i) {
            WidgetIrcMessageView* view = qobject_cast<WidgetIrcMessageView*>(tab->widget(i));
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
    splitterIrcMain->insertWidget(0, container);
    splitterIrcMain->setStretchFactor(1, 1);

    if (!quazaaSettings.WinMain.ChatTreeWidgetSplitter.isEmpty())
        splitterIrcMain->restoreState(quazaaSettings.WinMain.ChatTreeWidgetSplitter);
}
