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

#include "model_port.h"

#include "model_block.h"

ModelPort::ModelPort(ModelBlock *parent)
    : _parent(parent)
{
    _parent = NULL;
}

ModelPort::~ModelPort()
{
}

const QString &ModelPort::name() const
{
    return _name;
}

void ModelPort::setName(const QString &name)
{
    _name = name;
}

const QString &ModelPort::type() const
{
    return _type;
}

void ModelPort::setType(const QString &type)
{
    _type = type;
}

quint8 ModelPort::size() const
{
    return _size;
}

void ModelPort::setSize(const quint8 &size)
{
    _size = size;
}

const QString &ModelPort::description() const
{
    return _description;
}

void ModelPort::setDescription(const QString &description)
{
    _description = description;
}

ModelBlock *ModelPort::parent() const
{
    return _parent;
}

void ModelPort::setParent(ModelBlock *parent)
{
    _parent = parent;
}

ModelPort *ModelPort::fromNodeGenerated(const QDomElement &domElement)
{
    ModelPort *port = new ModelPort();
    bool ok = false;

    port->setName(domElement.attribute("name","no_name"));
    port->setType(domElement.attribute("type",""));

    int size = domElement.attribute("size","0").toInt(&ok);
    if(ok && size>=0)
        port->setSize(size);
    else
        port->setSize(0);

    port->setDescription(domElement.attribute("desc",""));

    return port;
}

QList<ModelPort *> ModelPort::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelPort *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="port")
                list.append(ModelPort::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
