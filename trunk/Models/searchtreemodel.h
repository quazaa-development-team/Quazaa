#ifndef SEARCHTREEMODEL_H
#define SEARCHTREEMODEL_H

#include <QObject>
#include <QIcon>
#include <QAbstractItemModel>
#include "NetworkCore/QueryHit.h"


namespace SearchHitData {
	struct sSearchHitData {
		CSHA1 oSha1Hash;
		QIcon iNetwork;
		QIcon iCountry;
	};
};

class SearchTreeItem : public QObject
{
	Q_OBJECT
public:
	SearchTreeItem(const QList<QVariant> &data, SearchTreeItem *parent = 0);
	~SearchTreeItem();

	SearchHitData::sSearchHitData HitData;

	void appendChild(SearchTreeItem *child);
	void clearChildren();

	SearchTreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
	int find(SearchTreeItem *containerItem, QString hash);
	void updateHitCount(int count);
	bool duplicateCheck(SearchTreeItem *containerItem, QString ip);
	QVariant data(int column) const;
	int row() const;
	SearchTreeItem *parent();

private:
	QList<SearchTreeItem*> childItems;
	QList<QVariant> itemData;
	SearchTreeItem *parentItem;
};

class SearchTreeModel : public QAbstractItemModel
{
	Q_OBJECT;

public:
    SearchTreeModel();
	~SearchTreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
	void setupModelData(const QStringList &lines, SearchTreeItem *parent);
	SearchTreeItem *rootItem;

public slots:
	void clear();

private slots:
	void addQueryHit(QueryHitSharedPtr pHit);
};

#endif // SEARCHTREEMODEL_H
