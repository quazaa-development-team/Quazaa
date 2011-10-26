/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
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


#include "sharesnavigatortreemodel.h"
#include <QStyle>
#include "sharemanager.h"
#if defined(_MSC_VER) && defined(_DEBUG)
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#define new DEBUG_NEW
#endif
SharesNavigatorTreeModel::SharesNavigatorTreeModel()
{
        QList<QVariant> rootData;
		rootData << "Folder"
				 << "Full Path"
				 << "ParentIndex"
				 << "Index";
        rootItem = new SharesNavigatorTreeItem(rootData);
        nFileCount = 0;
}
SharesNavigatorTreeModel::~SharesNavigatorTreeModel()
{
        clear();
        delete rootItem;
}
bool SharesNavigatorTreeModel::isRoot(QModelIndex index)
{
        SharesNavigatorTreeItem* item;
        if(!index.isValid())
        {
                item = rootItem;
        }
        else
        {
                item = static_cast<SharesNavigatorTreeItem*>(index.internalPointer());
        }
        if(item == rootItem)
        {
                return true;
        }
        return false;
}
int SharesNavigatorTreeModel::columnCount(const QModelIndex& parent) const
{
        if(parent.isValid())
        {
                return static_cast<SharesNavigatorTreeItem*>(parent.internalPointer())->columnCount();
        }
        else
        {
                return rootItem->columnCount();
        }
}
QVariant SharesNavigatorTreeModel::data(const QModelIndex& index, int role) const
{
        if(!index.isValid())
        {
                return QVariant();
        }
        SharesNavigatorTreeItem* item = static_cast<SharesNavigatorTreeItem*>(index.internalPointer());
        if(role == Qt::DecorationRole)
        {
            return QStyle::SP_DirIcon;
        }
        if(role == Qt::DisplayRole)
        {
                return item->data(index.column());
        }
        return QVariant();
}
Qt::ItemFlags SharesNavigatorTreeModel::flags(const QModelIndex& index) const
{
        if(!index.isValid())
        {
                return 0;
        }
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
QVariant SharesNavigatorTreeModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
        if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        {
                return rootItem->data(section);
        }
        return QVariant();
}
QModelIndex SharesNavigatorTreeModel::index(int row, int column, const QModelIndex& parent)
const
{
        if(!hasIndex(row, column, parent))
        {
                return QModelIndex();
        }
        SharesNavigatorTreeItem* parentItem;
        if(!parent.isValid())
        {
                parentItem = rootItem;
        }
        else
        {
                parentItem = static_cast<SharesNavigatorTreeItem*>(parent.internalPointer());
        }
        SharesNavigatorTreeItem* childItem = parentItem->child(row);
        if(childItem)
        {
                return createIndex(row, column, childItem);
        }
        else
        {
                return QModelIndex();
        }
}
QModelIndex SharesNavigatorTreeModel::parent(const QModelIndex& index) const
{
        if(!index.isValid())
        {
                return QModelIndex();
        }
        SharesNavigatorTreeItem* childItem = static_cast<SharesNavigatorTreeItem*>(index.internalPointer());
        SharesNavigatorTreeItem* parentItem = childItem->parent();
        if(parentItem == rootItem)
        {
                return QModelIndex();
        }
        return createIndex(parentItem->row(), 0, parentItem);
}
int SharesNavigatorTreeModel::rowCount(const QModelIndex& parent) const
{
        SharesNavigatorTreeItem* parentItem;
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
                parentItem = static_cast<SharesNavigatorTreeItem*>(parent.internalPointer());
        }
        return parentItem->childCount();
}
void SharesNavigatorTreeModel::setupModelData(const QStringList& lines, SharesNavigatorTreeItem* parent)
{
        QList<SharesNavigatorTreeItem*> parents;
        QList<int> indentations;
        parents << parent;
        indentations << 0;
        int number = 0;
        while(number < lines.count())
        {
                int position = 0;
                while(position < lines[number].length())
                {
                        if(lines[number].mid(position, 1) != " ")
                        {
                                break;
                        }
                        position++;
                }
                QString lineData = lines[number].mid(position).trimmed();
                if(!lineData.isEmpty())
                {
                        // Read the column data from the rest of the line.
                        QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
                        QList<QVariant> columnData;
                        for(int column = 0; column < columnStrings.count(); ++column)
                        {
                                columnData << columnStrings[column];
                        }
                        if(position > indentations.last())
                        {
                                // The last child of the current parent is now the new parent
                                // unless the current parent has no children.
                                if(parents.last()->childCount() > 0)
                                {
                                        parents << parents.last()->child(parents.last()->childCount() - 1);
                                        indentations << position;
                                }
                        }
                        else
                        {
                                while(position < indentations.last() && parents.count() > 0)
                                {
                                        parents.pop_back();
                                        indentations.pop_back();
                                }
                        }
                        // Append a new item to the current parent's list of children.
                        parents.last()->appendChild(new SharesNavigatorTreeItem(columnData, parents.last()));
                }
                number++;
        }
}
void SharesNavigatorTreeModel::clear()
{
        beginRemoveRows(QModelIndex(), 0, rootItem->childCount());
        //qDebug() << "clearSearch passing to rootItem";
        rootItem->clearChildren();
        endRemoveRows();
        QModelIndex idx1 = index(0, 0, QModelIndex());
        QModelIndex idx2 = index(rootItem->childCount(), 10, QModelIndex());
        emit dataChanged(idx1, idx2);
}
void SharesNavigatorTreeModel::addFolder()
{
        
}
SharesNavigatorTreeItem::SharesNavigatorTreeItem(const QList<QVariant> &data, SharesNavigatorTreeItem* parent)
{
        parentItem = parent;
        itemData = data;
}
SharesNavigatorTreeItem::~SharesNavigatorTreeItem()
{
        qDeleteAll(childItems);
}
void SharesNavigatorTreeItem::appendChild(SharesNavigatorTreeItem* item)
{
        childItems.append(item);
}
void SharesNavigatorTreeItem::clearChildren()
{
        childItems.clear();
}
SharesNavigatorTreeItem* SharesNavigatorTreeItem::child(int row)
{
        return childItems.value(row);
}
int SharesNavigatorTreeItem::childCount() const
{
        return childItems.count();
}
int SharesNavigatorTreeItem::columnCount() const
{
        return itemData.count();
}
int SharesNavigatorTreeItem::find(SharesNavigatorTreeItem* containerItem)
{
        
        return -1;
}
QVariant SharesNavigatorTreeItem::data(int column) const
{
        return itemData.value(column);
}
SharesNavigatorTreeItem* SharesNavigatorTreeItem::parent()
{
        return parentItem;
}
int SharesNavigatorTreeItem::row() const
{
        if(parentItem)
        {
                return parentItem->childItems.indexOf(const_cast<SharesNavigatorTreeItem*>(this));
        }
        return 0;
}
void SharesNavigatorTreeItem::updateHitCount(int count)
{
        itemData[5] = count;
}
bool SharesNavigatorTreeItem::duplicateCheck(SharesNavigatorTreeItem* containerItem, QString ip)
{
        for(int index = 0; index < containerItem->childItems.size(); ++index)
        {
                if(containerItem->child(index)->data(5).toString() == ip)
                {
                        return true;
                }
        }
        return false;
}

