/*
** chatuserlistmodel.h
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

#ifndef CHATUSERLISTMODEL_H
#define CHATUSERLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QIcon>
#include <QHash>
#include <ircbuffer.h>

namespace UserMode
{
	enum UserMode {	Normal, Voice, HalfOperator, Operator, Administrator, Owner };
};

class ChatUserItem : public QObject
{
	Q_OBJECT
public:
	ChatUserItem(QString nick, QString modes, ChatUserItem* parent = 0);
	~ChatUserItem();

	UserMode::UserMode userMode;
	QString sNick;
	QString sDisplayNick;
	QString sModes;

	void appendChild(ChatUserItem* child);
	void clearChildren();

	ChatUserItem* child(int row);
	int childCount() const;
	int find(QString nick);
	int duplicateCheck(QString displayNick);
	QVariant data(int column) const;
	int row() const;
	QList<ChatUserItem*> childItems;
	ChatUserItem* parent();

private:
	QList<QVariant> itemData;
	ChatUserItem* parentItem;
};

class ChatUserListModel : public QAbstractListModel
{
    Q_OBJECT
public:
	ChatUserListModel();
	~ChatUserListModel();

	QVariant data(const QModelIndex& index, int role) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex& parent = QModelIndex()) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int nOperatorCount;
	int nUserCount;
	void sort(Qt::SortOrder order = Qt::AscendingOrder);
	bool bNeedsSorting;
	QIcon iOwner;
	QIcon iAdmin;
	QIcon iOperator;
	QIcon iHalfOperator;
	QIcon iVoice;
	QIcon iNormal;

signals:
	void updateUserCount(ChatUserListModel* chatUserListModel, int operators, int users);

private:
	ChatUserItem* rootItem;

public slots:
	void clear();
	bool isRoot(QModelIndex index);
	void addUser(QString name, QString modes);
	void addUsers(Irc::Buffer* buffer);
	void removeUser(QString name);
	void updateUserMode(QString hostmask, QString mode, QString name);
	UserMode::UserMode highestMode(int index);
	void changeNick(QString oldNick, QString newNick);

private slots:
	QList<ChatUserItem*> caseInsensitiveSecondarySort(QList<ChatUserItem*> list, Qt::SortOrder order);
};

#endif // CHATUSERLISTMODEL_H
