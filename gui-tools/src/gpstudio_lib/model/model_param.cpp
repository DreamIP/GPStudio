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

#include "model_param.h"

#include "model_block.h"

ModelParam::ModelParam(ModelBlock *parent)
    : _parent(parent)
{
    _parent = NULL;
}

ModelParam::ModelParam(const ModelParam &modelParam)
{
    _parent = NULL;

    _name = modelParam._name;
    _type = modelParam._type;
    _regAddr = modelParam._regAddr;
    _value = modelParam._value;
    _defaultValue = modelParam._defaultValue;
    _min = modelParam._min;
    _max = modelParam._max;
    _hard = modelParam._hard;
    _description = modelParam._description;
    _propertyMap = modelParam._propertyMap;

    for(int i=0; i<modelParam._parambitfields.size(); i++)
        addParamBitField(new ModelParamBitField(*modelParam._parambitfields[i]));
}

ModelParam::~ModelParam()
{
    for(int i=0; i<_parambitfields.size(); i++)
        delete _parambitfields[i];
}

const QString &ModelParam::name() const
{
    return _name;
}

void ModelParam::setName(const QString &name)
{
    _name = name;
}

const QString &ModelParam::type() const
{
    return _type;
}

void ModelParam::setType(const QString &type)
{
    _type = type;
}

qint32 ModelParam::regAddr() const
{
    return _regAddr;
}

void ModelParam::setRegAddr(const qint32 &regAddr)
{
    _regAddr = regAddr;
}

qint32 ModelParam::absoluteAddr() const
{
    if(_parent)
        return _parent->addrAbs()+_regAddr;
    return _regAddr;
}

const QVariant &ModelParam::value() const
{
    return _value;
}

void ModelParam::setValue(const QVariant &value)
{
    _value = value;
}

const QVariant &ModelParam::defaultValue() const
{
    return _defaultValue;
}

void ModelParam::setDefaultValue(const QVariant &defaultValue)
{
    _defaultValue = defaultValue;
}

const QVariant &ModelParam::min() const
{
    return _min;
}

void ModelParam::setMin(const QVariant &min)
{
    _min = min;
}

const QVariant &ModelParam::max() const
{
    return _max;
}

void ModelParam::setMax(const QVariant &max)
{
    _max = max;
}

bool ModelParam::isHard() const
{
    return _hard;
}

void ModelParam::setHard(bool hard)
{
    _hard = hard;
}

const QString &ModelParam::propertyMap() const
{
    return _propertyMap;
}

void ModelParam::setPropertyMap(const QString &propertyMap)
{
    _propertyMap = propertyMap;
}

const QString &ModelParam::description() const
{
    return _description;
}

void ModelParam::setDescription(const QString &description)
{
    _description = description;
}

ModelBlock *ModelParam::parent() const
{
    return _parent;
}

void ModelParam::setParent(ModelBlock *parent)
{
    _parent = parent;
}

bool ModelParam::isDynamicParam() const
{
    return (!_hard && _regAddr>=0);
}

QList<ModelParamBitField *> &ModelParam::paramBitFields()
{
    return _parambitfields;
}

const QList<ModelParamBitField *> &ModelParam::paramBitFields() const
{
    return _parambitfields;
}

void ModelParam::addParamBitField(ModelParamBitField *bitField)
{
    bitField->setParent(this);
    _parambitfields.append(bitField);
}

void ModelParam::addParamBitFields(const QList<ModelParamBitField *> &bitFields)
{
    foreach (ModelParamBitField *bitField, bitFields)
    {
        addParamBitField(bitField);
    }
}

ModelParam *ModelParam::fromNodeGenerated(const QDomElement &domElement)
{
    ModelParam *param = new ModelParam();
    bool ok = false;

    param->setName(domElement.attribute("name","no_name"));
    param->setType(domElement.attribute("type",""));

    int regaddr = domElement.attribute("regaddr","-1").toInt(&ok);
    if(ok)
        param->setRegAddr(regaddr);
    else
        param->setRegAddr(-1);

    int value = domElement.attribute("value","0").toInt(&ok);
    if(ok)
        param->setValue(value);
    else
        param->setValue(0);

    int defaultValue = domElement.attribute("default","0").toInt(&ok);
    if(ok)
        param->setDefaultValue(defaultValue);
    else
        param->setDefaultValue(0);

    int min = domElement.attribute("min","0").toInt(&ok);
    if(ok)
        param->setMin(min);
    else
        param->setMin(0);

    int max = domElement.attribute("max","0").toInt(&ok);
    if(ok)
        param->setMax(max);
    else
        param->setMax(0);

    param->setPropertyMap(domElement.attribute("propertymap",""));

    param->setHard((domElement.attribute("hard","")=="1" || domElement.attribute("hard","")=="true"));

    param->setDescription(domElement.attribute("desc",""));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="bitfields")
                param->addParamBitFields(ModelParamBitField::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return param;
}

QList<ModelParam *> ModelParam::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelParam *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="param")
                list.append(ModelParam::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelParam::toXMLElement(QDomDocument &doc)
{
    QDomElement element = doc.createElement("param");

    element.setAttribute("name", _name);
    element.setAttribute("value", _value.toString());

    return element;
}
