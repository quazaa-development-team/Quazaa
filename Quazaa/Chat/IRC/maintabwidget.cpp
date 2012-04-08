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

#include "maintabwidget.h"
#include "quazaasettings.h"
#include <QShortcut>
#include <QTabBar>
#include <QApplication>

MainTabWidget::MainTabWidget(QWidget* parent) : TabWidget(parent)
{
    setTabPosition(QTabWidget::West);
    setStyleSheet(".MainTabWidget::pane { border: 0px; }");

    d.tabUpShortcut = new QShortcut(this);
    connect(d.tabUpShortcut, SIGNAL(activated()), this, SLOT(moveToPrevTab()));

    d.tabDownShortcut = new QShortcut(this);
    connect(d.tabDownShortcut, SIGNAL(activated()), this, SLOT(moveToNextTab()));

    d.tabLeftShortcut = new QShortcut(this);
    connect(d.tabLeftShortcut, SIGNAL(activated()), this, SLOT(moveToPrevSubTab()));

    d.tabRightShortcut = new QShortcut(this);
    connect(d.tabRightShortcut, SIGNAL(activated()), this, SLOT(moveToNextSubTab()));

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
	connect(&quazaaSettings, SIGNAL(chatSettingsChanged()), this, SLOT(applySettings()));
	applySettings();
}


void MainTabWidget::applySettings()
{
	setAlertColor(QColor(quazaaSettings.Chat.Colors.value(IRCColorType::Highlight)));
	setHighlightColor(QColor(quazaaSettings.Chat.Colors.value(IRCColorType::Highlight)));
}

void MainTabWidget::setSessionTitle(const QString& title)
{
    int index = senderIndex();
    if (index != -1)
        setTabText(index, title);
}

void MainTabWidget::setInactive(bool inactive)
{
	int index = senderIndex();
	if (index != -1)
		setTabInactive(index, inactive);
}

void MainTabWidget::alertTab(SessionTabWidget* session, bool on)
{
	int index = indexOf(session);
	if (index != -1)
	{
		if(index != currentIndex())
			setTabHighlight(index, on);
	}
}

void MainTabWidget::highlightTab(SessionTabWidget* session, bool on)
{
	int index = indexOf(session);
	if (index != -1)
	{
		if(index != currentIndex())
			setTabHighlight(index, on);
	}
}

int MainTabWidget::senderIndex() const
{
    if (!sender() || !sender()->isWidgetType())
        return -1;

    return indexOf(static_cast<QWidget*>(sender()));
}

void MainTabWidget::moveToNextSubTab()
{
    TabWidget* tabWidget = qobject_cast<TabWidget*>(currentWidget());
    if (tabWidget)
        tabWidget->moveToNextTab();
}

void MainTabWidget::moveToPrevSubTab()
{
    TabWidget* tabWidget = qobject_cast<TabWidget*>(currentWidget());
    if (tabWidget)
        tabWidget->moveToPrevTab();
}

void MainTabWidget::tabActivated(int index)
{
	if (index < count() - 1)
	{
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
