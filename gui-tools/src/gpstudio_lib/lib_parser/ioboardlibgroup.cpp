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

#include "ioboardlibgroup.h"

IOBoardLibGroup::IOBoardLibGroup(const QString &name)
    : _name(name)
{
}

IOBoardLibGroup::~IOBoardLibGroup()
{
}

const QString &IOBoardLibGroup::name() const
{
    return _name;
}

void IOBoardLibGroup::setName(const QString &name)
{
    _name = name;
}

bool IOBoardLibGroup::isOptional() const
{
    return _optional;
}

void IOBoardLibGroup::setOptional(bool isOptional)
{
    _optional = isOptional;
}

const QStringList &IOBoardLibGroup::ios() const
{
    return _ios;
}

void IOBoardLibGroup::addIos(const QString &name)
{
    _ios.append(name);
}


