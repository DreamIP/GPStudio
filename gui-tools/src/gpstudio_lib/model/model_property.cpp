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

#include "model_property.h"

#include <QDebug>

ModelProperty::ModelProperty()
{
    _parent = NULL;
    _parentProperty = NULL;
}

ModelProperty::ModelProperty(const ModelProperty &modelProperty)
{
    _parent = NULL;
    _parentProperty = NULL;

    _name = modelProperty._name;
    _caption = modelProperty._caption;
    _type = modelProperty._type;
    _value = modelProperty._value;
    _min = modelProperty._min;
    _max = modelProperty._max;
    _step = modelProperty._step;
    _assert = modelProperty._assert;
    _onchange = modelProperty._onchange;
    _propertymap = modelProperty._propertymap;
    _description = modelProperty._description;

    for(int i=0; i<modelProperty._properties.size(); i++)
        addProperty(new ModelProperty(*modelProperty._properties[i]));
    for(int i=0; i<modelProperty._propertyEnums.size(); i++)
        addPropertyEnum(new ModelPropertyEnum(*modelProperty._propertyEnums[i]));
}

ModelProperty::~ModelProperty()
{
    for(int i=0; i<_properties.size(); i++)
        delete _properties[i];
    for(int i=0; i<_propertyEnums.size(); i++)
        delete _propertyEnums[i];
}

const QString &ModelProperty::name() const
{
    return _name;
}

void ModelProperty::setName(const QString &name)
{
    _name = name;
}

const QString &ModelProperty::caption() const
{
    return _caption;
}

void ModelProperty::setCaption(const QString &caption)
{
    _caption = caption;
}

const QString &ModelProperty::type() const
{
    return _type;
}

void ModelProperty::setType(const QString &type)
{
    _type = type;
}

const QVariant &ModelProperty::value() const
{
    return _value;
}

void ModelProperty::setValue(const QVariant &value)
{
    _value = value;
}

const QString &ModelProperty::min() const
{
    return _min;
}

void ModelProperty::setMin(const QString &min)
{
    _min = min;
}

const QString &ModelProperty::max() const
{
    return _max;
}

void ModelProperty::setMax(const QString &max)
{
    _max = max;
}

const QString &ModelProperty::step() const
{
    return _step;
}

void ModelProperty::setStep(const QString &step)
{
    _step = step;
}

const QString &ModelProperty::assert() const
{
    return _assert;
}

void ModelProperty::setAssert(const QString &assert)
{
    _assert = assert;
}

const QString &ModelProperty::propertymap() const
{
    return _propertymap;
}

void ModelProperty::setPropertymap(const QString &propertymap)
{
    _propertymap = propertymap;
}

const QString &ModelProperty::onchange() const
{
    return _onchange;
}

void ModelProperty::setOnchange(const QString &onchange)
{
    _onchange = onchange;
}

const QString &ModelProperty::description() const
{
    return _description;
}

void ModelProperty::setDescription(const QString &description)
{
    _description = description;
}

ModelBlock *ModelProperty::parent() const
{
    return _parent;
}

void ModelProperty::setParent(ModelBlock *parent)
{
    _parent = parent;
    foreach (ModelProperty *property, _properties)
    {
        property->setParent(_parent);
    }
}

QList<ModelProperty *> &ModelProperty::properties()
{
    return _properties;
}

const QList<ModelProperty *> &ModelProperty::properties() const
{
    return _properties;
}

void ModelProperty::addProperty(ModelProperty *property)
{
    property->_parentProperty = this;
    _properties.append(property);
    _propertiesMap.insert(property->name(), property);
}

void ModelProperty::addProperty(QList<ModelProperty *> properties)
{
    foreach (ModelProperty *property, properties)
        addProperty(property);
}

ModelProperty *ModelProperty::getProperty(const QString &name) const
{
    QMap<QString, ModelProperty*>::const_iterator localConstFind = _propertiesMap.constFind(name);
    if(localConstFind!=_propertiesMap.constEnd())
        return *localConstFind;
    return NULL;
}

ModelProperty *ModelProperty::getPropertyPath(const QString &path) const
{
    if(path.isEmpty())
        return (ModelProperty *)this;
    int index = path.indexOf(".");
    if(index==-1)
    {
        if(_propertiesMap.contains(path))
            return _propertiesMap[path];
        else return NULL;
    }
    if(_propertiesMap.contains(path.left(index)))
        return _propertiesMap[path.left(index)]->getPropertyPath(path.mid(index+1));
    return NULL;
}

QString ModelProperty::getPath() const
{
    if(_parent)
        return QString();

    if(_parentProperty)
        return _parentProperty->getPath()+"."+_name;

    return _name;
}

QList<ModelPropertyEnum *> &ModelProperty::propertyEnums()
{
    return _propertyEnums;
}

const QList<ModelPropertyEnum *> &ModelProperty::propertyEnums() const
{
    return _propertyEnums;
}

void ModelProperty::addPropertyEnum(ModelPropertyEnum *propertyEnum)
{
    _propertyEnums.append(propertyEnum);
}

void ModelProperty::addPropertyEnum(QList<ModelPropertyEnum *> propertyEnums)
{
    foreach (ModelPropertyEnum *propertyEnum, propertyEnums)
        addPropertyEnum(propertyEnum);
}

ModelProperty *ModelProperty::fromNodeGenerated(const QDomElement &domElement)
{
    ModelProperty *blockProperty = new ModelProperty();

    blockProperty->setName(domElement.attribute("name","no_name"));
    blockProperty->setCaption(domElement.attribute("caption",blockProperty->name()));
    if(blockProperty->caption().isEmpty()) blockProperty->setCaption(blockProperty->name());
    blockProperty->setType(domElement.attribute("type",""));
    blockProperty->setValue(domElement.attribute("value",""));
    blockProperty->setMin(domElement.attribute("min",""));
    blockProperty->setMax(domElement.attribute("max",""));
    blockProperty->setStep(domElement.attribute("step","1"));
    blockProperty->setAssert(domElement.attribute("assert",""));
    blockProperty->setPropertymap(domElement.attribute("propertymap",""));
    blockProperty->setOnchange(domElement.attribute("onchange",""));
    blockProperty->setDescription(domElement.attribute("desc",""));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="properties")
                blockProperty->addProperty(ModelProperty::listFromNodeGenerated(e));
            if(e.tagName()=="enums")
                blockProperty->addPropertyEnum(ModelPropertyEnum::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return blockProperty;
}

QList<ModelProperty *> ModelProperty::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelProperty *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="property")
                list.append(ModelProperty::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelProperty::toXMLElement(QDomDocument &doc)
{
    QDomElement element = doc.createElement("property");

    element.setAttribute("name", _name);
    element.setAttribute("value", _value.toString());

    QDomElement propertyList = doc.createElement("properties");
    foreach (ModelProperty *property, _properties)
    {
        propertyList.appendChild(property->toXMLElement(doc));
    }
    if(!propertyList.childNodes().isEmpty())
        element.appendChild(propertyList);

    return element;
}
