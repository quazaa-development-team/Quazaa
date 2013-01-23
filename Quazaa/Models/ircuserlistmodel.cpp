/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
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

#include "ircuserlistmodel.h"

#include "session.h"
#include "ircsessioninfo.h"
#include <ircmessage.h>
#include <ircsender.h>
#include <irc.h>
#include <QDebug>

#ifdef _DEBUG
#include "debug_new.h"
#endif

IrcUserListModel::IrcUserListModel()
{
	rootItem = new IrcUserItem("Users", "");
	nOperatorCount = 0;
	nUserCount = 0;
	iImmune = QIcon(":/Resource/Chat/Immune.png");
	iOwner = QIcon(":/Resource/Chat/Owner.png");
	iAdmin = QIcon(":/Resource/Chat/Admin.png");
	iOperator = QIcon(":/Resource/Chat/Op.png");
	iHalfOperator = QIcon(":/Resource/Chat/HalfOp.png");
	iVoice = QIcon(":/Resource/Chat/Voice.png");
	iNormal = QIcon(":/Resource/Chat/Normal.png");
}

IrcUserListModel::~IrcUserListModel()
{
	clearUsers();
	delete rootItem;
}

Session *IrcUserListModel::session() const
{
	return d.session;
}

void IrcUserListModel::setSession(Session *session)
{
	d.session = session;
	clearUsers();
}

QString IrcUserListModel::channel() const
{
	return d.channel;
}

QStringList IrcUserListModel::users() const
{
	QStringList nicks;

	for (int i = 0; i < rootItem->childItems.size(); ++i)
	{
		nicks.append(rootItem->childItems.at(i)->sNick);
	}

	return nicks;
}

bool IrcUserListModel::hasUser(const QString &user) const
{
	return users().contains(user, Qt::CaseInsensitive);
}

bool IrcUserListModel::isRoot(QModelIndex index)
{
	IrcUserItem* item;

	if(!index.isValid())
	{
		item = rootItem;
	}
	else
	{
		item = static_cast<IrcUserItem*>(index.internalPointer());
	}

	if(item == rootItem)
	{
		return true;
	}

	return false;
}

QVariant IrcUserListModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
	{
		return QVariant();
	}

	IrcUserItem* item = static_cast<IrcUserItem*>(index.internalPointer());

	if(role == Qt::DecorationRole)
	{
		switch(item->userMode)
		{
			case UserMode::Immune:
				return iImmune;
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

Qt::ItemFlags IrcUserListModel::flags(const QModelIndex& index) const
{
	if(!index.isValid())
	{
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant IrcUserListModel::headerData(int section, Qt::Orientation orientation,
									   int role) const
{
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return rootItem->data(section);
	}

	return QVariant();
}

QModelIndex IrcUserListModel::index(int row, int column, const QModelIndex& parent)
const
{
	if(!hasIndex(row, column, parent))
	{
		return QModelIndex();
	}

	IrcUserItem* parentItem;

	if(!parent.isValid())
	{
		parentItem = rootItem;
	}
	else
	{
		parentItem = static_cast<IrcUserItem*>(parent.internalPointer());
	}

	IrcUserItem* childItem = parentItem->child(row);
	if(childItem)
	{
		return createIndex(row, column, childItem);
	}
	else
	{
		return QModelIndex();
	}
}

QStringListModel *IrcUserListModel::toStringListModel()
{
	QStringList nicks = users();

	if (!nicks.isEmpty())
		return new QStringListModel(nicks);
	else
		return new QStringListModel();
}

int IrcUserListModel::rowCount(const QModelIndex& parent) const
{
	IrcUserItem* parentItem;
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
		parentItem = static_cast<IrcUserItem*>(parent.internalPointer());
	}

	return parentItem->childCount();
}

void IrcUserListModel::clearUsers()
{
	beginRemoveRows(QModelIndex(), 0, rootItem->childCount());
	rootItem->clearChildren();
	endRemoveRows();

	QModelIndex idx1 = index(0, 0, QModelIndex());
	QModelIndex idx2 = index(rootItem->childCount(), 10, QModelIndex());
	emit dataChanged(idx1, idx2);
}

void IrcUserListModel::addUser(QString name)
{
	addUsers(QStringList() << name);
}

void IrcUserListModel::addUsers(QStringList users)
{
	foreach(const QString & user, users) {
		QString name = user;
		QString modes;

		IrcSessionInfo sessionInfo(d.session);

		for(int i = 0; i  < sessionInfo.prefixes().size(); i++)
		{
			if(user.startsWith(sessionInfo.prefixes().at(i)))
			{
				modes = sessionInfo.modes().at(i);
				name.remove(0,1);
			}
		}

		int existingUser = rootItem->find(name);

        IrcUserItem* m_oChatUserItem = new IrcUserItem(name, modes, rootItem);

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
}

void IrcUserListModel::removeUser(QString name)
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

void IrcUserListModel::sort(Qt::SortOrder order)
{
	QList<IrcUserItem*> immuneList;
	QList<IrcUserItem*> ownerList;
	QList<IrcUserItem*> administratorList;
	QList<IrcUserItem*> operatorList;
	QList<IrcUserItem*> halfOperatorList;
	QList<IrcUserItem*> voiceList;
	QList<IrcUserItem*> normalList;
	nOperatorCount = 0;
	nUserCount = 0;

	emit layoutAboutToBeChanged();
	if(order == Qt::AscendingOrder)
	{
		for(int i = 0; i < rootItem->childItems.size(); ++i)
		{
			switch(rootItem->childItems.at(i)->userMode)
			{
				case UserMode::Immune:
					immuneList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
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
		rootItem->childItems.append(caseInsensitiveSecondarySort(immuneList, order));
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
				case UserMode::Immune:
					immuneList.append(rootItem->childItems.at(i));
					nOperatorCount++;
					break;
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
		rootItem->childItems.append(caseInsensitiveSecondarySort(immuneList, order));
	}
	nUserCount = rootItem->childCount();
	emit layoutChanged();
	emit updateUserCount(this, nOperatorCount, nUserCount);
}

QList<IrcUserItem*> IrcUserListModel::caseInsensitiveSecondarySort(QList<IrcUserItem*> list, Qt::SortOrder order)
{
	if(order == Qt::AscendingOrder)
	{
		QMap<QString, IrcUserItem*> map;
		for(int i = 0; i < list.size(); ++i)
		{
			map.insert(list.at(i)->sNick.toLower(), list.at(i));
		}

		list = map.values();
		return list;
	}
	else
	{
		QMap<QString, IrcUserItem*> map;
		for(int i = 0; i < list.size(); ++i)
		{
			map.insert(list.at(i)->sNick.toLower(), list.at(i));
		}

		list.clear();
		QMapIterator<QString, IrcUserItem*> i(map);
		for(int i = map.size(); i > 0; --i)
		{
			list.append(map.values().at(i));
		}
		return list;
	}
}

void IrcUserListModel::setUserMode(QString name, QString mode)
{
	int existingUser = rootItem->find(name);

	if(existingUser != -1)
	{
		QString sAction = mode.at(0);
		QString sMode = mode.mid(1);
		if(sAction == "+")
		{
			if(sMode.contains('Y'))
			{
				rootItem->childItems.at(existingUser)->sModes += "Y";
				rootItem->childItems.at(existingUser)->userMode = UserMode::Immune;
			}
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
			if(sMode.contains('Y'))
			{
				rootItem->childItems.at(existingUser)->sModes.remove("Y");
				rootItem->childItems.at(existingUser)->userMode = highestMode(existingUser);
			}
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

UserMode::UserMode IrcUserListModel::highestMode(int index)
{
	if(rootItem->childItems.at(index)->sModes.contains('Y'))
	{
		return UserMode::Immune;
	}
	else if(rootItem->childItems.at(index)->sModes.contains('q'))
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

void IrcUserListModel::renameUser(QString oldNick, QString newNick)
{
	int existingUser = rootItem->find(oldNick);

	if(existingUser != -1)
	{
		rootItem->childItems.at(existingUser)->sNick = newNick;
		sort();
	}
}

void IrcUserListModel::setChannel(const QString &channel)
{
	d.channel = channel.toLower();
}

void IrcUserListModel::processMessage(IrcMessage *message)
{
	if (!d.session) {
		qWarning() << "IrcUserListModel::processMessage(): session is null!";
		return;
	}

	if (message->type() == IrcMessage::Nick) {
		QString nick = message->sender().name().toLower();
		renameUser(nick, static_cast<IrcNickMessage*>(message)->nick());
	} else if (message->type() == IrcMessage::Join) {
        if (message->flags() & IrcMessage::Own)
			clearUsers();
		else
			addUser(message->sender().name());
	} else if (message->type() == IrcMessage::Part) {
        if (message->flags() & IrcMessage::Own)
			clearUsers();
		else
			removeUser(message->sender().name());
	} else if (message->type() == IrcMessage::Kick) {
		removeUser(static_cast<IrcKickMessage*>(message)->user());
	} else if (message->type() == IrcMessage::Quit) {
		removeUser(message->sender().name());
	} else if (message->type() == IrcMessage::Mode) {
		IrcModeMessage* modeMsg = static_cast<IrcModeMessage*>(message);
		if (modeMsg->sender().name() != modeMsg->target() && !modeMsg->argument().isEmpty())
			setUserMode(modeMsg->argument(), modeMsg->mode());
	} else if (message->type() == IrcMessage::Numeric) {
		if (static_cast<IrcNumericMessage*>(message)->code() == Irc::RPL_NAMREPLY) {
			int count = message->parameters().count();
			if (!d.channel.isNull() && d.channel == message->parameters().value(count - 2).toLower()) {
				QString names = message->parameters().value(count - 1);
				addUsers(names.split(" ", QString::SkipEmptyParts));
			}
		}
	}
}

IrcUserItem::IrcUserItem(QString nick, QString modes, IrcUserItem* parent)
{
	parentItem = parent;
	sModes = modes;

	if(modes.contains('Y'))
	{
		sNick = nick;
		userMode = UserMode::Immune;
	}
	else if(modes.contains('q'))
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

IrcUserItem::~IrcUserItem()
{
	qDeleteAll(childItems);
}

void IrcUserItem::appendChild(IrcUserItem* item)
{
	childItems.append(item);
}

void IrcUserItem::clearChildren()
{
	childItems.clear();
}

IrcUserItem* IrcUserItem::child(int row)
{
	return childItems.value(row);
}

int IrcUserItem::childCount() const
{
	return childItems.count();
}

int IrcUserItem::find(QString nick)
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

QVariant IrcUserItem::data(int column) const
{
	return itemData.value(column);
}

IrcUserItem* IrcUserItem::parent()
{
	return parentItem;
}

int IrcUserItem::row() const
{
	if(parentItem)
	{
		return childItems.indexOf(const_cast<IrcUserItem*>(this));
	}

	return 0;
}

int IrcUserItem::duplicateCheck(QString displayNick)
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
