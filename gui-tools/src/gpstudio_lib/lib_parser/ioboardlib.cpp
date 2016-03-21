/****************************************************************************
** Copyright (C) 2016 Dream IP
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

#include "ioboardlib.h"

IOBoardLib::IOBoardLib()
{

}

IOBoardLib::~IOBoardLib()
{

}

QString IOBoardLib::name() const
{
    return _name;
}

void IOBoardLib::setName(const QString &name)
{
    _name = name;
}

QString IOBoardLib::type() const
{
    return _type;
}

void IOBoardLib::setType(const QString &type)
{
    _type = type;
}

QString IOBoardLib::driver() const
{
    return _driver;
}

void IOBoardLib::setDriver(const QString &driver)
{
    _driver = driver;
}

bool IOBoardLib::isOptional() const
{
    return _optional;
}

void IOBoardLib::setOptional(bool optional)
{
    _optional = optional;
}

QString IOBoardLib::group() const
{
    return _group;
}

void IOBoardLib::setGroup(const QString &group)
{
    _group = group;
}

QString IOBoardLib::description() const
{
    return _description;
}

void IOBoardLib::setDescription(const QString &description)
{
    _description = description;
}

IOBoardLib *IOBoardLib::fromNodeGenerated(const QDomElement &domElement)
{
    IOBoardLib *io=new IOBoardLib();
    io->setName(domElement.attribute("name","no_name"));
    io->setType(domElement.attribute("type",""));
    io->setDriver(domElement.attribute("driver",""));

    io->setOptional((domElement.attribute("optional","")=="1" || domElement.attribute("optional","")=="true"));
    io->setGroup(domElement.attribute("optionalgroup",""));

    io->setDescription(domElement.attribute("description",""));

    return io;
}

QList<IOBoardLib *> IOBoardLib::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<IOBoardLib *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="io") list.append(IOBoardLib::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
