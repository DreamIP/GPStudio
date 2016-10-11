/****************************************************************************
** Copyright (C) 2016 Dream IP
** 
** This file is part of GPStudio.
**
** GPStudio is a free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "libitemmodel.h"

LibItemModel::LibItemModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    _rootItem = new LibItem("lib");
}

int LibItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant LibItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Vertical) return QVariant();
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case Name:
            return QVariant("Name");
        case Description:
            return QVariant("Description");
        default:
            return QVariant();
        }
    break;
    }
    return QVariant();
}

QVariant LibItemModel::data(const QModelIndex &index, int role) const
{
    if(_rootItem->count()==0)
        return QVariant();

    LibItem *libItem = static_cast<LibItem*>(index.internalPointer());
    const BlockLib *processLib = libItem->blocklib();

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case Name:
            return QVariant(libItem->label());
        case Description:
            if(libItem->isCateg())
                return QVariant();
            else
                return QVariant(processLib->description());
        default:
            return QVariant();
        }
    case Qt::DecorationRole:
        if(index.column()==Name)
            if(libItem->isCateg())
                return QVariant();
            else
                return QVariant(processLib->icon());
        break;
    case Qt::ToolTipRole:
        if(libItem->isCateg())
            return QVariant();
        else
            return QVariant(processLib->description());
    case Qt::UserRole:
        return index.row();
    }
    return QVariant();
}

QModelIndex LibItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    LibItem *parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<LibItem*>(parent.internalPointer());

    const LibItem *childItem = parentItem->children(row);
    if (childItem)
        return createIndex(row, column, (void*)childItem);
    else
        return QModelIndex();
}

QModelIndex LibItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalPointer()==NULL)
        return QModelIndex();

    LibItem *childItem = static_cast<LibItem*>(child.internalPointer());
    LibItem *parentItem = childItem->parent();

    if (parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int LibItemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
            return 0;

    if(!parent.isValid()) // root item
    {
        return _rootItem->count();
    }
    else
    {
        LibItem *item = static_cast<LibItem*>(parent.internalPointer());
        if(item)
            return item->count();
    }
    return 0;
}

Qt::ItemFlags LibItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

void LibItemModel::setLib(const Lib *lib)
{
    emit layoutAboutToBeChanged();

    _rootItem->clear();
    foreach(BlockLib *processLib, lib->processes())
    {
        LibItem *libItem;
        QString categ = processLib->categ();
        if(categ.isEmpty())
            categ = "various";
        libItem = _rootItem->children(categ);
        if(!libItem)
            libItem = _rootItem->append(categ);
        libItem->append(processLib);
    }

    emit layoutChanged();
}

const BlockLib *LibItemModel::blockLib(const QModelIndex &index)
{
    if(!index.isValid())
        return NULL;

    LibItem *libItem = static_cast<LibItem*>(index.internalPointer());
    if(libItem->isCateg())
        return NULL;

    const BlockLib *processLib = libItem->blocklib();
    return processLib;
}
