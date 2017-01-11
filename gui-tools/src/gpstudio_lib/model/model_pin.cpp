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

#include "model_pin.h"

#include "model_block.h"

ModelPin::ModelPin(ModelBlock *parent)
    : _parent(parent)
{
    _parent = NULL;
}

ModelPin::ModelPin(const ModelPin &modelPin)
{
    _parent = NULL;

    _name = modelPin._name;
    _mapTo = modelPin._mapTo;

    for(int i=0; i<modelPin._attributes.size(); i++)
        addAttribute(new ModelAttribute(*modelPin._attributes[i]));
}

ModelPin::~ModelPin()
{
    for(int i=0; i<_attributes.size(); i++)
        delete _attributes[i];
}

const QString &ModelPin::name() const
{
    return _name;
}

void ModelPin::setName(const QString &name)
{
    _name = name;
}

const QString &ModelPin::mapTo() const
{
    return _mapTo;
}

void ModelPin::setMapTo(const QString &mapTo)
{
    _mapTo = mapTo;
}

ModelBlock *ModelPin::parent() const
{
    return _parent;
}

void ModelPin::setParent(ModelBlock *parent)
{
    _parent = parent;
}

QList<ModelAttribute *> &ModelPin::attributes()
{
    return _attributes;
}

const QList<ModelAttribute *> &ModelPin::attributes() const
{
    return _attributes;
}

void ModelPin::addAttribute(ModelAttribute *attribute)
{
    _attributes.append(attribute);
}

ModelPin *ModelPin::fromNodeGenerated(const QDomElement &domElement)
{
    ModelPin *pin=new ModelPin();

    pin->setName(domElement.attribute("name","no_name"));
    pin->setMapTo(domElement.attribute("mapto",""));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="attributes")
                pin->_attributes.append(ModelAttribute::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return pin;
}

QList<ModelPin *> ModelPin::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelPin *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="pin")
                list.append(ModelPin::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
