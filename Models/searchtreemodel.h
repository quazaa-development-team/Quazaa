#ifndef SEARCHTREEMODEL_H
#define SEARCHTREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>

class SearchTreeItem
{
public:
	SearchTreeItem(const QList<QVariant> &data, SearchTreeItem *parent = 0);
	~SearchTreeItem();

	void appendChild(SearchTreeItem *child);

	SearchTreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
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
};

#endif // SEARCHTREEMODEL_H
