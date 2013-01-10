/*
** chatuserlistmodel.h
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

#ifndef CHATUSERLISTMODEL_H
#define CHATUSERLISTMODEL_H

#include <QAbstractListModel>
#include <QStringListModel>
#include <QStringList>
#include <QIcon>
#include <QHash>

class Session;
class IrcMessage;

namespace UserMode
{
	enum UserMode {	Normal, Voice, HalfOperator, Operator, Administrator, Owner, Immune };
};

class IrcUserItem : public QObject
{
	Q_OBJECT
public:
	IrcUserItem(QString nick, QString modes, IrcUserItem* parent = 0);
	~IrcUserItem();

	UserMode::UserMode userMode;
	QString sNick;
	QString sDisplayNick;
	QString sModes;

	void appendChild(IrcUserItem* child);
	void clearChildren();

	IrcUserItem* child(int row);
	int childCount() const;
	int find(QString nick);
	int duplicateCheck(QString displayNick);
	QVariant data(int column) const;
	int row() const;
	QList<IrcUserItem*> childItems;
	IrcUserItem* parent();

private:
	QList<QVariant> itemData;
	IrcUserItem* parentItem;
};

class IrcUserListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	IrcUserListModel();
	~IrcUserListModel();

	Session* session() const;
	void setSession(Session* session);

	QString channel() const;

	QStringList users() const;
	bool hasUser(const QString& user) const;

	QVariant data(const QModelIndex& index, int role) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex& parent = QModelIndex()) const;
	QStringListModel* toStringListModel();
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int nOperatorCount;
	int nUserCount;
	void sort(Qt::SortOrder order = Qt::AscendingOrder);
	bool bNeedsSorting;
	QIcon iImmune;
	QIcon iOwner;
	QIcon iAdmin;
	QIcon iOperator;
	QIcon iHalfOperator;
	QIcon iVoice;
	QIcon iNormal;

signals:
	void updateUserCount(IrcUserListModel* chatUserListModel, int operators, int users);

private:
	IrcUserItem* rootItem;
	struct Private {
		QString channel;
		Session* session;
	} d;

public slots:
	void setChannel(const QString& channel);
	void processMessage(IrcMessage* message);

protected:
	bool isRoot(QModelIndex index);
	void addUser(QString name);
	void addUsers(QStringList users);
	void removeUser(QString name);
	void clearUsers();
	void renameUser(QString oldNick, QString newNick);
	void setUserMode(QString name, QString mode);
	UserMode::UserMode highestMode(int index);

private slots:
	QList<IrcUserItem*> caseInsensitiveSecondarySort(QList<IrcUserItem*> list, Qt::SortOrder order);
};

#endif // CHATUSERLISTMODEL_H
