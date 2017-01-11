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

#include "model_board.h"

#include "model_block.h"
#include "model_node.h"

#include <QDebug>

ModelBoard::ModelBoard()
{
    _parent = NULL;
}

ModelBoard::~ModelBoard()
{
}

const QString &ModelBoard::name() const
{
    return _name;
}

void ModelBoard::setName(const QString &name)
{
    _name = name;
}

ModelNode *ModelBoard::parent() const
{
    return _parent;
}

void ModelBoard::setParent(ModelNode *parent)
{
    _parent = parent;
}

ModelBoard *ModelBoard::fromNodeGenerated(const QDomElement &domElement)
{
    ModelBoard *attribute = new ModelBoard();

    attribute->setName(domElement.attribute("name","no_name"));

    return attribute;
}

ModelBoard *ModelBoard::fromNodeDef(const QDomElement &domElement)
{
    ModelBoard *board = new ModelBoard();

    board->setName(domElement.attribute("name","no_name"));

    return board;
}

QList<ModelBlock *> ModelBoard::listIosFromNodeDef(const QDomElement &domElement)
{
    QList<ModelBlock *> ios;
    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="ios")
                ios.append(ModelBlock::listFromNodeDef(e));
        }
        n = n.nextSibling();
    }
    return ios;
}

QDomElement ModelBoard::toXMLElement(QDomDocument &doc)
{
    QDomElement element = doc.createElement("board");

    element.setAttribute("name", _name);

    if(_parent)
    {
        QDomElement iosList = doc.createElement("ios");
        foreach (ModelBlock *io, _parent->blocks())
        {
            if(io->isIO())
            {
                iosList.appendChild(io->toXMLElement(doc));
            }
        }
        element.appendChild(iosList);
    }

    return element;
}
