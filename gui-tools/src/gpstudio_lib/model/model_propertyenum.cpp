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

#include "model_propertyenum.h"

ModelPropertyEnum::ModelPropertyEnum()
{
}

ModelPropertyEnum::ModelPropertyEnum(const ModelPropertyEnum &modelPropertyEnum)
{
    _name = modelPropertyEnum._name;
    _caption = modelPropertyEnum._caption;
    _value = modelPropertyEnum._value;
    _description = modelPropertyEnum._description;
}

ModelPropertyEnum::~ModelPropertyEnum()
{
}

const QString &ModelPropertyEnum::name() const
{
    return _name;
}

void ModelPropertyEnum::setName(const QString &name)
{
    _name = name;
}

const QString &ModelPropertyEnum::caption() const
{
    return _caption;
}

void ModelPropertyEnum::setCaption(const QString &caption)
{
    _caption = caption;
}

const QVariant &ModelPropertyEnum::value() const
{
    return _value;
}

void ModelPropertyEnum::setValue(const QVariant &value)
{
    _value = value;
}

const QString &ModelPropertyEnum::description() const
{
    return _description;
}

void ModelPropertyEnum::setDescription(const QString &description)
{
    _description = description;
}

ModelPropertyEnum *ModelPropertyEnum::fromNodeGenerated(const QDomElement &domElement)
{
    ModelPropertyEnum *blockPropertyEnum = new ModelPropertyEnum();

    blockPropertyEnum->setName(domElement.attribute("name","no_name"));
    blockPropertyEnum->setCaption(domElement.attribute("caption",blockPropertyEnum->name()));

    QVariant value = domElement.attribute("value","");
    blockPropertyEnum->setValue(value);

    blockPropertyEnum->setDescription(domElement.attribute("desc",""));

    return blockPropertyEnum;
}

QList<ModelPropertyEnum *> ModelPropertyEnum::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelPropertyEnum *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="enum")
                list.append(ModelPropertyEnum::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
