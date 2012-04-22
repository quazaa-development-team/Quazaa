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

#include "chatuserlistmodel.h"

#include "ircutil.h"

#include <QDebug>

#ifdef _DEBUG
#include "debug_new.h"
#endif

ChatUserListModel::ChatUserListModel()
{
	rootItem = new ChatUserItem("Users", "");
	nOperatorCount = 0;
	nUserCount = 0;
	iOwner = QIcon(":/Resource/Chat/Owner.png");
	iAdmin = QIcon(":/Resource/Chat/Admin.png");
	iOperator = QIcon(":/Resource/Chat/Op.png");
	iHalfOperator = QIcon(":/Resource/Chat/HalfOp.png");
	iVoice = QIcon(":/Resource/Chat/Voice.png");
	iNormal = QIcon(":/Resource/Chat/Normal.png");
}

ChatUserListModel::~ChatUserListModel()
{
	clear();
	delete rootItem;
}

bool ChatUserListModel::isRoot(QModelIndex index)
{
	ChatUserItem* item;

	if(!index.isValid())
	{
		item = rootItem;
	}
	else
	{
		item = static_cast<ChatUserItem*>(index.internalPointer());
	}

	if(item == rootItem)
	{
		return true;
	}

	return false;
}

QVariant ChatUserListModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
	{
		return QVariant();
	}

	ChatUserItem* item = static_cast<ChatUserItem*>(index.internalPointer());

	if(role == Qt::DecorationRole)
	{
		switch(item->userMode)
		{
			case UserMode::Owner:
				return iOwner;
			case UserMode::Administrator:
				return iAdmin;
			case UserMode::Operator:
				return iOperator;
			case UserMode::HalfOperator:
				return iHalfOperator;
			case UserMode::Voice:
				return iVoice;
			default:
				return iNormal;
		}
	}

	if(role == Qt::DisplayRole)
	{
		return item->sNick;
	}

	if(role == Qt::ToolTipRole)
	{
		//todo: put tooltip here when its ready :D
	}
	return QVariant();
}

Qt::ItemFlags ChatUserListModel::flags(const QModelIndex& index) const
{
	if(!index.isValid())
	{
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ChatUserListModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return rootItem->data(section);
	}

	return QVariant();
}

QModelIndex ChatUserListModel::index(int row, int column, const QModelIndex& parent)
const
{
	if(!hasIndex(row, column, parent))
	{
		return QModelIndex();
	}

	ChatUserItem* parentItem;

	if(!parent.isValid())
	{
		parentItem = rootItem;
	}
	else
	{
		parentItem = static_cast<ChatUserItem*>(parent.internalPointer());
	}

	ChatUserItem* childItem = parentItem->child(row);
	if(childItem)
	{
		return createIndex(row, column, childItem);
	}
	else
	{
		return QModelIndex();
	}
}

int ChatUserListModel::rowCount(const QModelIndex& parent) const
{
	ChatUserItem* parentItem;
	if(parent.column() > 0)
	{
		return 0;
	}

	if(!parent.isValid())
	{
		parentItem = rootItem;
	}
	else
	{
		parentItem = static_cast<ChatUserItem*>(parent.internalPointer());
	}

	return parentItem->childCount();
}

void ChatUserListModel::clear()
{
	beginRemoveRows(QModelIndex(), 0, rootItem->childCount());
	rootItem->clearChildren();
	endRemoveRows();

	QModelIndex idx1 = index(0, 0, QModelIndex());
	QModelIndex idx2 = index(rootItem->childCount(), 10, QModelIndex());
	emit dataChanged(idx1, idx2);
}

void ChatUserListModel::addUser(QString name, QString modes)
{
	int existingUser = rootItem->find(name);

	ChatUserItem* m_oChatUserItem = new ChatUserItem(name, modes, rootItem);

	if(existingUser == -1)
	{
		beginInsertRows(QModelIndex(), rootItem->childCount(), rootItem->childCount());
		rootItem->appendChild(m_oChatUserItem);
		endInsertRows();
		QModelIndex idx1 = index(0, 0, QModelIndex());
		QModelIndex idx2 = index(rootItem->childCount(), 1, QModelIndex());
		emit dataChanged(idx1, idx2);
		sort(Qt::AscendingOrder);
	}
	else
	{
		int duplicate = rootItem->duplicateCheck(m_oChatUserItem->sNick);
		if(duplicate != -1)
		{
			rootItem->childItems.replace(duplicate, m_oChatUserItem);
			sort();
			QModelIndex idx1 = index(0, 0, QModelIndex());
			QModelIndex idx2 = index(rootItem->childCount(), 1, QModelIndex());
			emit dataChanged(idx1, idx2);
			sort(Qt::AscendingOrder);
		}
	}
}

void ChatUserListModel::removeUser(QString name)
{
	int existingUser = rootItem->find(name);

	if(existingUser != -1)
	{
		beginRemoveRows(QModelIndex(), 0, rootItem->childCount());
		rootItem->childItems.removeAt(existingUser);
		endRemoveRows();

		QModelIndex idx1 = index(0, 0, QModelIndex());
		QModelIndex idx2 = index(rootItem->childCount(), 10, QModelIndex());
		emit dataChanged(idx1, idx2);
	}
}

void ChatUserListModel::addUsers(QStringList users)
{

}

void ChatUserListModel::sort(Qt::SortOrder order)
{
	QList<ChatUserItem*> ownerList;
	QList<ChatUserItem*> administratorList;
	QList<ChatUserItem*> operatorList;
	QList<ChatUserItem*> halfOperatorList;
	QList<ChatUserItem*> voiceList;
	QList<ChatUserItem*> normalList;
	nOperatorCount = 0;
	nUserCount = 0;

	emit layoutAboutToBeChanged();
	if(order == Qt::AscendingOrder)
	{
		for(int i = 0; i < rootItem->childItems.size(); ++i)
		{
			switch(rootItem->childItems.at(i)->userMode)
			{
				case UserMode::Owner:
					ownerList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::Administrator:
					administratorList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::Operator:
					operatorList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::HalfOperator:
					halfOperatorList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::Voice:
					voiceList.append(rootItem->childItems.at(i));
					break;
				default:
					normalList.append(rootItem->childItems.at(i));
					break;
			}
		}

		rootItem->childItems.clear();
		rootItem->childItems.append(caseInsensitiveSecondarySort(ownerList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(administratorList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(operatorList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(halfOperatorList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(voiceList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(normalList, order));
	}
	else if(order == Qt::DescendingOrder)
	{
		for(int i = rootItem->childItems.size(); i > 0; --i)
		{
			switch(rootItem->childItems.at(i)->userMode)
			{
				case UserMode::Owner:
					ownerList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::Administrator:
					administratorList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::Operator:
					operatorList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::HalfOperator:
					halfOperatorList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
				case UserMode::Voice:
					voiceList.append(rootItem->childItems.at(i));
					break;
				default:
					normalList.append(rootItem->childItems.at(i));
					break;
			}
		}

		rootItem->childItems.clear();
		rootItem->childItems.append(caseInsensitiveSecondarySort(normalList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(voiceList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(halfOperatorList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(operatorList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(administratorList, order));
		rootItem->childItems.append(caseInsensitiveSecondarySort(ownerList, order));
	}
	nUserCount = rootItem->childCount();
	emit layoutChanged();
	emit updateUserCount(this, nOperatorCount, nUserCount);
}

QList<ChatUserItem*> ChatUserListModel::caseInsensitiveSecondarySort(QList<ChatUserItem*> list, Qt::SortOrder order)
{
	if(order == Qt::AscendingOrder)
	{
		QMap<QString, ChatUserItem*> map;
		for(int i = 0; i < list.size(); ++i)
		{
			map.insert(list.at(i)->sNick.toLower(), list.at(i));
		}

		list = map.values();
		return list;
	}
	else
	{
		QMap<QString, ChatUserItem*> map;
		for(int i = 0; i < list.size(); ++i)
		{
			map.insert(list.at(i)->sNick.toLower(), list.at(i));
		}

		list.clear();
		QMapIterator<QString, ChatUserItem*> i(map);
		for(int i = map.size(); i > 0; --i)
		{
			list.append(map.values().at(i));
		}
		return list;
	}
}

void ChatUserListModel::updateUserMode(QString mode, QString name)
{
	int existingUser = rootItem->find(name);

	if(existingUser != -1)
	{
		QString sAction = mode.at(0);
		QString sMode = mode.at(1);
		if(sAction == "+")
		{
			if(sMode.contains('q'))
			{
				rootItem->childItems.at(existingUser)->sModes += "q";
				rootItem->childItems.at(existingUser)->userMode = UserMode::Owner;
			}
			else if(sMode.contains('a') && highestMode(existingUser) < UserMode::Administrator)
			{
				rootItem->childItems.at(existingUser)->sModes += "a";
				rootItem->childItems.at(existingUser)->userMode = UserMode::Administrator;
			}
			else if(sMode.contains('o') && highestMode(existingUser) < UserMode::Operator)
			{
				rootItem->childItems.at(existingUser)->sModes += "o";
				rootItem->childItems.at(existingUser)->userMode = UserMode::Operator;
			}
			else if(sMode.contains('h') && highestMode(existingUser) < UserMode::HalfOperator)
			{
				rootItem->childItems.at(existingUser)->sModes += "h";
				rootItem->childItems.at(existingUser)->userMode = UserMode::HalfOperator;
			}
			else if(sMode.contains('v') && highestMode(existingUser) < UserMode::Voice)
			{
				rootItem->childItems.at(existingUser)->sModes += "v";
				rootItem->childItems.at(existingUser)->userMode = UserMode::Voice;
			}
		}
		else
		{
			if(sMode.contains('q'))
			{
				rootItem->childItems.at(existingUser)->sModes.remove("q");
				rootItem->childItems.at(existingUser)->userMode = highestMode(existingUser);
			}
			else if(sMode.contains('a'))
			{
				rootItem->childItems.at(existingUser)->sModes.remove("a");
				rootItem->childItems.at(existingUser)->userMode = highestMode(existingUser);
			}
			else if(sMode.contains('o'))
			{
				rootItem->childItems.at(existingUser)->sModes.remove("o");
				rootItem->childItems.at(existingUser)->userMode = highestMode(existingUser);
			}
			else if(sMode.contains('h'))
			{
				rootItem->childItems.at(existingUser)->sModes.remove("h");
				rootItem->childItems.at(existingUser)->userMode = highestMode(existingUser);
			}
			else if(sMode.contains('v'))
			{
				rootItem->childItems.at(existingUser)->sModes.remove("v");
				rootItem->childItems.at(existingUser)->userMode = highestMode(existingUser);
			}
		}
		sort();
	}
}

UserMode::UserMode ChatUserListModel::highestMode(int index)
{
	if(rootItem->childItems.at(index)->sModes.contains('q'))
	{
		return UserMode::Owner;
	}
	else if(rootItem->childItems.at(index)->sModes.contains('a'))
	{
		return UserMode::Administrator;
	}
	else if(rootItem->childItems.at(index)->sModes.contains('o'))
	{
		return UserMode::Operator;
	}
	else if(rootItem->childItems.at(index)->sModes.contains('h'))
	{
		return UserMode::HalfOperator;
	}
	else if(rootItem->childItems.at(index)->sModes.contains('v'))
	{
		return UserMode::Voice;
	}
	else
	{
		return UserMode::Normal;
	}
}

void ChatUserListModel::changeNick(QString oldNick, QString newNick)
{
	int existingUser = rootItem->find(oldNick);

	if(existingUser != -1)
	{
		rootItem->childItems.at(existingUser)->sNick = newNick;
		sort();
	}
}

ChatUserItem::ChatUserItem(QString nick, QString modes, ChatUserItem* parent)
{
	parentItem = parent;
	sModes = modes;

	if(modes.contains('q'))
	{
		sNick = nick;
		userMode = UserMode::Owner;
	}
	else if(modes.contains('a'))
	{
		sNick = nick;
		userMode = UserMode::Administrator;
	}
	else if(modes.contains('o'))
	{
		sNick = nick;
		userMode = UserMode::Operator;
	}
	else if(modes.contains('h'))
	{
		sNick = nick;
		userMode = UserMode::HalfOperator;
	}
	else if(modes.contains('v'))
	{
		sNick = nick;
		userMode = UserMode::Voice;
	}
	else
	{
		sNick = nick;
		userMode = UserMode::Normal;
	}
}

ChatUserItem::~ChatUserItem()
{
	qDeleteAll(childItems);
}

void ChatUserItem::appendChild(ChatUserItem* item)
{
	childItems.append(item);
}

void ChatUserItem::clearChildren()
{
	childItems.clear();
}

ChatUserItem* ChatUserItem::child(int row)
{
	return childItems.value(row);
}

int ChatUserItem::childCount() const
{
	return childItems.count();
}

int ChatUserItem::find(QString nick)
{
	for(int index = 0; index < childItems.size(); ++index)
	{
		if(child(index)->sNick == nick)
		{
			return index;
		}
	}
	return -1;
}

QVariant ChatUserItem::data(int column) const
{
	return itemData.value(column);
}

ChatUserItem* ChatUserItem::parent()
{
	return parentItem;
}

int ChatUserItem::row() const
{
	if(parentItem)
	{
		return childItems.indexOf(const_cast<ChatUserItem*>(this));
	}

	return 0;
}

int ChatUserItem::duplicateCheck(QString displayNick)
{
	for(int index = 0; index < childItems.size(); ++index)
	{
		if(child(index)->sNick == displayNick)
		{
			return index;
		}
	}
	return -1;
}

