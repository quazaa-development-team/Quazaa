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

#ifndef SESSIONTABWIDGET_H
#define SESSIONTABWIDGET_H

#include "tabwidget.h"
#include "messagehandler.h"
#include <QHash>

class Session;
class MessageView;
struct Connection;
class IrcMessage;

class SessionTabWidget : public TabWidget
{
    Q_OBJECT

public:
    SessionTabWidget(Session* session, QWidget* parent = 0);

    Session* session() const;
    QStringList channels() const;

public slots:
	MessageView* openView(const QString& receiver);
    void removeView(const QString& receiver);
    void closeCurrentView();
	void closeView(MessageView *view);
	void closeView(int index);
    void renameView(const QString& from, const QString& to);
	void messageToView(const QString &receiver, const QString &message);
    void quit(const QString& message = QString());

signals:
    void titleChanged(const QString& title);
	void inactiveStatusChanged(bool inactive);
	void sessionHighlighted(SessionTabWidget* session, bool on);
	void sessionAlerted(SessionTabWidget* session, bool on);


private slots:
	void updateStatus();
    void onAboutToQuit();
    void onDisconnected();
    void tabActivated(int index);
	void onNewTabRequested(QString channel = "");
	void onTabMenuRequested(int index, const QPoint& pos);
	void onTabCloseRequested();
	void delayedTabReset();
    void delayedTabResetTimeout();
    void alertTab(MessageView* view, bool on);
    void highlightTab(MessageView* view, bool on);
	void applySettings();

private:
    struct SessionTabWidgetData
    {
        bool hasQuit;
        QList<int> delayedIndexes;
        MessageHandler handler;
        QHash<QString, MessageView*> views;
    } d;
};

#endif // SESSIONTABWIDGET_H
