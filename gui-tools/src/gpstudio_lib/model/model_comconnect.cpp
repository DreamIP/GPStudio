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

#include "model_comconnect.h"

ModelComConnect::ModelComConnect()
{
}

ModelComConnect::~ModelComConnect()
{
}

const QString &ModelComConnect::link() const
{
    return _link;
}

void ModelComConnect::setLink(const QString &link)
{
    _link = link;
}

const QString &ModelComConnect::id() const
{
    return _id;
}

void ModelComConnect::setId(const QString &id)
{
    _id = id;
}

const QString &ModelComConnect::type() const
{
    return _type;
}

void ModelComConnect::setType(const QString &type)
{
    _type = type;
}

ModelComConnect *ModelComConnect::fromNodeGenerated(const QDomElement &domElement)
{
    ModelComConnect *comConnect = new ModelComConnect();

    comConnect->setLink(domElement.attribute("link",""));
    comConnect->setId(domElement.attribute("id",""));
    comConnect->setType(domElement.attribute("type",""));

    return comConnect;
}

QList<ModelComConnect *> ModelComConnect::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelComConnect *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="com_connect")
                list.append(ModelComConnect::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
