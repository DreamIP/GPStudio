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

#include "libitem.h"

#include "lib_parser/blocklib.h"

LibItem::LibItem()
{
    _blocklib = NULL;
}

LibItem::LibItem(const BlockLib *blocklib)
{
    _blocklib = blocklib;
    _label = blocklib->name();
}

LibItem::LibItem(const QString &label)
{
    _blocklib = NULL;
    _label = label;
}

LibItem::~LibItem()
{
    foreach (LibItem *item, _childrens)
        delete item;
}

const BlockLib *LibItem::blocklib() const
{
    return _blocklib;
}

bool LibItem::isCateg() const
{
    return (_blocklib == NULL);
}

QString LibItem::label() const
{
    return _label;
}

const QList<LibItem*> &LibItem::childrens() const
{
    return _childrens;
}

LibItem *LibItem::children(int i) const
{
    return _childrens[i];
}

LibItem *LibItem::children(const QString &label) const
{
    QMap<QString, LibItem*>::const_iterator localConstFind = _childrensLabel.constFind(label);
    if(localConstFind!=_childrensLabel.constEnd())
        return *localConstFind;
    return NULL;
}

LibItem *LibItem::path(const QString &path) const
{
    if(path.isEmpty())
        return (LibItem *)this;
    int index = path.indexOf("/");
    if(index==-1)
        return children(path);
    if(_childrensLabel.contains(path.left(index)))
        return children(path.left(index))->path(path.mid(index+1));
    return NULL;
}

LibItem *LibItem::createPath(const QString &path)
{
    if(path.isEmpty())
        return (LibItem *)this;
    int index = path.indexOf("/");
    if(index==-1)
    {
        LibItem *item = children(path);
        if(item)
            return item;
        else
            return append(path);
    }

    LibItem *item = children(path.left(index));
    if(item)
        return item;
    else
        return append(path.left(index))->createPath(path.mid(index+1));
}

int LibItem::count() const
{
    return _childrens.count();
}

LibItem *LibItem::append(const BlockLib *blocklib)
{
    LibItem *item = new LibItem(blocklib);
    item->_parent = this;
    item->_row = _childrens.count();

    _childrens.append(item);
    _childrensLabel.insert(item->label(), item);
    return item;
}

LibItem *LibItem::append(const QString &label)
{
    LibItem *item = new LibItem(label);
    item->_parent = this;
    item->_row = _childrens.count();

    _childrens.append(item);
    _childrensLabel.insert(item->label(), item);
    return item;
}

void LibItem::clear()
{
    foreach (LibItem *item, _childrens)
        delete item;
    _childrens.clear();
}

int LibItem::row()
{
    return _row;
}

LibItem *LibItem::parent() const
{
    return _parent;
}

QMap<QString, LibItem *> LibItem::childrensLabel() const
{
    return _childrensLabel;
}
