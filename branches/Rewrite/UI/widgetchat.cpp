/*
** widgetchat.cpp
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
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

#include "widgetchat.h"
#include "ui_widgetchat.h"

#include "quazaasettings.h"
 
#include "systemlog.h"

WidgetChat::WidgetChat(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::WidgetChat)
{
	ui->setupUi(this);
	panelChatMiddle = new WidgetChatMiddle();
	ui->splitterChat->restoreState(quazaaSettings.WinMain.ChatSplitter);
	ui->verticalLayoutChatMiddle->addWidget(panelChatMiddle);
	ui->toolButtonChatFriendsHeader->setChecked(quazaaSettings.WinMain.ChatFriendsTaskVisible);
	ui->toolButtonChatRoomsHeader->setChecked(quazaaSettings.WinMain.ChatRoomsTaskVisible);
	ui->listViewChatRooms->setModel(panelChatMiddle->channelListModel);
	connect(panelChatMiddle, SIGNAL(roomChanged(WidgetChatRoom*)), this, SLOT(updateUserList(WidgetChatRoom*)));
	connect(panelChatMiddle, SIGNAL(updateUserCount(int,int)), this, SLOT(updateUserCount(int,int)));
	connect(this, SIGNAL(changeRoom(int)), panelChatMiddle, SLOT(changeRoom(int)));
}

WidgetChat::~WidgetChat()
{
	panelChatMiddle->close();
	delete ui;
}

void WidgetChat::changeEvent(QEvent* e)
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

void WidgetChat::saveWidget()
{
	quazaaSettings.WinMain.ChatSplitter = ui->splitterChat->saveState();
	quazaaSettings.WinMain.ChatFriendsTaskVisible = ui->toolButtonChatFriendsHeader->isChecked();
	quazaaSettings.WinMain.ChatRoomsTaskVisible = ui->toolButtonChatRoomsHeader->isChecked();
	panelChatMiddle->saveWidget();
}

void WidgetChat::on_splitterChat_customContextMenuRequested(QPoint pos)
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

void WidgetChat::updateUserList(WidgetChatRoom* currentTab)
{
	//qDebug() << "Signal successful. In WidgetChat::updateUserList()";
	if (currentTab != 0)
	{
		ui->frameChatRightSidebar->setVisible(true);
		ui->listViewChatUsers->setModel(currentTab->chatUserListModel);
		ui->labelChatUsersCount->setText(tr("%1 Operators, %2 Total").arg(currentTab->chatUserListModel->nOperatorCount).arg(currentTab->chatUserListModel->nUserCount));
	} else {
		ui->frameChatRightSidebar->setVisible(false);
	}
}

void WidgetChat::on_listViewChatRooms_pressed(QModelIndex index)
{
	switch (QApplication::mouseButtons ())
	{
	case Qt::LeftButton:
		emit changeRoom(index.row());
		break;
	case Qt::RightButton:
		break;
	default:
		break;
	}
}

void WidgetChat::on_toolButtonChatRoomAdd_clicked()
{
	bool ok;
	QString channel = QInputDialog::getText(this, tr("Enter room to join."),
										  tr("Room:"), QLineEdit::Normal,
										  "#", &ok);
	if (ok && !channel.isEmpty())
		 panelChatMiddle->quazaaIrc->addRoom(channel);
}

void WidgetChat::on_toolButtonChatRoomRemove_clicked()
{
	if (ui->listViewChatRooms->currentIndex().row() > 0)
	{
		panelChatMiddle->quazaaIrc->removeRoom(ui->listViewChatRooms->currentIndex().data(Qt::DisplayRole).toString());
	}
}

void WidgetChat::updateUserCount(int operators, int users)
{
	ui->labelChatUsersCount->setText(tr("%1 Operators, %2 Total").arg(operators).arg(users));
}
