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

#include "model_attribute.h"

ModelAttribute::ModelAttribute()
{
}

ModelAttribute::~ModelAttribute()
{
}

const QString &ModelAttribute::name() const
{
    return _name;
}

void ModelAttribute::setName(const QString &name)
{
    _name = name;
}

const QString &ModelAttribute::value() const
{
    return _value;
}

void ModelAttribute::setValue(const QString &value)
{
    _value = value;
}

const QString &ModelAttribute::type() const
{
    return _type;
}

void ModelAttribute::setType(const QString &type)
{
    _type = type;
}

ModelAttribute *ModelAttribute::fromNodeGenerated(const QDomElement &domElement)
{
    ModelAttribute *attribute = new ModelAttribute();

    attribute->setName(domElement.attribute("name","no_name"));
    attribute->setValue(domElement.attribute("value",""));
    attribute->setType(domElement.attribute("type",""));

    return attribute;
}

QList<ModelAttribute *> ModelAttribute::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelAttribute *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="attribute")
                list.append(ModelAttribute::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
