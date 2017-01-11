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

#include "model_componentpartproperty.h"

#include "model_block.h"

#include <QDebug>

ModelComponentPartProperty::ModelComponentPartProperty()
{
    _parent = NULL;
    _pos = QPoint(0, 0);
}

ModelComponentPartProperty::ModelComponentPartProperty(const ModelComponentPartProperty &other)
{
    _name = other._name;
    _pos = other._pos;
}

ModelComponentPartProperty::~ModelComponentPartProperty()
{
}

QString ModelComponentPartProperty::name() const
{
    return _name;
}

void ModelComponentPartProperty::setName(const QString &name)
{
    _name = name;
}

QPoint ModelComponentPartProperty::pos() const
{
    return _pos;
}

void ModelComponentPartProperty::setPos(const QPoint &pos)
{
    _pos = pos;
}

QSize ModelComponentPartProperty::size() const
{
    return _size;
}

void ModelComponentPartProperty::setSize(const QSize &size)
{
    _size = size;
}

ModelComponentPart *ModelComponentPartProperty::parent() const
{
    return _parent;
}

void ModelComponentPartProperty::setParent(ModelComponentPart *parent)
{
    _parent = parent;
}

ModelComponentPartProperty *ModelComponentPartProperty::fromNodeGenerated(const QDomElement &domElement)
{
    bool ok;

    ModelComponentPartProperty *part = new ModelComponentPartProperty();

    part->setName(domElement.attribute("name","no_name"));

    QPoint pos;
    int xPos = domElement.attribute("x_pos","-1").toInt(&ok);
    if(ok)
        pos.setX(xPos);
    else
        pos.setX(-1);

    int yPos = domElement.attribute("y_pos","-1").toInt(&ok);
    if(ok)
        pos.setY(yPos);
    else
        pos.setY(-1);
    part->setPos(pos);

    QSize size;
    int width = domElement.attribute("width","-1").toInt(&ok);
    if(ok)
        size.setWidth(width);
    else
        size.setWidth(-1);

    int height = domElement.attribute("height","-1").toInt(&ok);
    if(ok)
        size.setHeight(height);
    else
        size.setHeight(-1);
    part->setSize(size);

    return part;
}

QList<ModelComponentPartProperty *> ModelComponentPartProperty::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelComponentPartProperty *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="property")
                list.append(ModelComponentPartProperty::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelComponentPartProperty::toXMLElement(QDomDocument &doc)
{
    QDomElement element;
    element = doc.createElement("part");

    element.setAttribute("name", _name);
    element.setAttribute("x_pos", _pos.x());
    element.setAttribute("y_pos", _pos.y());
    element.setAttribute("width", _size.width());
    element.setAttribute("height", _size.height());

    return element;
}
