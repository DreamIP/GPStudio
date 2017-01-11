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

#include "model_parambitfield.h"

#include <QRegExp>
#include <QStringList>
#include <QDebug>

ModelParamBitField::ModelParamBitField()
{
    _parent = NULL;
}

ModelParamBitField::ModelParamBitField(const ModelParamBitField &modelParamBitField)
{
    _parent = NULL;

    _name = modelParamBitField._name;
    _type = modelParamBitField._type;
    _value = modelParamBitField._value;
    _bitfield = modelParamBitField._bitfield;
    _description = modelParamBitField._description;
    _propertyMap = modelParamBitField._propertyMap;
}

ModelParamBitField::~ModelParamBitField()
{
}

const QString &ModelParamBitField::name() const
{
    return _name;
}

void ModelParamBitField::setName(const QString &name)
{
    _name = name;
}

const QString &ModelParamBitField::type() const
{
    return _type;
}

void ModelParamBitField::setType(const QString &type)
{
    _type = type;
}

const QVariant &ModelParamBitField::value() const
{
    return _value;
}

void ModelParamBitField::setValue(const QVariant &value)
{
    _value = value;
}

const QString &ModelParamBitField::bitfield() const
{
    return _bitfield;
}

void ModelParamBitField::setBitfield(const QString &bitfield)
{
    _bitfield = bitfield;
}

const QString &ModelParamBitField::description() const
{
    return _description;
}

void ModelParamBitField::setDescription(const QString &description)
{
    _description = description;
}

const QString &ModelParamBitField::propertyMap() const
{
    return _propertyMap;
}

void ModelParamBitField::setPropertyMap(const QString &propertyMap)
{
    _propertyMap = propertyMap;
}

ModelParam *ModelParamBitField::parent() const
{
    return _parent;
}

void ModelParamBitField::setParent(ModelParam *parent)
{
    _parent = parent;
}

ModelParamBitField *ModelParamBitField::fromNodeGenerated(const QDomElement &domElement)
{
    ModelParamBitField *paramBitField = new ModelParamBitField();

    paramBitField->setName(domElement.attribute("name","no_name"));
    paramBitField->setType(domElement.attribute("type",""));

    QVariant value = domElement.attribute("value","");
    paramBitField->setValue(value);

    paramBitField->setBitfield(domElement.attribute("bitfield",""));
    paramBitField->setPropertyMap(domElement.attribute("propertymap",""));
    paramBitField->setDescription(domElement.attribute("desc",""));

    return paramBitField;
}

QList<ModelParamBitField *> ModelParamBitField::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelParamBitField *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="bitfield")
                list.append(ModelParamBitField::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QList<uint> ModelParamBitField::listBitFromBitField(const QString &bitField)
{
    QList<uint> bits;
    if(bitField.isEmpty())
        return QList<uint>();

    QString lastSymbole = "";
    int previous = -1;

    QRegExp exp("([-,]?)([0-9]+)");
    int pos = 0;
    while((pos = exp.indexIn(bitField, pos)) != -1)
    {
        QString symbole = exp.cap(1);
        int bit = exp.cap(2).toUInt();

        if(bit>31)
            return QList<uint>();

        if(symbole==",")
        {
            if(lastSymbole=="," || lastSymbole=="")
            {
                if(previous==-1)
                    return QList<uint>();
                bits.append(previous);
            }
        }
        else if(symbole=="-")
        {
            if(previous<bit)
                return QList<uint>();
            for(int i=previous; i>=bit; i--) bits.append(i);
        }

        previous = bit;
        lastSymbole = symbole;

        pos += exp.matchedLength();
    }

    if((lastSymbole=="," || lastSymbole=="") and previous!=-1)
        bits.append(previous);

    return bits;
}
