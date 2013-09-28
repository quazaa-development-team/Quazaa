/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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
#include "addviewdialog.h"
#include "tabwidget_p.h"
#include "widgetircmessageview.h"
#include "menufactory.h"
#include "quazaasettings.h"
#include "session.h"
#include <irccommand.h>
#include <QShortcut>

SessionTabWidget::SessionTabWidget(Session* session, QWidget* parent) :
    CTabWidget(parent)
{
    d.menuFactory = 0;
    d.handler.setSession(session);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(this, SIGNAL(newTabRequested()), this, SLOT(onNewTabRequested()), Qt::QueuedConnection);
    connect(this, SIGNAL(tabMenuRequested(int, QPoint)), this, SLOT(onTabMenuRequested(int, QPoint)));

    connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateStatus()));
    connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateStatus()));

    connect(&d.handler, SIGNAL(receiverToBeAdded(QString)), this, SLOT(addView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRemoved(QString)), this, SLOT(removeView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRenamed(QString, QString)), this, SLOT(renameView(QString, QString)));

    QShortcut* shortcut = new QShortcut(QKeySequence::AddTab, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onNewTabRequested()));

    shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeCurrentView()));

    CWidgetIrcMessageView* view = addView(d.handler.session()->host());
    session->setDefaultView(view);
    d.handler.setDefaultReceiver(view);
    d.handler.setCurrentReceiver(view);
    updateStatus();

	applySettings();
}

Session* SessionTabWidget::session() const
{
    return qobject_cast<Session*>(d.handler.session());
}

CWidgetIrcMessageView *SessionTabWidget::currentView() const
{
    return qobject_cast<CWidgetIrcMessageView*>(currentWidget());
}

CWidgetIrcMessageView *SessionTabWidget::viewAt(int index) const
{
    return qobject_cast<CWidgetIrcMessageView*>(widget(index));
}

MenuFactory* SessionTabWidget::menuFactory() const
{
    if (!d.menuFactory) {
        SessionTabWidget* that = const_cast<SessionTabWidget*>(this);
        that->d.menuFactory = new MenuFactory(that);
    }
    return d.menuFactory;
}

void SessionTabWidget::setMenuFactory(MenuFactory* factory)
{
    if (d.menuFactory && d.menuFactory->parent() == this)
        delete d.menuFactory;
    d.menuFactory = factory;
}



QByteArray SessionTabWidget::saveSplitter() const
{
    foreach (CWidgetIrcMessageView* view, d.views) {
        if (view->viewType() != CWidgetIrcMessageView::ServerView)
            return view->saveSplitter();
    }
    return QByteArray();
}

void SessionTabWidget::restoreSplitter(const QByteArray& state)
{
    foreach (CWidgetIrcMessageView* view, d.views) {
        view->blockSignals(true);
        view->restoreSplitter(state);
        view->blockSignals(false);
    }
    emit splitterChanged(state);
}

CWidgetIrcMessageView* SessionTabWidget::addView(const QString& receiver)
{
    CWidgetIrcMessageView* view = d.views.value(receiver.toLower());
	if (!view) {
        CWidgetIrcMessageView::ViewType type = CWidgetIrcMessageView::ServerView;
		if (!d.views.isEmpty())
            type = session()->isChannel(receiver) ? CWidgetIrcMessageView::ChannelView : CWidgetIrcMessageView::QueryView;
        view = new CWidgetIrcMessageView(type, d.handler.session(), this);
        view->applySettings();
		view->setReceiver(receiver);
        connect(view, SIGNAL(queried(QString)), this, SLOT(addView(QString)));
        connect(view, SIGNAL(queried(QString)), this, SLOT(openView(QString)));
        connect(view, SIGNAL(messaged(QString,QString)), this, SLOT(sendMessage(QString,QString)));
		connect(view, SIGNAL(splitterChanged(QByteArray)), this, SLOT(restoreSplitter(QByteArray)));
        connect(view, SIGNAL(openView(QString)), this, SLOT(openView(QString)));
        connect(view, SIGNAL(closeView(QString)), this, SLOT(removeView(QString)));

		d.handler.addReceiver(receiver, view);
		d.views.insert(receiver.toLower(), view);
        int index = addTab(view, QString(receiver).replace("&", "&&"));
        tabBar()->setTabButton(index, QTabBar::RightSide, view->closeButton);
        if(view->viewType() == CWidgetIrcMessageView::ServerView)
        {
            setTabIcon(index, QIcon(":/Resource/Network/Network.png"));
        } else if (view->viewType() == CWidgetIrcMessageView::ChannelView) {
            setTabIcon(index, QIcon(":/Resource/Chat/Friends.png"));
            connect(view, SIGNAL(appendRawMessage(QString)), view->session()->defaultView()->textBrowser(), SLOT(append(QString)));
            connect(view, SIGNAL(appendRawMessage(QString)), this, SLOT(switchToServerTab()));
        } else {
            setTabIcon(index, QIcon(":/Resource/Chat/Chat.png"));
            connect(view, SIGNAL(appendRawMessage(QString)), view->session()->defaultView()->textBrowser(), SLOT(append(QString)));
            connect(view, SIGNAL(appendRawMessage(QString)), this, SLOT(switchToServerTab()));
        }
		emit viewAdded(view);

        if (d.handler.session()->isChannel(receiver))
            openView(receiver);
    }
    return view;
}

void SessionTabWidget::openView(const QString &reciever)
{
    CWidgetIrcMessageView* view = d.views.value(reciever.toLower());
    if (view)
        setCurrentWidget(view);
}

void SessionTabWidget::removeView(const QString& receiver)
{
    CWidgetIrcMessageView* view = d.views.take(receiver.toLower());
    if (view) {
        view->deleteLater();
        emit viewRemoved(view);
        if (indexOf(view) == 0) {
            deleteLater();
            session()->destructLater();
            emit sessionClosed(session());
        }

        d.handler.removeReceiver(view->receiver());
    }
}

void SessionTabWidget::closeCurrentView()
{
    closeView(currentIndex());
}

void SessionTabWidget::closeView(int index)
{
    CWidgetIrcMessageView* view = viewAt(index);
    if (view) {
        if (view->isActive()) {
            QString reason = tr("%1 %2").arg(QApplication::applicationName())
                             .arg(QApplication::applicationVersion());
            if (indexOf(view) == 0)
                session()->quit(reason);
            else if (view->viewType() == CWidgetIrcMessageView::ChannelView)
                d.handler.session()->sendCommand(IrcCommand::createPart(view->receiver(), reason));
        }
        d.handler.removeReceiver(view->receiver());
    }
}

void SessionTabWidget::renameView(const QString& from, const QString& to)
{
    CWidgetIrcMessageView* view = d.views.take(from.toLower());
    if (view) {
        view->setReceiver(to);
        d.views.insert(to.toLower(), view);
        int index = indexOf(view);
        if (index != -1)
            setTabText(index, view->receiver().replace("&", "&&"));
        emit viewRenamed(view);
    }
}

void SessionTabWidget::sendMessage(const QString &receiver, const QString &message)
{
    CWidgetIrcMessageView* view = addView(receiver);
    if (view) {
        setCurrentWidget(view);
        view->sendMessage(message);
    }
}

bool SessionTabWidget::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate)
        delayedTabReset();
    return CTabWidget::event(event);
}

void SessionTabWidget::updateStatus()
{
    bool inactive = !session()->isActive() && !session()->isConnected();
    setTabInactive(0, inactive);
    emit inactiveStatusChanged(inactive);
}

void SessionTabWidget::resetTab(int index)
{
    if (index < count() - 1) {
        CWidgetIrcMessageView* view = qobject_cast<CWidgetIrcMessageView*>(widget(index));
        if (view) {
            setTabAlert(index, false);
            setTabHighlight(index, false);
        }
    }
}

void SessionTabWidget::tabActivated(int index)
{
    if (index < count() - 1) {
        CWidgetIrcMessageView* view = viewAt(index);
        if (view) {
            resetTab(index);
            if (isVisible()) {
                d.handler.setCurrentReceiver(view);
                window()->setWindowFilePath(tabText(index).replace("&&", "&"));
                view->setFocus();
                emit viewActivated(view);
            }
        }
    }
}

void SessionTabWidget::onNewTabRequested()
{
    AddViewDialog dialog(session(), this);
    if (dialog.exec()) {
        QString view = dialog.view();
        if (!session()->isChannel(view))
            view = view.prepend("#");
        if (!session()->isChannel(view))
        {
            view = view.remove(0,1);
            view = view.prepend(session()->info().channelTypes().at(0));
        }
        if (session()->isChannel(view))
            d.handler.session()->sendCommand(IrcCommand::createJoin(view, dialog.password()));
        openView(view);
    }
}

void SessionTabWidget::onTabMenuRequested(int index, const QPoint& pos)
{
    CWidgetIrcMessageView* view = qobject_cast<CWidgetIrcMessageView*>(widget(index));
    if (view) {
        QMenu* menu = menuFactory()->createTabViewMenu(view, this);
        menu->exec(pos);
        menu->deleteLater();
    }
}

void SessionTabWidget::delayedTabReset()
{
    d.delayedIndexes += currentIndex();
    QTimer::singleShot(500, this, SLOT(delayedTabResetTimeout()));
}

void SessionTabWidget::delayedTabResetTimeout()
{
    if (!d.delayedIndexes.isEmpty()) {
        resetTab(d.delayedIndexes.takeLast());
        d.delayedIndexes.clear();
    }
}

void SessionTabWidget::onEditSession()
{
    emit editSession(session());
}

void SessionTabWidget::switchToServerTab()
{
    setCurrentIndex(0);
}

void SessionTabWidget::applySettings()
{
	CTabBar* tb = static_cast<CTabBar*>(tabBar());
	tb->setNavigationShortcut(CTabBar::Next, QKeySequence(quazaaSettings.Chat.Shortcuts.value(IrcShortcutType::NavigateRight)));
	tb->setNavigationShortcut(CTabBar::Previous, QKeySequence(quazaaSettings.Chat.Shortcuts.value(IrcShortcutType::NavigateLeft)));
	tb->setNavigationShortcut(CTabBar::NextUnread, QKeySequence(quazaaSettings.Chat.Shortcuts.value(IrcShortcutType::NextUnreadRight)));
	tb->setNavigationShortcut(CTabBar::PreviousUnread, QKeySequence(quazaaSettings.Chat.Shortcuts.value(IrcShortcutType::NextUnreadLeft)));
	tb->setVisible(quazaaSettings.Chat.Layout == "tabs");

	QColor color(quazaaSettings.Chat.Colors.value(IrcColorType::Highlight));
	setTabTextColor(Alert, color);
	setTabTextColor(Highlight, color);

	foreach(CWidgetIrcMessageView * view, d.views)
		view->applySettings();
}
