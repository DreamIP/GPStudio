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

#include "model_componentpartflow.h"

#include "model_block.h"

#include <QDebug>

ModelComponentPartFlow::ModelComponentPartFlow()
{
    _parent = NULL;
    _pos = QPoint(0, 0);
}

ModelComponentPartFlow::ModelComponentPartFlow(const ModelComponentPartFlow &other)
{
    _name = other._name;
    _pos = other._pos;
}

ModelComponentPartFlow::~ModelComponentPartFlow()
{
}

QString ModelComponentPartFlow::name() const
{
    return _name;
}

void ModelComponentPartFlow::setName(const QString &name)
{
    _name = name;
}

QPoint ModelComponentPartFlow::pos() const
{
    return _pos;
}

void ModelComponentPartFlow::setPos(const QPoint &pos)
{
    _pos = pos;
}

ModelComponentPart *ModelComponentPartFlow::parent() const
{
    return _parent;
}

void ModelComponentPartFlow::setParent(ModelComponentPart *parent)
{
    _parent = parent;
}

ModelComponentPartFlow *ModelComponentPartFlow::fromNodeGenerated(const QDomElement &domElement)
{
    bool ok;
    QPoint pos;

    ModelComponentPartFlow *part = new ModelComponentPartFlow();

    part->setName(domElement.attribute("name","no_name"));

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

    return part;
}

QList<ModelComponentPartFlow *> ModelComponentPartFlow::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelComponentPartFlow *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="flow")
                list.append(ModelComponentPartFlow::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelComponentPartFlow::toXMLElement(QDomDocument &doc)
{
    QDomElement element;
    element = doc.createElement("part");

    element.setAttribute("name", _name);
    element.setAttribute("x_pos", _pos.x());
    element.setAttribute("y_pos", _pos.y());

    return element;
}
