#include "chatuserlistmodel.h"

#include "ircutil.h"

#include <QDebug>

ChatUserListModel::ChatUserListModel()
{
	rootItem = new ChatUserItem("Users");
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
		switch (item->userMode)
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
		return item->sDisplayNick;
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

void ChatUserListModel::addUser(QString name)
{
	int existingUser = rootItem->find(name);

	ChatUserItem* m_oChatUserItem = new ChatUserItem(name, rootItem);

	if(existingUser == -1)
	{
		if (m_oChatUserItem->userMode == (UserMode::Owner || UserMode::Administrator || UserMode::Operator ||
										  UserMode::HalfOperator))
			++nOperatorCount;
		beginInsertRows(QModelIndex(), rootItem->childCount(), rootItem->childCount());
		rootItem->appendChild(m_oChatUserItem);
		endInsertRows();
		QModelIndex idx1 = index(0, 0, QModelIndex());
		QModelIndex idx2 = index(rootItem->childCount(), 1, QModelIndex());
		emit dataChanged(idx1, idx2);
		sort(Qt::AscendingOrder);
		nUserCount = rootItem->childCount();
		emit updateUserCount(nOperatorCount, nUserCount);
	}
	else
	{
		int duplicate = rootItem->duplicateCheck(m_oChatUserItem->sDisplayNick);
		if(duplicate != -1)
		{
			rootItem->childItems.replace(duplicate, m_oChatUserItem);
			sort();
			QModelIndex idx1 = index(0, 0, QModelIndex());
			QModelIndex idx2 = index(rootItem->childCount(), 1, QModelIndex());
			emit dataChanged(idx1, idx2);
			sort(Qt::AscendingOrder);
			nUserCount = rootItem->childCount();
			emit updateUserCount(nOperatorCount, nUserCount);
		}
	}
}

void ChatUserListModel::removeUser(QString name)
{
	int existingUser = rootItem->find(name);

	if (existingUser != -1)
	{
		beginRemoveRows(QModelIndex(), 0, rootItem->childCount());
		rootItem->childItems.removeAt(existingUser);
		endRemoveRows();

		QModelIndex idx1 = index(0, 0, QModelIndex());
		QModelIndex idx2 = index(rootItem->childCount(), 10, QModelIndex());
		emit dataChanged(idx1, idx2);
	}
}

void ChatUserListModel::addUsers(QStringList names)
{
	for (int i = 0; i < names.size(); ++i)
		addUser(names.at(i));
}

void ChatUserListModel::sort(Qt::SortOrder order)
{
	QList<ChatUserItem*> ownerList;
	QList<ChatUserItem*> administratorList;
	QList<ChatUserItem*> operatorList;
	QList<ChatUserItem*> halfOperatorList;
	QList<ChatUserItem*> voiceList;
	QList<ChatUserItem*> normalList;

	emit layoutAboutToBeChanged();
	if (order == Qt::AscendingOrder)
	{
		for (int i = 0; i < rootItem->childItems.size(); ++i)
		{
			switch (rootItem->childItems.at(i)->userMode)
			{
			case UserMode::Owner:
				ownerList.append(rootItem->childItems.at(i));
				break;
			case UserMode::Administrator:
				administratorList.append(rootItem->childItems.at(i));
				break;
			case UserMode::Operator:
				operatorList.append(rootItem->childItems.at(i));
				break;
			case UserMode::HalfOperator:
				halfOperatorList.append(rootItem->childItems.at(i));
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
	} else if (order == Qt::DescendingOrder){
		for (int i = rootItem->childItems.size(); i > 0; --i)
		{
			switch (rootItem->childItems.at(i)->userMode)
			{
			case UserMode::Owner:
				ownerList.append(rootItem->childItems.at(i));
				break;
			case UserMode::Administrator:
				administratorList.append(rootItem->childItems.at(i));
				break;
			case UserMode::Operator:
				operatorList.append(rootItem->childItems.at(i));
				break;
			case UserMode::HalfOperator:
				halfOperatorList.append(rootItem->childItems.at(i));
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
	emit layoutChanged();
}

QList<ChatUserItem*> ChatUserListModel::caseInsensitiveSecondarySort(QList<ChatUserItem*> list, Qt::SortOrder order)
{
	 if (order == Qt::AscendingOrder)
	 {
		 QMap<QString, ChatUserItem*> map;
		 for (int i = 0; i < list.size(); ++i)
			 map.insert(list.at(i)->sNick.toLower(), list.at(i));

		 list = map.values();
		 return list;
	 } else {
		 QMap<QString, ChatUserItem*> map;
		 for (int i = 0; i < list.size(); ++i)
			 map.insert(list.at(i)->sNick.toLower(), list.at(i));

		 list.clear();
		 QMapIterator<QString, ChatUserItem*> i(map);
		 for (int i = map.size(); i > 0; --i) {
			 list.append(map.values().at(i));
		 }
		 return list;
	 }
}

ChatUserItem::ChatUserItem(QString nick, ChatUserItem* parent)
{
	parentItem = parent;

	sNick = nick;
	if (nick.at(0) == '~')
	{
		sDisplayNick = nick.remove(0,1);
		userMode = UserMode::Owner;
	}
	else if (nick.at(0) == '&')
	{
		sDisplayNick = nick.remove(0,1);
		userMode = UserMode::Administrator;
	}
	else if (nick.at(0) == '@')
	{
		sDisplayNick = nick.remove(0,1);
		userMode = UserMode::Operator;
	}
	else if (nick.at(0) == '%')
	{
		sDisplayNick = nick.remove(0,1);
		userMode = UserMode::HalfOperator;
	}
	else if (nick.at(0) == '+')
	{
		sDisplayNick = nick.remove(0,1);
		userMode = UserMode::Voice;
	}
	else
	{
		sDisplayNick = nick;
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
		return parentItem->childItems.indexOf(const_cast<ChatUserItem*>(this));
	}

	return 0;
}

int ChatUserItem::duplicateCheck(QString displayNick)
{
	for(int index = 0; index < parentItem->childItems.size(); ++index)
	{
		if(parentItem->child(index)->sDisplayNick == displayNick)
		{
			return index;
		}
	}
	return -1;
}
