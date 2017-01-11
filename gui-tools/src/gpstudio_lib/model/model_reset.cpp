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

#include "model_reset.h"

#include "model_block.h"

ModelReset::ModelReset(ModelBlock *parent)
    : _parent(parent)
{
    _parent = NULL;
}

ModelReset::~ModelReset()
{
}

const QString &ModelReset::name() const
{
    return _name;
}

void ModelReset::setName(const QString &name)
{
    _name = name;
}

const QString &ModelReset::group() const
{
    return _group;
}

void ModelReset::setGroup(const QString &group)
{
    _group = group;
}

ModelReset::Direction ModelReset::direction() const
{
    return _direction;
}

void ModelReset::setDirection(const ModelReset::Direction &direction)
{
    _direction = direction;
}

const QString &ModelReset::description() const
{
    return _description;
}

void ModelReset::setDescription(const QString &description)
{
    _description = description;
}

ModelBlock *ModelReset::parent() const
{
    return _parent;
}

void ModelReset::setParent(ModelBlock *parent)
{
    _parent = parent;
}

ModelReset *ModelReset::fromNodeGenerated(const QDomElement &domElement)
{
    ModelReset *reset = new ModelReset();

    reset->setName(domElement.attribute("name","no_name"));
    reset->setGroup(domElement.attribute("group",""));

    QString direction = domElement.attribute("direction","");
    if(direction.toLower()=="in")
        reset->setDirection(DirIn);
    else if(direction.toLower()=="out")
        reset->setDirection(DirOut);
    else
        reset->setDirection(DirUndef);

    reset->setDescription(domElement.attribute("desc",""));

    return reset;
}

QList<ModelReset *> ModelReset::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelReset *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="reset")
                list.append(ModelReset::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
