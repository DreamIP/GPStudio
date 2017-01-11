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

#include "model_treeconnect.h"

#include <QDebug>

ModelTreeConnect::ModelTreeConnect()
{
}

ModelTreeConnect::ModelTreeConnect(const ModelTreeConnect &modelTreeConnect)
{
    _toblock = modelTreeConnect._toblock;
    _toflow = modelTreeConnect._toflow;
    _size = modelTreeConnect._size;
    _order = modelTreeConnect._order;
    _muxname = modelTreeConnect._muxname;

    for(int i=0; i<modelTreeConnect._treeitems.size(); i++)
        addTreeItem(new ModelTreeItem(*modelTreeConnect._treeitems[i]));
}

ModelTreeConnect::~ModelTreeConnect()
{
    for(int i=0; i<_treeitems.size(); i++)
        delete _treeitems[i];
}

const QString &ModelTreeConnect::toblock() const
{
    return _toblock;
}

void ModelTreeConnect::setToblock(const QString &toblock)
{
    _toblock = toblock;
}

const QString &ModelTreeConnect::toflow() const
{
    return _toflow;
}

void ModelTreeConnect::setToflow(const QString &toflow)
{
    _toflow = toflow;
}

uint ModelTreeConnect::size() const
{
    return _size;
}

void ModelTreeConnect::setSize(const uint &size)
{
    _size = size;
}

const QString &ModelTreeConnect::order() const
{
    return _order;
}

void ModelTreeConnect::setOrder(const QString &order)
{
    _order = order;
}

const QString &ModelTreeConnect::muxname() const
{
    return _muxname;
}

void ModelTreeConnect::setMuxname(const QString &muxname)
{
    _muxname = muxname;
}

QList<ModelTreeItem *> &ModelTreeConnect::treeitems()
{
    return _treeitems;
}

const QList<ModelTreeItem *> &ModelTreeConnect::treeitems() const
{
    return _treeitems;
}

void ModelTreeConnect::addTreeItem(ModelTreeItem *treeItem)
{
    _treeitems.append(treeItem);
}

void ModelTreeConnect::addTreeItems(const QList<ModelTreeItem *> &treeItems)
{
    foreach (ModelTreeItem *treeItem, treeItems)
    {
        addTreeItem(treeItem);
    }
}

ModelTreeConnect *ModelTreeConnect::fromNodeGenerated(const QDomElement &domElement)
{
    ModelTreeConnect *treeConnect = new ModelTreeConnect();
    bool ok = false;

    treeConnect->setToblock(domElement.attribute("toblock",""));
    treeConnect->setToflow(domElement.attribute("toflow",""));

    int size = domElement.attribute("size","0").toInt(&ok);
    if(ok && size>=0)
        treeConnect->setSize(size);
    else
        treeConnect->setSize(0);

    treeConnect->setOrder(domElement.attribute("order",""));
    treeConnect->setMuxname(domElement.attribute("muxname",""));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="tree_items")
                treeConnect->addTreeItems(ModelTreeItem::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return treeConnect;
}

QList<ModelTreeConnect *> ModelTreeConnect::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelTreeConnect *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="tree_connect")
                list.append(ModelTreeConnect::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
