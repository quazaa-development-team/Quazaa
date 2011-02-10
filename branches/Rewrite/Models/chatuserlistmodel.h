#ifndef CHATUSERLISTMODEL_H
#define CHATUSERLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QIcon>

namespace UserMode
{
	enum UserMode {	Normal, Voice, HalfOperator, Operator, Administrator, Owner };
};

class ChatUserItem : public QObject
{
	Q_OBJECT
public:
	ChatUserItem(QString name = "", ChatUserItem* parent = 0);
	~ChatUserItem();

	UserMode::UserMode userMode;
	QString sNick;
	QString sDisplayNick;

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
	void sort(Qt::SortOrder order = Qt::AscendingOrder);
	QIcon iOwner;
	QIcon iAdmin;
	QIcon iOperator;
	QIcon iHalfOperator;
	QIcon iVoice;
	QIcon iNormal;

signals:
	void updateUserCount(int operators, int users);

private:
	ChatUserItem* rootItem;

public slots:
	void clear();
	bool isRoot(QModelIndex index);
	void addUser(QString name);
	void addUsers(QStringList names);
	void removeUser(QString name);

private slots:
	QList<ChatUserItem*> caseInsensitiveSecondarySort(QList<ChatUserItem*> list, Qt::SortOrder order);
};

#endif // CHATUSERLISTMODEL_H
