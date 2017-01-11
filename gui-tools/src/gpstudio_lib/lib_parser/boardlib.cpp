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

#include "boardlib.h"

#include <QFile>
#include <QDebug>

#include "model/model_board.h"

BoardLib::BoardLib()
{
}

BoardLib::~BoardLib()
{

}

const QString &BoardLib::name() const
{
    return _name;
}

void BoardLib::setName(const QString &name)
{
    _name = name;
}

QList<IOBoardLib *> &BoardLib::ios()
{
    return _ios;
}

const QList<IOBoardLib *> &BoardLib::ios() const
{
    return _ios;
}

void BoardLib::addIO(IOBoardLib *io)
{
    _ios.append(io);
    _iosMap.insert(io->name(), io);
    if(_iosGroups.contains(io->group()))
    {
        _iosGroups[io->group()].addIos(io->name());
    }
    else
    {
        _iosGroups.insert(io->group(), IOBoardLibGroup(io->type()));
        _iosGroups[io->group()].addIos(io->name());
    }
}

void BoardLib::addIOs(const QList<IOBoardLib *> &ios)
{
    foreach (IOBoardLib *io, ios)
        addIO(io);
}

IOBoardLib *BoardLib::io(const QString &name) const
{
    QMap<QString, IOBoardLib* >::const_iterator it = _iosMap.find(name);
    if(it != _iosMap.end())
        return it.value();
    return NULL;
}

ModelBoard *BoardLib::modelBoard() const
{
    return _modelBoard;
}

BoardLib *BoardLib::readFromFile(const QString &fileName)
{
    QDomDocument doc;
    QFile file(fileName);
    BoardLib *boardLib = NULL;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug()<<"Cannot open"<<file.fileName();
    else
    {
        if(!doc.setContent(&file))
            qDebug()<<"Cannot open"<<file.fileName();
        else
            boardLib = BoardLib::fromDomElement(doc.documentElement());
        file.close();
    }
    return boardLib;
}

BoardLib *BoardLib::fromDomElement(const QDomElement &domElement)
{
    BoardLib *board=new BoardLib();
    board->_modelBoard = new ModelBoard();

    board->setName(domElement.attribute("name","no_name"));
    board->_modelBoard->setName(board->name());

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="ios")
                board->addIOs(IOBoardLib::listFromDomElement(e));
        }
        n = n.nextSibling();
    }

    return board;
}

const QMap<QString, IOBoardLibGroup> &BoardLib::iosGroups() const
{
    return _iosGroups;
}
