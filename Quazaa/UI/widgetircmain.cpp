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
#include "singleapplication.h"
#include "ignoremanager.h"
#include "wizardircconnection.h"
#include "sessionstackview.h"
#include "soundnotification.h"
#include "sessiontreewidget.h"
#include "messagestackview.h"
#include "sessiontreeitem.h"
#include "connectioninfo.h"
#include "addviewdialog.h"
#include "systemnotifier.h"
#include "searchpopup.h"
#include "messageview.h"
#include "homepage.h"
#include "overlay.h"
#include "toolbar.h"
#include "connection.h"
#include "qtdocktile.h"
#include "quazaasettings.h"
#include "dialogircsettings.h"
#include <QCloseEvent>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QDebug>

CWidgetIrcMain::CWidgetIrcMain(QWidget* parent) : QMainWindow(parent),
	treeWidget(0), muteAction(0), dockTile(0), sound(0), homePage(0)
{
	qRegisterMetaTypeStreamOperators<ConnectionInfo>("ConnectionInfo");
	qRegisterMetaTypeStreamOperators<ConnectionInfos>("ConnectionInfos");

	quazaaSettings.loadChat();

	stackView = new SessionStackView(this);

	splitterIrcMain = new QSplitter(this);
	splitterIrcMain->setHandleWidth(1);
	splitterIrcMain->addWidget(stackView);
	setCentralWidget(splitterIrcMain);

	createHome();

	createTree();

	if (QtDockTile::isAvailable())
		dockTile = new QtDockTile(this);

	if (SoundNotification::isAvailable())
		sound = new SoundNotification(this);

	QShortcut* shortcut = new QShortcut(QKeySequence::Quit, this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

	shortcut = new QShortcut(QKeySequence::New, this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(connectTo()), Qt::QueuedConnection);

	shortcut = new QShortcut(QKeySequence::AddTab, this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(addView()), Qt::QueuedConnection);

	shortcut = new QShortcut(QKeySequence::Close, this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(closeView()));

	searchShortcut = new QShortcut(this);
	connect(searchShortcut, SIGNAL(activated()), this, SLOT(searchView()));

#ifdef Q_OS_MAC
	QMenu* menu = new QMenu(this);
	menuBar()->addMenu(menu);

	QAction* action = new QAction(tr("Connect"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(connectTo()));
	menu->addAction(action);

	action = new QAction(tr("Settings"), this);
	action->setMenuRole(QAction::PreferencesRole);
	connect(action, SIGNAL(triggered()), this, SLOT(showSettings()));
	menu->addAction(action);
#endif // Q_OS_MAC

	applySettings();
	connect(&quazaaSettings, SIGNAL(chatSettingsChanged()), this, SLOT(applySettings()));

	if(quazaaSettings.Chat.ConnectOnStartup)
		QTimer::singleShot(600, this, SLOT(initialize()));
}

CWidgetIrcMain::~CWidgetIrcMain()
{
}

QSize CWidgetIrcMain::sizeHint() const
{
	return QSize(800, 600);
}

void CWidgetIrcMain::closeEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);
}

void CWidgetIrcMain::changeEvent(QEvent* event)
{
	QWidget::changeEvent(event);
	if (event->type() == QEvent::ActivationChange) {
		if (isActiveWindow()) {
			if (dockTile)
				dockTile->setBadge(0);
		}
	}
}

void CWidgetIrcMain::saveWidget()
{
	quazaaSettings.WinMain.ChatTreeWidget = treeWidget->saveState();
	quazaaSettings.WinMain.ChatTreeWidgetSplitter = splitterIrcMain->saveState();

	if (muteAction)
		quazaaSettings.Chat.MuteNotifications = muteAction->isChecked();
	quazaaSettings.Chat.Ignores = IgnoreManager::instance()->ignores();


	QList<IrcConnection*> connections = stackView->connections();
	if(!connections.isEmpty()) {
		ConnectionInfos infos;
		foreach (IrcConnection* c, connections) {
			if (Connection* connection = qobject_cast<Connection*>(c)) { // TODO
				ConnectionInfo info = ConnectionInfo::fromConnection(connection);
				info.views = treeWidget->viewInfos(connection);
				infos += info;
				connection->quit();
				connection->disconnect();
			}
		}

		if(!infos.isEmpty()) {
			quazaaSettings.Chat.Connections.setValue(QVariant::fromValue(infos));
			quazaaSettings.saveChatConnections();
		}
	}
}

void CWidgetIrcMain::connectTo(const QString& host, quint16 port, const QString& nick, const QString& password)
{
	ConnectionInfo conn;
	conn.host = host;
	conn.port = port;
	conn.nick = nick;
	conn.pass = password;
	connectTo(conn);
}

void CWidgetIrcMain::connectTo(const ConnectionInfo& connection)
{
	CWizardIrcConnection wizard;
	wizard.setConnection(connection);

	if (!connection.host.isEmpty() && !connection.nick.isEmpty())
		connectToImpl(connection);
	else if (wizard.exec())
		connectToImpl(wizard.connection());
}

void CWidgetIrcMain::connectToImpl(const ConnectionInfo& info)
{
	Connection* connection = info.toConnection(this);
	connection->setEncoding(SingleApplication::encoding());
	int index = stackView->addConnection(connection);
	IgnoreManager::instance()->addConnection(connection);
	if (connection->isEnabled()) {
		connection->open();
		if (!treeWidget->hasRestoredCurrent())
			stackView->setCurrentIndex(index);
	}

	connect(SystemNotifier::instance(), SIGNAL(sleep()), connection, SLOT(quit()));
	connect(SystemNotifier::instance(), SIGNAL(wake()), connection, SLOT(open()));

	connect(SystemNotifier::instance(), SIGNAL(online()), connection, SLOT(open()));
	connect(SystemNotifier::instance(), SIGNAL(offline()), connection, SLOT(quit()));

	connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(updateOverlay()));
	updateOverlay();

	IrcChannelStackView* stack = stackView->connectionWidget(connection);
	connect(stack, SIGNAL(viewAdded(MessageView*)), this, SLOT(viewAdded(MessageView*)));
	connect(stack, SIGNAL(viewRemoved(MessageView*)), treeWidget, SLOT(removeView(MessageView*)));
	connect(stack, SIGNAL(viewRenamed(MessageView*)), treeWidget, SLOT(renameView(MessageView*)));
	connect(stack, SIGNAL(viewActivated(MessageView*)), this, SLOT(viewActivated(MessageView*)));

	if (MessageView* view = stack->viewAt(0)) {
		treeWidget->addView(view);
		if (!treeWidget->hasRestoredCurrent() && (connection->status() != IrcConnection::Closed || stackView->count() == 1))
			treeWidget->setCurrentView(view);
		treeWidget->parentWidget()->show();
	}

	bool expand = false;
	foreach (const ViewInfo& view, info.views) {
		if (view.type != -1 && view.expanded) {
			expand = true;
			break;
		}
	}
	if (expand || info.views.isEmpty())
		treeWidget->expandItem(treeWidget->connectionItem(connection));
}

void CWidgetIrcMain::initialize()
{
	quazaaSettings.loadChatConnections();
	ConnectionInfos connections = quazaaSettings.Chat.Connections.value<ConnectionInfos>();

	foreach(const ConnectionInfo & connection, connections)
		connectToImpl(connection);

	if (connections.isEmpty()) {
		connectTo(ConnectionInfo());
	}
}

void CWidgetIrcMain::editConnection(IrcConnection* connection)
{
	CWizardIrcConnection wizard;
	if (Connection* c = qobject_cast<Connection*>(connection)) { // TODO
		wizard.setConnection(ConnectionInfo::fromConnection(c));
		if (wizard.exec())
			wizard.connection().initConnection(c);
		updateOverlay();
	}
}

void CWidgetIrcMain::applySettings()
{
	bool dark = quazaaSettings.Chat.DarkTheme;
	QFile file(QString(":/Resource/%1.css").arg(dark ? "dark" : "light"));
	if (file.open(QFile::ReadOnly | QIODevice::Text))
		setStyleSheet(QString::fromUtf8(file.readAll()));

	searchShortcut->setKey(QKeySequence(quazaaSettings.Chat.Shortcuts.value(IrcShortcutType::SearchView)));
	if (muteAction)
		muteAction->setChecked(quazaaSettings.Chat.MuteNotifications);
	if (overlay && overlay->isVisible())
		updateOverlay();
	IgnoreManager::instance()->setIgnores(quazaaSettings.Chat.Ignores);
}

void CWidgetIrcMain::highlighted(IrcMessage* message)
{
	Q_UNUSED(message);
	if (!isActiveWindow()) {
		QApplication::alert(this);
		if (dockTile)
			dockTile->setBadge(dockTile->badge() + 1);
		if (sound && (!muteAction || !muteAction->isChecked()))
			sound->play();
	}

	MessageView* view = qobject_cast<MessageView*>(sender());
	if (view) {
		SessionTreeItem* item = treeWidget->connectionItem(view->connection());
		if (view->viewType() != ViewInfo::Server)
			item = item->findChild(view->receiver());
		if (item) {
			treeWidget->highlight(item);
			item->setBadge(item->badge() + 1);
		}
	}
}

void CWidgetIrcMain::missed(IrcMessage* message)
{
	Q_UNUSED(message);
	MessageView* view = qobject_cast<MessageView*>(sender());
	if (view) {
		SessionTreeItem* item = treeWidget->connectionItem(view->connection());
		if (view->viewType() != ViewInfo::Server)
			item = item->findChild(view->receiver());
		if (item)
			item->setBadge(item->badge() + 1);
	}
}

void CWidgetIrcMain::viewAdded(MessageView* view)
{
	connect(view, SIGNAL(splitterChanged(QByteArray)), this, SLOT(splitterChanged(QByteArray)));
	connect(view, SIGNAL(highlighted(IrcMessage*)), this, SLOT(highlighted(IrcMessage*)));
	connect(view, SIGNAL(missed(IrcMessage*)), this, SLOT(missed(IrcMessage*)));

	if (!quazaaSettings.WinMain.ChatUserListSplitter.isEmpty())
		view->restoreSplitter(quazaaSettings.WinMain.ChatUserListSplitter);

	treeWidget->addView(view);
	if (!quazaaSettings.WinMain.ChatTreeWidget.isEmpty())
		treeWidget->restoreState(quazaaSettings.WinMain.ChatTreeWidget);

	update();
}

void CWidgetIrcMain::viewActivated(MessageView* view)
{
	if (!quazaaSettings.WinMain.ChatUserListSplitter.isEmpty())
		view->restoreSplitter(quazaaSettings.WinMain.ChatUserListSplitter);

	treeWidget->setCurrentView(view);
}

void CWidgetIrcMain::closeTreeItem(SessionTreeItem* item)
{
	IrcChannelStackView* stack = stackView->connectionWidget(item->connection());
	if (stack) {
		int index = stack->indexOf(item->view());
		stack->closeView(index);
		if (index == 0) {
			stackView->removeConnection(stack->connection());
			IgnoreManager::instance()->removeConnection(stack->connection());
			treeWidget->parentWidget()->setVisible(!stackView->connections().isEmpty());
			if (stackView->count() == 0)
				createHome();
		}
	}
}

void CWidgetIrcMain::currentTreeItemChanged(IrcConnection* connection, const QString& view)
{
	IrcChannelStackView* stack = stackView->connectionWidget(connection);
	if (stack) {
		stackView->setCurrentWidget(stack);
		if (view.isEmpty())
			stack->setCurrentIndex(0);
		else
			stack->openView(view);
	}
	setWindowFilePath(view);
	updateOverlay();
}

void CWidgetIrcMain::splitterChanged(const QByteArray& state)
{
	quazaaSettings.WinMain.ChatUserListSplitter = state;
}

void CWidgetIrcMain::updateOverlay()
{
	IrcChannelStackView* stack = stackView->currentWidget();
	if (stack && stack->connection()) {
		if (!overlay) {
			overlay = new Overlay(stackView);
			connect(overlay, SIGNAL(refresh()), this, SLOT(reconnect()));
		}
		IrcConnection* connection = stack->connection();
		overlay->setParent(stack->currentWidget());
		overlay->setBusy(connection->isActive() && !connection->isConnected());
		overlay->setRefresh(!connection->isActive());
		overlay->setVisible(!connection->isConnected());
		overlay->setDark(quazaaSettings.Chat.DarkTheme);
		if (!connection->isConnected())
			overlay->setFocus();
	}
}

void CWidgetIrcMain::reconnect()
{
	IrcChannelStackView* stack = stackView->currentWidget();
	if (stack && stack->connection())
		stack->connection()->open();
}

void CWidgetIrcMain::addView()
{
	IrcChannelStackView* stack = stackView->currentWidget();
	if (stack && stack->connection()->isActive()) {
		AddViewDialog dialog(stack->connection(), this);
		if (dialog.exec()) {
			QString view = dialog.view();
			if (!dialog.isChannel())
				view = view.prepend("#");
			if (!dialog.isChannel(view)) {
				view = view.remove(0,1);
				view = view.prepend(stack->connection()->network()->channelTypes().at(0));
			}
			if (dialog.isChannel(view)) {
				stack->connection()->sendCommand(IrcCommand::createJoin(view, dialog.password()));
				stack->openView(view);
			}
		}
	}
}

void CWidgetIrcMain::closeView()
{
	IrcChannelStackView* stack = stackView->currentWidget();
	if (stack) {
		int index = stack->currentIndex();
		stack->closeView(index);
		if (index == 0) {
			stackView->removeConnection(stack->connection());
			IgnoreManager::instance()->removeConnection(stack->connection());
			treeWidget->parentWidget()->setVisible(!stackView->connections().isEmpty());
			if (stackView->count() == 0)
				createHome();
		}
	}
}

void CWidgetIrcMain::searchView()
{
	SearchPopup* search = new SearchPopup(this);
	connect(search, SIGNAL(searched(QString)), treeWidget, SLOT(search(QString)));
	connect(search, SIGNAL(searchedAgain(QString)), treeWidget, SLOT(searchAgain(QString)));
	connect(search, SIGNAL(destroyed()), treeWidget, SLOT(unblockItemReset()));
	connect(treeWidget, SIGNAL(searched(bool)), search, SLOT(onSearched(bool)));
	treeWidget->blockItemReset();
	search->popup();
}

void CWidgetIrcMain::showSettings()
{
	CDialogIrcSettings *dlgIrcSettings = new CDialogIrcSettings(this);

	dlgIrcSettings->exec();
}

void CWidgetIrcMain::createTree()
{
	QSplitter* splitter = static_cast<QSplitter*>(centralWidget());
	QWidget* container = new QWidget(this);
	container->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	container->setVisible(false);

	treeWidget = new SessionTreeWidget(container);
	treeWidget->setFocusPolicy(Qt::NoFocus);

	connect(treeWidget, SIGNAL(editConnection(IrcConnection*)), this, SLOT(editConnection(IrcConnection*)));
	connect(treeWidget, SIGNAL(closeItem(SessionTreeItem*)), this, SLOT(closeTreeItem(SessionTreeItem*)));
	connect(treeWidget, SIGNAL(currentViewChanged(IrcConnection*, QString)), this, SLOT(currentTreeItemChanged(IrcConnection*, QString)));

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

void CWidgetIrcMain::createHome()
{
	homePage = new HomePage(stackView);
	connect(homePage, SIGNAL(connectRequested()), this, SLOT(initialize()));
	stackView->insertWidget(0, homePage);
}
