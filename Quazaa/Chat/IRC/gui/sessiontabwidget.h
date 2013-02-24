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

#ifndef SESSIONTABWIDGET_H
#define SESSIONTABWIDGET_H

#include "tabwidget.h"
#include "messagehandler.h"
#include <QHash>

class Session;
class IrcMessage;
class WidgetIrcMessageView;
class MenuFactory;

class SessionTabWidget : public TabWidget
{
    Q_OBJECT

public:
    SessionTabWidget(Session* session, QWidget* parent = 0);

    Session* session() const;

    WidgetIrcMessageView* currentView() const;
    WidgetIrcMessageView* viewAt(int index) const;

    MenuFactory* menuFactory() const;
    void setMenuFactory(MenuFactory* factory);

    QByteArray saveSplitter() const;

public slots:
    void restoreSplitter(const QByteArray& state);
    void switchToServerTab();
    WidgetIrcMessageView* openView(const QString& receiver);
	void removeView(const QString& receiver);
	void closeCurrentView();
	void closeView(int index);
	void renameView(const QString& from, const QString& to);
    void sendMessage(const QString& receiver, const QString& message);
	void applySettings();

signals:
    void inactiveStatusChanged(bool inactive);
    void sessionClosed(Session* session);
    void splitterChanged(const QByteArray& state);
    void editSession(Session* session);

    void viewAdded(WidgetIrcMessageView* view);
    void viewRemoved(WidgetIrcMessageView* view);
    void viewRenamed(WidgetIrcMessageView* view);
    void viewActivated(WidgetIrcMessageView* view);

protected:
    bool event(QEvent* event);

private slots:
    void updateStatus();
    void resetTab(int index);
    void tabActivated(int index);
    void onNewTabRequested();
    void onTabMenuRequested(int index, const QPoint& pos);
    void delayedTabReset();
    void delayedTabResetTimeout();
    void onEditSession();

private:
    struct SessionTabWidgetData {
        QList<int> delayedIndexes;
        MessageHandler handler;
        QHash<QString, WidgetIrcMessageView*> views;
        MenuFactory* menuFactory;
    } d;
};

#endif // SESSIONTABWIDGET_H
