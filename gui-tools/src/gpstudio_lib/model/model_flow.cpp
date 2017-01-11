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

#include "model_flow.h"

#include "model_block.h"

#include <QDebug>

ModelFlow::ModelFlow(ModelBlock *parent)
    : _parent(parent)
{
    _parent = NULL;
}

ModelFlow::ModelFlow(const ModelFlow &modelFlow)
{
    _parent = NULL;

    _name = modelFlow._name;
    _type = modelFlow._type;
    _size = modelFlow._size;
    _description = modelFlow._description;

    for(int i=0; i<modelFlow._properties.size(); i++)
        addProperty(new ModelProperty(*modelFlow._properties[i]));
}

ModelFlow::~ModelFlow()
{
    for(int i=0; i<_properties.size(); i++)
        delete _properties[i];
}

const QString &ModelFlow::name() const
{
    return _name;
}

void ModelFlow::setName(const QString &name)
{
    _name = name;
}

const QString &ModelFlow::type() const
{
    return _type;
}

void ModelFlow::setType(const QString &type)
{
    _type = type;
}

quint8 ModelFlow::size() const
{
    return _size;
}

void ModelFlow::setSize(const quint8 &size)
{
    _size = size;
}

const QString &ModelFlow::description() const
{
    return _description;
}

void ModelFlow::setDescription(const QString &description)
{
    _description = description;
}

ModelBlock *ModelFlow::parent() const
{
    return _parent;
}

void ModelFlow::setParent(ModelBlock *parent)
{
    _parent = parent;
    foreach (ModelProperty *property, _properties)
    {
        property->setParent(_parent);
    }
}

QList<ModelProperty *> &ModelFlow::properties()
{
    return _properties;
}

const QList<ModelProperty *> &ModelFlow::properties() const
{
    return _properties;
}

void ModelFlow::addProperty(ModelProperty *property)
{
    property->setParent(_parent);
    _properties.append(property);
}

void ModelFlow::addProperties(const QList<ModelProperty *> &properties)
{
    foreach (ModelProperty *property, properties)
    {
        addProperty(property);
    }
}

ModelProperty *ModelFlow::getBlockProperty(const QString &name) const
{
    for(int i=0; i<this->properties().size(); i++)
    {
        ModelProperty *blockProperty = this->properties().at(i);
        if(blockProperty->name()==name)
            return blockProperty;
    }
    return NULL;
}

ModelFlow *ModelFlow::fromNodeGenerated(const QDomElement &domElement)
{
    ModelFlow *flow = new ModelFlow();
    bool ok = false;

    flow->setName(domElement.attribute("name","no_name"));
    flow->setType(domElement.attribute("type",""));

    int size = domElement.attribute("size","8").toInt(&ok);
    if(ok && size>=0)
        flow->setSize(size);
    else
        flow->setSize(8);

    flow->setDescription(domElement.attribute("desc",""));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="properties")
                flow->addProperties(ModelProperty::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return flow;
}

QList<ModelFlow *> ModelFlow::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelFlow *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="flow")
                list.append(ModelFlow::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelFlow::toXMLElement(QDomDocument &doc)
{
    QDomElement element = doc.createElement("flow");

    element.setAttribute("name", _name);
    element.setAttribute("size", _size);

    return element;
}
