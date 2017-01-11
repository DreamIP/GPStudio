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

#include "model_treeitem.h"

#include <QDebug>

ModelTreeItem::ModelTreeItem()
{
}

ModelTreeItem::~ModelTreeItem()
{
}

const QString &ModelTreeItem::fromblock() const
{
    return _fromblock;
}

void ModelTreeItem::setFromblock(const QString &fromblock)
{
    _fromblock = fromblock;
}

const QString &ModelTreeItem::fromflow() const
{
    return _fromflow;
}

void ModelTreeItem::setFromflow(const QString &fromflow)
{
    _fromflow = fromflow;
}

uint ModelTreeItem::size() const
{
    return _size;
}

void ModelTreeItem::setSize(const uint &size)
{
    _size = size;
}

const QString &ModelTreeItem::order() const
{
    return _order;
}

void ModelTreeItem::setOrder(const QString &order)
{
    _order = order;
}

uint ModelTreeItem::muxvalue() const
{
    return _muxvalue;
}

void ModelTreeItem::setMuxvalue(const uint &muxvalue)
{
    _muxvalue = muxvalue;
}

ModelTreeItem *ModelTreeItem::fromNodeGenerated(const QDomElement &domElement)
{
    ModelTreeItem *treeItem = new ModelTreeItem();
    bool ok = false;

    treeItem->setFromblock(domElement.attribute("fromblock",""));
    treeItem->setFromflow(domElement.attribute("fromflow",""));

    int size = domElement.attribute("size","0").toInt(&ok);
    if(ok && size>=0)
        treeItem->setSize(size);
    else
        treeItem->setSize(0);

    treeItem->setOrder(domElement.attribute("order",""));

    int muxvalue = domElement.attribute("muxvalue","0").toInt(&ok);
    if(ok && muxvalue>=0)
        treeItem->setMuxvalue(muxvalue);
    else
        treeItem->setMuxvalue(0);

    return treeItem;
}

QList<ModelTreeItem *> ModelTreeItem::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelTreeItem *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="tree_item")
                list.append(ModelTreeItem::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
