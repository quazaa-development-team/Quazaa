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

#include "listviewircusers.h"
#include "menufactory.h"
#include "ircuserlistmodel.h"
#include "session.h"
#include <QItemSelectionModel>
#include <QContextMenuEvent>
#include <QScrollBar>
#include <QAction>

ListViewIrcUsers::ListViewIrcUsers(QWidget* parent) : QListView(parent)
{
	d.menuFactory = 0;
	d.userModel = new IrcUserListModel();
	connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked(QModelIndex)));
}

ListViewIrcUsers::~ListViewIrcUsers()
{
}

QSize ListViewIrcUsers::sizeHint() const
{
	return QSize(16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width(), QListView::sizeHint().height());
}

Session* ListViewIrcUsers::session() const
{
	return d.userModel->session();
}

void ListViewIrcUsers::setSession(Session* session)
{
    d.userModel->setSession(session);
}

QString ListViewIrcUsers::channel() const
{
	return d.userModel->channel();
}

void ListViewIrcUsers::setChannel(const QString& channel)
{
	d.userModel->setChannel(channel);
}

IrcUserListModel *ListViewIrcUsers::userModel() const
{
	return d.userModel;
}

bool ListViewIrcUsers::hasUser(const QString& user) const
{
	return d.userModel->hasUser(user);
}

MenuFactory* ListViewIrcUsers::menuFactory() const
{
	if (!d.menuFactory) {
        ListViewIrcUsers* that = const_cast<ListViewIrcUsers*>(this);
		that->d.menuFactory = new MenuFactory(that);
	}
	return d.menuFactory;
}

void ListViewIrcUsers::setMenuFactory(MenuFactory* factory)
{
	if (d.menuFactory && d.menuFactory->parent() == this)
		delete d.menuFactory;
	d.menuFactory = factory;
}

void ListViewIrcUsers::processMessage(IrcMessage* message)
{
	d.userModel->processMessage(message);
}

void ListViewIrcUsers::contextMenuEvent(QContextMenuEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	if (index.isValid()) {
		QMenu* menu = menuFactory()->createUserListMenu(index.data().toString(), this);
		menu->exec(event->globalPos());
		menu->deleteLater();
	}
}

void ListViewIrcUsers::mousePressEvent(QMouseEvent* event)
{
	QListView::mousePressEvent(event);
	if (!indexAt(event->pos()).isValid())
        selectionModel()->clear();
}

void ListViewIrcUsers::showEvent(QShowEvent *event)
{
    QListView::showEvent(event);
    if (!model()) {
        setModel(d.userModel);
    }
}

void ListViewIrcUsers::onDoubleClicked(const QModelIndex& index)
{
	if (index.isValid())
		emit doubleClicked(index.data(Qt::DisplayRole).toString());
}
