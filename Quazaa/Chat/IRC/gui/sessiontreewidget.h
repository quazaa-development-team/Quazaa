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

#ifndef SESSIONTREEWIDGET_H
#define SESSIONTREEWIDGET_H

#include <QTreeWidget>
#include <QShortcut>
#include <QColor>
#include <QHash>

class Session;
class WidgetIrcMessageView;
class MenuFactory;
class SessionTreeItem;

class SessionTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    SessionTreeWidget(QWidget* parent = 0);

    QSize sizeHint() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

    MenuFactory* menuFactory() const;
    void setMenuFactory(MenuFactory* factory);

    enum ItemStatus { Active, Inactive, Alert, Highlight };

    QColor statusColor(ItemStatus status) const;
    void setStatusColor(ItemStatus status, const QColor& color);

    QColor currentAlertColor() const;

    SessionTreeItem* viewItem(WidgetIrcMessageView* view) const;
    SessionTreeItem* sessionItem(Session* session) const;

public slots:
    void addView(WidgetIrcMessageView* view);
    void removeView(WidgetIrcMessageView* view);
    void renameView(WidgetIrcMessageView* view);
    void setCurrentView(WidgetIrcMessageView* view);

    void moveToNextItem();
    void moveToPrevItem();

    void moveToNextUnreadItem();
    void moveToPrevUnreadItem();

    void expandCurrentSession();
    void collapseCurrentSession();

    void alert(SessionTreeItem* item);
    void unalert(SessionTreeItem* item);

	void applySettings();

signals:
    void editSession(Session* session);
    void closeItem(SessionTreeItem* item);
    void currentViewChanged(Session* session, const QString& view);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    bool event(QEvent* event);
    QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;

private slots:
    void updateView(WidgetIrcMessageView* view = 0);
    void updateSession(Session* session = 0);
    void onItemSelectionChanged();
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onItemClicked(QTreeWidgetItem* item, int column);
    void delayedItemReset();
    void delayedItemResetTimeout();
    void alertTimeout();

private:
    void resetItem(SessionTreeItem* item);
    QTreeWidgetItem* lastItem() const;
    QTreeWidgetItem* nextItem(QTreeWidgetItem* from) const;
    QTreeWidgetItem* previousItem(QTreeWidgetItem* from) const;

    struct SessionTreeWidgetData {
        QColor alertColor;
        MenuFactory* menuFactory;
        QShortcut* prevShortcut;
        QShortcut* nextShortcut;
        QShortcut* prevUnreadShortcut;
        QShortcut* nextUnreadShortcut;
        QShortcut* expandShortcut;
        QShortcut* collapseShortcut;
        QHash<ItemStatus, QColor> colors;
        QSet<SessionTreeItem*> alertedItems;
        QSet<SessionTreeItem*> resetedItems;
        QHash<WidgetIrcMessageView*, SessionTreeItem*> viewItems;
        QHash<Session*, SessionTreeItem*> sessionItems;
        QVariantHash state;
        mutable QTreeWidgetItem* dropParent;
    } d;
    friend class SessionTreeItem;
};

#endif // SESSIONTREEWIDGET_H
