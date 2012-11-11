/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public 
** License version 3.0 requirements will be met: 
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version 
** 3.0 along with Quazaa; if not, write to the Free Software Foundation, 
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "widgetircsidebars.h"
#include "ui_widgetircsidebars.h"
#include "sessiontabwidget.h"
#include "messageview.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#include "systemlog.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetIrcSidebars::WidgetIrcSidebars(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::WidgetIrcSidebars)
{
	ui->setupUi(this);
	ui->frameChatLeftSidebar->hide();
	ui->frameChatRightSidebar->hide();
	panelIrcMain = new WidgetIrcMain();
	connect(panelIrcMain, SIGNAL(showFriends()), ui->frameChatLeftSidebar, SLOT(show()));
	connect(panelIrcMain, SIGNAL(msgTabChanged(SessionTabWidget*)), this, SLOT(onMsgTabChanged(SessionTabWidget*)));
	connect(panelIrcMain, SIGNAL(disconnected()), ui->frameChatLeftSidebar, SLOT(hide()));
	connect(panelIrcMain, SIGNAL(disconnected()), ui->frameChatRightSidebar, SLOT(hide()));
	ui->splitterChat->restoreState(quazaaSettings.WinMain.ChatSplitter);
	ui->verticalLayoutChatMiddle->addWidget(panelIrcMain);
	setSkin();
}

WidgetIrcSidebars::~WidgetIrcSidebars()
{
	panelIrcMain->close();
	delete ui;
}

void WidgetIrcSidebars::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WidgetIrcSidebars::saveWidget()
{
	quazaaSettings.WinMain.ChatSplitter = ui->splitterChat->saveState();
	panelIrcMain->saveWidget();
}

void WidgetIrcSidebars::on_splitterChat_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterChat->handle(1)->underMouse())
	{
		if(ui->splitterChat->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.ChatSplitterRestoreLeft = ui->splitterChat->sizes()[0];
			quazaaSettings.WinMain.ChatSplitterRestoreMiddle = ui->splitterChat->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterChat->sizes()[0] + ui->splitterChat->sizes()[1]);
			newSizes.append(ui->splitterChat->sizes()[2]);
			ui->splitterChat->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreMiddle);
			sizesList.append(ui->splitterChat->sizes()[2]);
			ui->splitterChat->setSizes(sizesList);
		}
	}

	if(ui->splitterChat->handle(2)->underMouse())
	{
		if(ui->splitterChat->sizes()[2] > 0)
		{
			quazaaSettings.WinMain.ChatSplitterRestoreMiddle = ui->splitterChat->sizes()[1];
			quazaaSettings.WinMain.ChatSplitterRestoreRight = ui->splitterChat->sizes()[2];
			QList<int> newSizes;
			newSizes.append(ui->splitterChat->sizes()[0]);
			newSizes.append(ui->splitterChat->sizes()[1] + ui->splitterChat->sizes()[2]);
			newSizes.append(0);
			ui->splitterChat->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(ui->splitterChat->sizes()[0]);
			sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreMiddle);
			sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreRight);
			ui->splitterChat->setSizes(sizesList);
		}
	}
}

void WidgetIrcSidebars::on_toolButtonChatFriendsHeader_clicked()
{
	if(ui->splitterChat->sizes()[0] > 0)
	{
		quazaaSettings.WinMain.ChatSplitterRestoreLeft = ui->splitterChat->sizes()[0];
		quazaaSettings.WinMain.ChatSplitterRestoreMiddle = ui->splitterChat->sizes()[1];
		QList<int> newSizes;
		newSizes.append(0);
		newSizes.append(ui->splitterChat->sizes()[0] + ui->splitterChat->sizes()[1]);
		newSizes.append(ui->splitterChat->sizes()[2]);
		ui->splitterChat->setSizes(newSizes);
	}
	else
	{
		QList<int> sizesList;
		sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreLeft);
		sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreMiddle);
		sizesList.append(ui->splitterChat->sizes()[2]);
		ui->splitterChat->setSizes(sizesList);
	}
}

void WidgetIrcSidebars::on_toolButtonChatUsersHeader_clicked()
{
	if(ui->splitterChat->sizes()[2] > 0)
	{
		quazaaSettings.WinMain.ChatSplitterRestoreMiddle = ui->splitterChat->sizes()[1];
		quazaaSettings.WinMain.ChatSplitterRestoreRight = ui->splitterChat->sizes()[2];
		QList<int> newSizes;
		newSizes.append(ui->splitterChat->sizes()[0]);
		newSizes.append(ui->splitterChat->sizes()[1] + ui->splitterChat->sizes()[2]);
		newSizes.append(0);
		ui->splitterChat->setSizes(newSizes);
	}
	else
	{
		QList<int> sizesList;
		sizesList.append(ui->splitterChat->sizes()[0]);
		sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreMiddle);
		sizesList.append(quazaaSettings.WinMain.ChatSplitterRestoreRight);
		ui->splitterChat->setSizes(sizesList);
	}
}

void WidgetIrcSidebars::onMsgTabChanged(SessionTabWidget* widget)
{
	MessageView* view = qobject_cast<MessageView*>(widget->currentWidget());

	if( view )
	{
		if( view->isChannelView() )
		{
			if( qobject_cast<MessageView*>(panelIrcMain->currentSession()->currentWidget()) == view)
			{
				ui->frameChatRightSidebar->show();
				ui->listViewChatUsers->setModel(view->userList());
			}
		}
		else
		{
			ui->frameChatRightSidebar->hide();
		}
	}
}

void WidgetIrcSidebars::setSkin()
{
	ui->frameChatLeftSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonChatFriendsHeader->setStyleSheet(skinSettings.taskHeader);
	ui->frameChatFriendsTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->toolButtonChatUsersHeader->setStyleSheet(skinSettings.taskHeader);
	ui->frameChatUsersTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->frameChatRightSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->listViewChatFriends->setStyleSheet(skinSettings.listViews);
	ui->listViewChatUsers->setStyleSheet(skinSettings.listViews);
}
