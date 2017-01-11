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

#include "model_clock.h"

#include "model_block.h"

#include <QDebug>

ModelClock::ModelClock(ModelBlock *parent)
    : _parent(parent)
{
    _parent = NULL;
}

ModelClock::~ModelClock()
{
}

const QString &ModelClock::name() const
{
    return _name;
}

void ModelClock::setName(const QString &name)
{
    _name = name;
}

const QString &ModelClock::group() const
{
    return _group;
}

void ModelClock::setGroup(const QString &group)
{
    _group = group;
}

qint16 ModelClock::shift() const
{
    return _shift;
}

void ModelClock::setShift(const qint16 &shift)
{
    _shift = shift;
}

qint32 ModelClock::min() const
{
    return _min;
}

void ModelClock::setMin(const qint32 &min)
{
    _min = min;
}

qint32 ModelClock::max() const
{
    return _max;
}

void ModelClock::setMax(const qint32 &max)
{
    _max = max;
}

qint32 ModelClock::typical() const
{
    return _typical;
}

void ModelClock::setTypical(const qint32 &typical)
{
    _typical = typical;
}

ModelClock::Direction ModelClock::direction() const
{
    return _direction;
}

void ModelClock::setDirection(const ModelClock::Direction &direction)
{
    _direction = direction;
}

const QString &ModelClock::description() const
{
    return _description;
}

void ModelClock::setDescription(const QString &desc)
{
    _description = desc;
}

ModelBlock *ModelClock::parent() const
{
    return _parent;
}

void ModelClock::setParent(ModelBlock *parent)
{
    _parent = parent;
}

ModelClock *ModelClock::fromNodeGenerated(const QDomElement &domElement)
{
    ModelClock *clock = new ModelClock();
    bool ok=false;

    clock->setName(domElement.attribute("name","no_name"));
    clock->setGroup(domElement.attribute("group",""));

    int shift = domElement.attribute("shift","0").toInt(&ok);
    if(ok)
        clock->setShift(shift);
    else
        clock->setShift(0);

    int min = domElement.attribute("min","0").toInt(&ok);
    if(ok)
        clock->setMin(min);
    else
        clock->setMin(0);

    QString direction = domElement.attribute("direction","in");
    if(direction == "in")
        clock->setDirection(In);
    else
        clock->setDirection(Out);

    int max = domElement.attribute("max","0").toInt(&ok);
    if(ok)
        clock->setMax(max);
    else
        clock->setMax(0);

    int mult = 1;
    QString typicalStr = domElement.attribute("typical","0");
    if(typicalStr.contains("G", Qt::CaseInsensitive))
        mult = 1000000000;
    if(typicalStr.contains("M", Qt::CaseInsensitive))
        mult = 1000000;
    if(typicalStr.contains("k", Qt::CaseInsensitive))
        mult = 1000;
    typicalStr.replace("GgMmKk","");
    int typical = typicalStr.toInt(&ok);
    if(ok)
        clock->setTypical(typical * mult);
    else
        clock->setTypical(0);

    clock->setDescription(domElement.attribute("desc",""));

    return clock;
}

QList<ModelClock *> ModelClock::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelClock *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="clock")
                list.append(ModelClock::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}

QDomElement ModelClock::toXMLElement(QDomDocument &doc)
{
    QDomElement element = doc.createElement("clock");

    element.setAttribute("name", _name);
    element.setAttribute("typical", _typical);

    return element;
}
