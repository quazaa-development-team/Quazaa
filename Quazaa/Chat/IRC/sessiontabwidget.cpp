/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#include "sessiontabwidget.h"
#include "messageview.h"
#include "quazaasettings.h"
#include "quazaaglobals.h"
#include "session.h"
#include <irccommand.h>
#include <QtGui>

SessionTabWidget::SessionTabWidget(Session* session, QWidget* parent) :
    TabWidget(parent)
{
    d.hasQuit = false;
    d.handler.setSession(session);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(this, SIGNAL(newTabRequested()), this, SLOT(onNewTabRequested()));
    connect(session, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateStatus()));
	connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateStatus()));
	connect(session, SIGNAL(networkChanged(QString)), this, SIGNAL(titleChanged(QString)));

    connect(&d.handler, SIGNAL(receiverToBeAdded(QString)), this, SLOT(openView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRemoved(QString)), this, SLOT(removeView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRenamed(QString,QString)), this, SLOT(renameView(QString,QString)));

    QShortcut* shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeCurrentView()));
	connect(&quazaaSettings, SIGNAL(chatSettingsChanged()), this, SLOT(applySettings()));

	applySettings();

    MessageView* view = openView(d.handler.session()->host());
    d.handler.setDefaultReceiver(view);
	view->setStatusChannel(true);
	updateStatus();
}

Session* SessionTabWidget::session() const
{
    return qobject_cast<Session*>(d.handler.session());
}

QStringList SessionTabWidget::channels() const
{
    QStringList chans;
    foreach (MessageView* view, d.views)
    {
        if (view->isChannelView())
            chans += view->receiver();
    }
    return chans;
}

MessageView* SessionTabWidget::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (!view)
    {
        view = new MessageView(d.handler.session(), this);
        view->setReceiver(receiver);
        connect(view, SIGNAL(alert(MessageView*, bool)), this, SLOT(alertTab(MessageView*, bool)));
        connect(view, SIGNAL(highlight(MessageView*, bool)), this, SLOT(highlightTab(MessageView*, bool)));
		connect(view, SIGNAL(query(QString)), this, SLOT(openView(QString)));
		connect(view, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));
		connect(view, SIGNAL(closeQuery(MessageView*)), this, SLOT(closeView(MessageView*)));

        d.handler.addReceiver(receiver, view);
        d.views.insert(receiver.toLower(), view);
		int index = addTab(view, receiver);
		tabBar()->setTabButton(index, QTabBar::RightSide, view->closeButton);
		if(index == 0)
		{
			setTabIcon(index, QIcon(":/Resource/Network/Network.png"));
		} else if(view->isChannelView()) {
			setTabIcon(index, QIcon(":/Resource/Chat/Rooms.png"));
		} else {
			setTabIcon(index, QIcon(":/Resource/Chat/Chat.png"));
		}
    }
    if (view)
        setCurrentWidget(view);
    return view;
}

void SessionTabWidget::removeView(const QString& receiver)
{
    MessageView* view = d.views.take(receiver.toLower());
    if (view)
    {
		if (view)
		{
			if (indexOf(view) == 0)
				deleteLater();
			else
				view->deleteLater();
		}
    }
}

void SessionTabWidget::closeCurrentView()
{
	closeView(currentIndex());
}

void SessionTabWidget::closeView(MessageView* view)
{
	if (view)
	{
		QString reason = tr("%1 %2").arg(QApplication::applicationName())
									.arg(QuazaaGlobals::APPLICATION_VERSION_STRING());
		if (indexOf(view) == 0)
			session()->quit(reason);
		else if (view->isChannelView())
			d.handler.session()->sendCommand(IrcCommand::createPart(view->receiver(), reason));

		d.handler.removeReceiver(view->receiver());
	}
}

void SessionTabWidget::closeView(int index)
{
	MessageView* view = d.views.value(tabText(index).toLower());

	closeView(view);
}

void SessionTabWidget::renameView(const QString& from, const QString& to)
{
    MessageView* view = d.views.take(from.toLower());
    if (view)
    {
        view->setReceiver(to);
        d.views.insert(to.toLower(), view);
        int index = indexOf(view);
        if (index != -1)
            setTabText(index, view->receiver());
        if (index == 0)
            emit titleChanged(view->receiver());
    }
}

void SessionTabWidget::quit(const QString &message)
{
    QString reason = message.trimmed();
    if (reason.isEmpty())
		reason = tr("%1 %2").arg(QApplication::applicationName())
							.arg(QuazaaGlobals::APPLICATION_VERSION_STRING());
    d.handler.session()->sendCommand(IrcCommand::createQuit(reason));
}

void SessionTabWidget::updateStatus()
{
	bool inactive = !session()->isActive() && !session()->isConnected();
	setTabInactive(0, inactive);
	emit inactiveStatusChanged(inactive);
}

void SessionTabWidget::onAboutToQuit()
{
    d.hasQuit = true;
}

void SessionTabWidget::onDisconnected()
{
    if (d.hasQuit)
        deleteLater();
    // TODO: else reconnect?
}

void SessionTabWidget::tabActivated(int index)
{
    if (index < count() - 1)
    {
        d.handler.setCurrentReceiver(currentWidget());
        setTabAlert(index, false);
        setTabHighlight(index, false);
        if (isVisible())
        {
            window()->setWindowFilePath(tabText(index));
            if (currentWidget())
                currentWidget()->setFocus();
        }
    }
}

void SessionTabWidget::onNewTabRequested()
{
    QString channel = QInputDialog::getText(this, tr("Join channel"), tr("Channel:"));
    if (!channel.isEmpty())
    {
        if (channel.startsWith("#") || channel.startsWith("&"))
            d.handler.session()->sendCommand(IrcCommand::createJoin(channel));
        openView(channel);
    }
}

void SessionTabWidget::onTabMenuRequested(int index, const QPoint& pos)
{
	QMenu menu;
	if (index == 0)
	{
		if (session()->isActive())
			menu.addAction(tr("Disconnect"), session(), SLOT(quit()));
		else
			menu.addAction(tr("Reconnect"), session(), SLOT(reconnect()));
	}
	if (static_cast<MessageView*>(widget(index))->isChannelView())
		menu.addAction(tr("Part"), this, SLOT(onTabCloseRequested()))->setData(index);
	else
		menu.addAction(tr("Close"), this, SLOT(onTabCloseRequested()))->setData(index);
	menu.exec(pos);
}

void SessionTabWidget::onTabCloseRequested()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
		closeView(action->data().toInt());
}

void SessionTabWidget::delayedTabReset()
{
    d.delayedIndexes += currentIndex();
    QTimer::singleShot(500, this, SLOT(delayedTabResetTimeout()));
}

void SessionTabWidget::delayedTabResetTimeout()
{
    if (d.delayedIndexes.isEmpty())
        return;

    int index = d.delayedIndexes.takeFirst();
    tabActivated(index);
}

void SessionTabWidget::alertTab(MessageView* view, bool on)
{
    int index = indexOf(view);
    if (index != -1)
    {
        if (!isVisible() || !isActiveWindow() || index != currentIndex())
			setTabAlert(index, on);
    }
}

void SessionTabWidget::highlightTab(MessageView* view, bool on)
{
    int index = indexOf(view);
    if (index != -1)
    {
        if (!isVisible() || !isActiveWindow() || index != currentIndex())
            setTabHighlight(index, on);
    }
}

void SessionTabWidget::applySettings()
{
	setAlertColor(QColor(quazaaSettings.Chat.Colors.value(IRCColorType::Highlight)));
	setHighlightColor(QColor(quazaaSettings.Chat.Colors.value(IRCColorType::Highlight)));
}
