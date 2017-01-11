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

#include "model_componentpart.h"

#include "model_block.h"

#include <QDebug>

ModelComponentPart::ModelComponentPart()
{
    _parent = NULL;
    _pos = QPoint(0, 0);
}

ModelComponentPart::ModelComponentPart(const ModelComponentPart &other)
{
    _name = other._name;
    _draw = other._draw;
    _pos = other._pos;
    _parent = NULL;

    for(int i=0; i<other._flows.size(); i++)
        addFlow(new ModelComponentPartFlow(*other._flows[i]));
    for(int i=0; i<other._properties.size(); i++)
        addProperty(new ModelComponentPartProperty(*other._properties[i]));
}

ModelComponentPart::~ModelComponentPart()
{
    for(int i=0; i<_flows.size(); i++)
        delete _flows[i];
    for(int i=0; i<_properties.size(); i++)
        delete _properties[i];
}

QString ModelComponentPart::name() const
{
    return _name;
}

void ModelComponentPart::setName(const QString &name)
{
    _name = name;
}

QPoint ModelComponentPart::pos() const
{
    return _pos;
}

void ModelComponentPart::setPos(const QPoint &pos)
{
    _pos = pos;
}

QString ModelComponentPart::draw() const
{
    return _draw;
}

void ModelComponentPart::setDraw(const QString &draw)
{
    _draw = draw;
}

ModelBlock *ModelComponentPart::parent() const
{
    return _parent;
}

void ModelComponentPart::setParent(ModelBlock *parent)
{
    _parent = parent;
}

QList<ModelComponentPartFlow *> &ModelComponentPart::flows()
{
    return _flows;
}

const QList<ModelComponentPartFlow *> &ModelComponentPart::flows() const
{
    return _flows;
}

void ModelComponentPart::addFlow(ModelComponentPartFlow *flow)
{
    flow->setParent(this);
    _flows.append(flow);
}

void ModelComponentPart::addFlows(const QList<ModelComponentPartFlow *> &flows)
{
    foreach (ModelComponentPartFlow *flow, flows)
    {
        addFlow(flow);
    }
}

ModelComponentPartFlow *ModelComponentPart::getFlow(const QString &name) const
{
    for(int i=0; i<this->flows().size(); i++)
    {
        ModelComponentPartFlow *flow = this->flows().at(i);
        if(flow->name()==name)
            return flow;
    }
    return NULL;
}

QList<ModelComponentPartProperty *> &ModelComponentPart::properties()
{
    return _properties;
}

const QList<ModelComponentPartProperty *> &ModelComponentPart::properties() const
{
    return _properties;
}

void ModelComponentPart::addProperty(ModelComponentPartProperty *property)
{
    property->setParent(this);
    _properties.append(property);
}

void ModelComponentPart::addProperties(const QList<ModelComponentPartProperty *> &properties)
{
    foreach (ModelComponentPartProperty *property, properties)
    {
        addProperty(property);
    }
}

ModelComponentPartProperty *ModelComponentPart::getProperty(const QString &name) const
{
    for(int i=0; i<this->_properties.size(); i++)
    {
        ModelComponentPartProperty *property = this->properties().at(i);
        if(property->name()==name)
            return property;
    }
    return NULL;
}

ModelComponentPart *ModelComponentPart::fromNodeGenerated(const QDomElement &domElement)
{
    bool ok;
    QPoint pos;

    ModelComponentPart *part = new ModelComponentPart();

    part->setName(domElement.attribute("name","no_name"));
    if(part->parent())
        qDebug()<<part->name()<<part->parent()->name();

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

    // get svg part of file and save as string
    const QDomNodeList &nodesSvg = domElement.elementsByTagName("svg");
    if(nodesSvg.size()>0)
    {
        QString svg;
        QTextStream streamSvg(&svg);
        streamSvg << nodesSvg.at(0);
        part->setDraw(svg);
    }

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="flows")
                part->addFlows(ModelComponentPartFlow::listFromNodeGenerated(e));
            if(e.tagName()=="properties")
                part->addProperties(ModelComponentPartProperty::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return part;
}

QList<ModelComponentPart *> ModelComponentPart::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelComponentPart *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="part")
                list.append(ModelComponentPart::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelComponentPart::toXMLElement(QDomDocument &doc)
{
    QDomElement element;
    element = doc.createElement("part");

    element.setAttribute("x_pos", _pos.x());
    element.setAttribute("y_pos", _pos.y());
    element.setAttribute("name", _name);

    return element;
}
