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

#ifndef WIDGETIRCMAIN_H
#define WIDGETIRCMAIN_H

#include <QWidget>
#include <QSplitter>

class MultiSessionTabWidget;
class SessionTreeWidget;
class SessionTreeItem;
struct ConnectionInfo;
class WidgetIrcMessageView;
class IrcMessage;
class QtDockTile;
class Session;

class WidgetIrcMain : public QWidget
{
	Q_OBJECT

public:
    explicit WidgetIrcMain(QWidget* parent = 0);
    ~WidgetIrcMain();

	QSize sizeHint() const;
	void saveWidget();

public slots:
	void connectTo(const QString& host = QString(), quint16 port = 6667,
				   const QString& nick = QString(), const QString& password = QString());
	void connectTo(const ConnectionInfo& connection);
	void connectToImpl(const ConnectionInfo& connection);

protected:
	void closeEvent(QCloseEvent* event);
	void changeEvent(QEvent* event);

private slots:
	void initialize();
	void editSession(Session* session);
	void applySettings();
	void showSettings();
    void highlighted(IrcMessage* message);
    void missed(IrcMessage* message);
    void viewAdded(WidgetIrcMessageView* view);
    void viewRemoved(WidgetIrcMessageView* view);
    void viewRenamed(WidgetIrcMessageView* view);
    void viewActivated(WidgetIrcMessageView* view);
	void closeTreeItem(SessionTreeItem* item);
	void currentTreeItemChanged(Session* session, const QString& view);
	void splitterChanged(const QByteArray& state);
	void sessionAdded(Session* session);
	void sessionRemoved(Session* session);
	void updateSession(Session* session = 0);
	void addView();

private:
	void createTree();

    QSplitter* splitterIrcMain;
	SessionTreeWidget* treeWidget;
    MultiSessionTabWidget* tabWidget;
	QtDockTile* dockTile;
};

#endif // WIDGETIRCMAIN_H
