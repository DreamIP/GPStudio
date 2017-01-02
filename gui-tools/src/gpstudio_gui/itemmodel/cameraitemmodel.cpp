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

#include "cameraitemmodel.h"

#include "camera/camera.h"
#include "cameracom.h"

#include "model/model_node.h"
#include "model/model_gpviewer.h"
#include "model/model_viewer.h"

#include <QDebug>
#include <QIcon>
#include <QMimeData>

CameraItemModel::CameraItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    _rootItem = new CameraItem();
}

CameraItemModel::CameraItemModel(Camera *camera, QObject *parent)
    : QAbstractItemModel(parent)
{
    _rootItem = new CameraItem();
    setCamera(camera);
}

QModelIndex CameraItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    CameraItem *parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<CameraItem*>(parent.internalPointer());

    const CameraItem *childItem = parentItem->children(row);
    if (childItem)
        return createIndex(row, column, childItem->ptr());
    else
        return QModelIndex();
}

QModelIndex CameraItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalPointer()==NULL)
        return QModelIndex();

    CameraItem *childItem = static_cast<CameraItem*>(child.internalPointer());
    CameraItem *parentItem = childItem->parent();

    if (parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CameraItemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
            return 0;

    if(!parent.isValid()) // root item
    {
        return _rootItem->count();
    }
    else
    {
        CameraItem *item = static_cast<CameraItem*>(parent.internalPointer());
        if(item)
            return item->count();
    }
    return 0;
}

int CameraItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant CameraItemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(_rootItem->count()==0)
        return QVariant();

    CameraItem *item = static_cast<CameraItem*>(index.internalPointer());

    return item->value(index.column(), role);
}

QVariant CameraItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Vertical) return QVariant();
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case Name:
            return QVariant("Name");
        case Value:
            return QVariant("Value");
        default:
            return QVariant();
        }
        break;
    }
    return QVariant();
}

void CameraItemModel::setCamera(const Camera *camera, uint filter)
{
    beginResetModel();
    resetInternalData();
    _rootItem->clear();
    _rootItem->setType(CameraItem::CameraType);
    _rootItem->append(camera, filter);
    endResetModel();
}

void CameraItemModel::setBlock(const Block *block, uint filter)
{
    beginResetModel();
    resetInternalData();
    _rootItem->clear();
    _rootItem->setType(CameraItem::ModelBlockType);
    _rootItem->append(block, filter);
    endResetModel();
}

void CameraItemModel::setNode(const ModelNode *node, uint filter)
{
    beginResetModel();
    resetInternalData();
    _rootItem->clear();
    _rootItem->setType(CameraItem::ModelNodeType);
    _rootItem->append(node, filter);
    endResetModel();
}

void CameraItemModel::setViewer(const ModelGPViewer *gpViewer, uint filter)
{
    beginResetModel();
    resetInternalData();
    setRootItem(new CameraItem(gpViewer, filter));
    endResetModel();
}

void CameraItemModel::clearAll()
{
    beginResetModel();
    resetInternalData();
    _rootItem->clear();
    endResetModel();
}

const ModelGPViewer *CameraItemModel::gpViewer(const QModelIndex &index) const
{
    CameraItem *item = static_cast<CameraItem*>(index.internalPointer());
    return item->modelGPViewer();
}

const ModelViewer *CameraItemModel::viewer(const QModelIndex &index) const
{
    CameraItem *item = static_cast<CameraItem*>(index.internalPointer());
    return item->modelViewer();
}

const ModelViewerFlow *CameraItemModel::viewerFlow(const QModelIndex &index) const
{
    CameraItem *item = static_cast<CameraItem*>(index.internalPointer());
    return item->modelViewerFlow();
}

Qt::DropActions CameraItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions CameraItemModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags CameraItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
    {
        if(index.column() == 0)
            return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
        else
            return defaultFlags;
    }
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList CameraItemModel::mimeTypes() const
{
    QStringList types;
    types << "flow/flowid";
    return types;
}

QMimeData *CameraItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    foreach (QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            QString text = data(index, Qt::DisplayRole).toString();
            encodedData.append(text+":");
        }
    }

    mimeData->setData("flow/flowid", encodedData);
    return mimeData;
}

bool CameraItemModel::canDropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action)
    Q_UNUSED(row)
    Q_UNUSED(parent)
    if(mimeData->hasFormat("flow/flowid"))
    {
        if(column>0)
            return false;
        return true;
    }

    return true;
}

bool CameraItemModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row)
    Q_UNUSED(column)
    QString viewerName;
    if(action == Qt::IgnoreAction)
        return true;
    if(!mimeData->hasFormat("flow/flowid"))
        return false;

    if(!parent.isValid())
    {
        viewerName = "newViewer";
        emit viewerAdded(viewerName);
        qDebug()<<"Added viewer";
    }
    else
        viewerName = data(parent).toString();

    QStringList flowsName = QString(mimeData->data("flow/flowid")).split(':');
    foreach (QString flowName, flowsName)
    {
        if(!flowName.isEmpty())
        {
            qDebug()<<"Added viewer"<<flowName<<"flow to"<<viewerName;
            emit viewerFlowAdded(viewerName, flowName);
        }
    }

    return true;
}

/*bool CameraItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    CameraItem *parentItem;
    if(!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<CameraItem*>(parent.internalPointer());

    beginInsertRows(parent, row, row + count - 1);
    parentItem->insertRow(row, count);
    endInsertRows();

    return true;
}*/

bool CameraItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    CameraItem *parentItem;
    if(!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<CameraItem*>(parent.internalPointer());

    if(row + count >= parentItem->count())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    parentItem->removeRows(row, count);
    endRemoveRows();

    return true;
}

void CameraItemModel::setRootItem(CameraItem *rootItem)
{
    delete _rootItem;
    _rootItem = rootItem;
}

void CameraItemModel::addViewer(ModelViewer *viewer)
{
    if(_rootItem->type() != CameraItem::ModelGPViewerType)
        return;

    beginInsertRows(QModelIndex(), 0, 0);
    _rootItem->insertRow(0, new CameraItem(viewer));
    endInsertRows();
}

void CameraItemModel::removeViewer(QString viewerName)
{
    QModelIndexList items = match(
                index(0,0,QModelIndex()),
                Qt::DisplayRole,
                QVariant::fromValue(viewerName),
                Qt::MatchExactly || Qt::MatchRecursive);
    if(items.size()>0)
    {
        removeRow(items[0].row(), items[0].parent());
    }
}
