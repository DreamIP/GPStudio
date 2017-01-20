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

#include "model_comparam.h"

ModelComParam::ModelComParam()
{
}

ModelComParam::~ModelComParam()
{
}

QString ModelComParam::name() const
{
    return _name;
}

void ModelComParam::setName(const QString &name)
{
    _name = name;
}

QVariant ModelComParam::value() const
{
    return _value;
}

void ModelComParam::setValue(const QVariant &value)
{
    _value = value;
}

ModelComParam *ModelComParam::fromNodeGenerated(const QDomElement &domElement)
{
    ModelComParam *comParam = new ModelComParam();

    comParam->setName(domElement.attribute("name",""));
    comParam->setValue(domElement.attribute("value",""));

    return comParam;
}

QList<ModelComParam *> ModelComParam::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelComParam *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="com_param")
                list.append(ModelComParam::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
