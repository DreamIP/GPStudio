/****************************************************************************
** Copyright (C) 2014-2017 Dream IP
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

#include "cameraitem.h"

#include "camera/camera.h"
#include "cameracom.h"

#include "cameraitemmodel.h"

#include "model/model_gpviewer.h"
#include "model/model_viewer.h"
#include "model/model_viewerflow.h"

#include <QIcon>

CameraItem::CameraItem()
{
    _type = NullType;
    _ptr = NULL;
    _parent = NULL;
}

CameraItem::CameraItem(const Camera *camera, uint filter)
{
    _type = CameraType;
    _ptr = camera;

    if(filter & CameraItem::FBlock)
    {
        foreach (Block *block, camera->blocks())
        {
            append(block);
        }
    }
}

CameraItem::CameraItem(const ModelNode *node, uint filter)
{
    _type = ModelNodeType;
    _ptr = node;

    if(filter & CameraItem::FBlock)
    {
        foreach (ModelBlock *block, node->blocks())
        {
            append(block);
        }
    }
}

CameraItem::CameraItem(const Block *block, uint filter)
{
    _type = BlockType;
    _ptr = block;

    if(!block->flows().empty())
    {
        foreach (Flow *flow, block->flows())
        {
            if((filter & CameraItem::FFlowIn && flow->type()==Flow::Input) || (filter & CameraItem::FFlowOut && flow->type()==Flow::Output))
                append(flow);
        }
    }
}

CameraItem::CameraItem(const ModelBlock *block, uint filter)
{
    _type = ModelBlockType;
    _ptr = block;

    if(!block->flows().empty())
    {
        foreach (ModelFlow *flow, block->flows())
        {
            if((filter & CameraItem::FFlowIn && flow->type()=="in") || (filter & CameraItem::FFlowOut && flow->type()=="out"))
                append(flow);
        }
    }
}

CameraItem::CameraItem(const Flow *flow, uint filter)
{
    Q_UNUSED(filter)
    _type = FlowType;
    _ptr = flow;
}

CameraItem::CameraItem(const ModelFlow *flow, uint filter)
{
    Q_UNUSED(filter)
    _type = ModelFlowType;
    _ptr = flow;
}

CameraItem::CameraItem(const ModelGPViewer *gpViewer, uint filter)
{
    Q_UNUSED(filter)
    _type = ModelGPViewerType;
    _ptr = gpViewer;
    foreach (ModelViewer *viewer, gpViewer->viewers())
    {
        append(viewer);
    }
}

CameraItem::CameraItem(const ModelViewer *viewer, uint filter)
{
    Q_UNUSED(filter)
    _type = ModelViewerType;
    _ptr = viewer;
    foreach (ModelViewerFlow *viewer, viewer->viewerFlows())
    {
        append(viewer);
    }
}

CameraItem::CameraItem(const ModelViewerFlow *viewerFlow, uint filter)
{
    Q_UNUSED(filter)
    _type = ModelViewerFlowType;
    _ptr = viewerFlow;
}

CameraItem::~CameraItem()
{
    foreach (CameraItem *item, _childrens)
        delete item;
}

CameraItem::Type CameraItem::type() const
{
    return _type;
}

void CameraItem::setType(const Type &type)
{
    _type = type;
}

const Camera *CameraItem::camera() const
{
    if(_type==CameraType)
        return static_cast<const Camera*>(_ptr);
    return NULL;
}

const Block *CameraItem::block() const
{
    if(_type==BlockType)
        return static_cast<const Block*>(_ptr);
    return NULL;
}

const Flow *CameraItem::flow() const
{
    if(_type==FlowType)
        return static_cast<const Flow*>(_ptr);
    return NULL;
}

const ModelNode *CameraItem::modelNode() const
{
    if(_type==ModelNodeType)
        return static_cast<const ModelNode*>(_ptr);
    return NULL;
}

const ModelBlock *CameraItem::modelBlock() const
{
    if(_type==ModelBlockType)
        return static_cast<const ModelBlock*>(_ptr);
    return NULL;
}

const ModelFlow *CameraItem::modelFlow() const
{
    if(_type==ModelFlowType)
        return static_cast<const ModelFlow*>(_ptr);
    return NULL;
}

const ModelGPViewer *CameraItem::modelGPViewer() const
{
    if(_type==ModelGPViewerType)
        return static_cast<const ModelGPViewer*>(_ptr);
    return NULL;
}

const ModelViewer *CameraItem::modelViewer() const
{
    if(_type==ModelViewerType)
        return static_cast<const ModelViewer*>(_ptr);
    return NULL;
}

const ModelViewerFlow *CameraItem::modelViewerFlow() const
{
    if(_type==ModelViewerFlowType)
        return static_cast<const ModelViewerFlow*>(_ptr);
    return NULL;
}

const QList<CameraItem*> &CameraItem::childrens() const
{
    return _childrens;
}

const CameraItem *CameraItem::children(int i) const
{
    return _childrens[i];
}

CameraItem *CameraItem::children(const QString &name)
{
    for(int i=0; i<_childrens.size(); i++)
    {
        CameraItem *item = _childrens.at(i);
        if(item->value(0)==name)
            return item;
    }
    return NULL;
}

int CameraItem::count() const
{
    return _childrens.count();
}

CameraItem *CameraItem::append(const Camera *camera, uint filter)
{
    CameraItem *item = new CameraItem(camera, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const ModelNode *node, uint filter)
{
    CameraItem *item = new CameraItem(node, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const Block *block, uint filter)
{
    CameraItem *item = new CameraItem(block, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const ModelBlock *block, uint filter)
{
    CameraItem *item = new CameraItem(block, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const Flow *flow, uint filter)
{
    CameraItem *item = new CameraItem(flow, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const ModelFlow *flow, uint filter)
{
    CameraItem *item = new CameraItem(flow, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const ModelGPViewer *gpViewer, uint filter)
{
    CameraItem *item = new CameraItem(gpViewer, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const ModelViewer *viewer, uint filter)
{
    Q_UNUSED(filter)
    CameraItem *item = new CameraItem(viewer, filter);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

CameraItem *CameraItem::append(const ModelViewerFlow *viewerFlow, uint filter)
{
    Q_UNUSED(filter)
    CameraItem *item = new CameraItem(viewerFlow);
    item->_parent = this;
    item->_row = _childrens.count();
    _childrens.append(item);
    return item;
}

bool CameraItem::insertRow(int row, CameraItem *item)
{
    QList<CameraItem *> items;
    items.append(item);
    return insertRow(row, items);
}

bool CameraItem::insertRow(int row, QList<CameraItem *> items)
{
    if(row > _childrens.count())
        return false;

    for(int i=0; i<items.count(); i++)
        _childrens.insert(row, items[i]);
    for(int i=row; i<_childrens.count(); i++)
    {
        _childrens[i]->_parent = this;
        _childrens[i]->_row = i;
    }

    return true;
}

bool CameraItem::removeRows(int row, int count)
{
    if(row + count > _childrens.count())
        return false;

    for(int i=row; i<row+count; i++)
    {
        delete _childrens[i];
        _childrens.removeAt(row);
    }
    for(int i=row; i<_childrens.count(); i++)
        _childrens[i]->_row = i;

    return true;
}

void CameraItem::clear()
{
    _childrens.clear();
}

int CameraItem::row()
{
    return _row;
}

void *CameraItem::ptr() const
{
    return (void*)(this);
}

CameraItem *CameraItem::parent() const
{
    return _parent;
}

QVariant CameraItem::value(int column, int role) const
{
    switch (type())
    {
    case CameraItem::CameraType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                if(camera()->node())
                    return QVariant(camera()->node()->name());
                else
                    return QVariant();
            case CameraItemModel::Value:
                if(camera()->isConnected())
                    return QVariant(camera()->com()->info().name());
                else
                    return QVariant("Not connected");
            default:
                return QVariant();
            }
        case Qt::EditRole:
            switch (column)
            {
            case CameraItemModel::Name:
                if(camera()->node())
                    return QVariant(camera()->node()->name());
                else
                    return QVariant();
            default:
                return QVariant();
            }
        case Qt::DecorationRole:
            if(column==0)
            {
                return QIcon(":/icons/img/usb.png");
            }
        }
        return QVariant();
    case CameraItem::ModelNodeType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelNode()->name());
            case CameraItemModel::Value:
                return QVariant("");
            default:
                return QVariant();
            }
        case Qt::EditRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelNode()->name());
            default:
                return QVariant();
            }
        case Qt::DecorationRole:
            if(column==0)
            {
                return QIcon(":/icons/img/usb.png");
            }
        }
        return QVariant();
    case CameraItem::BlockType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(block()->name());
            default:
                return QVariant(block()->modelBlock()->driver());
            }
        case Qt::EditRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(block()->name());
            default:
                return QVariant();
            }
        }
        return QVariant();
    case CameraItem::ModelBlockType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelBlock()->name());
            default:
                return QVariant(modelBlock()->driver());
            }
        case Qt::EditRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelBlock()->name());
            default:
                return QVariant();
            }
        }
        return QVariant();
    case CameraItem::FlowType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(flow()->name());
            case CameraItemModel::Value:
                return QVariant(flow()->type()==Flow::Input ? "in" : "out");
            default:
                return QVariant();
            }
        case Qt::DecorationRole:
            if(column==0)
            {
                if(flow()->type()==Flow::Input)
                    return QIcon(":/icons/img/flow-in.png");
                else
                    return QIcon(":/icons/img/flow-out.png");
            }
        }
        return QVariant();
    case CameraItem::ModelFlowType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelFlow()->name());
            case CameraItemModel::Value:
                return QVariant(modelFlow()->type());
            default:
                return QVariant();
            }
        case Qt::DecorationRole:
            if(column==0)
            {
                if(modelFlow()->type()=="in")
                    return QIcon(":/icons/img/flow-in.png");
                else
                    return QIcon(":/icons/img/flow-out.png");
            }
        }
        return QVariant();
    case CameraItem::ModelGPViewerType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelGPViewer()->name());
            default:
                return QVariant();
            }
        }
        return QVariant();
    case CameraItem::ModelViewerType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelViewer()->name());
            default:
                return QVariant();
            }
        case Qt::EditRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelViewer()->name());
            default:
                return QVariant();
            }
        }
        return QVariant();
    case CameraItem::ModelViewerFlowType:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case CameraItemModel::Name:
                return QVariant(modelViewerFlow()->flowName());
            default:
                return QVariant();
            }
        }
        return QVariant();
    default:
        return QVariant();
    }
}

Qt::ItemFlags CameraItem::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if(index.column() == 0)
    {
        switch (type())
        {
        case CameraItem::ModelViewerType:
        case CameraItem::ModelFlowType:
        case CameraItem::FlowType:
            return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable | defaultFlags;

        case CameraItem::ModelNodeType:
        case CameraItem::CameraType:
            return Qt::ItemIsEditable | defaultFlags;

        case CameraItem::ModelBlockType:
            if(modelBlock()->isIO())
                return defaultFlags;
            else
                return Qt::ItemIsEditable | defaultFlags;

        case CameraItem::BlockType:
            if(block()->modelBlock()->isIO())
                return defaultFlags;
            else
                return Qt::ItemIsEditable | defaultFlags;
        default:
            return defaultFlags;
        }
    }
    else
        return defaultFlags;
}
