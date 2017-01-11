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

#include "propertyenum.h"

PropertyEnum::PropertyEnum(const QString &name, const QVariant &value)
    : _name(name), _value(value)
{
}

PropertyEnum::PropertyEnum(const QString &name, const QString &caption, const QVariant &value)
    : _name(name), _caption(caption), _value(value)
{
}

PropertyEnum::~PropertyEnum()
{
}

QString PropertyEnum::name() const
{
    return _name;
}

void PropertyEnum::setName(const QString &name)
{
    _name = name;
}

QString PropertyEnum::caption() const
{
    return _caption;
}

void PropertyEnum::setCaption(const QString &caption)
{
    _caption = caption;
}

QVariant PropertyEnum::value() const
{
    return _value;
}

void PropertyEnum::setValue(const QVariant &value)
{
    _value = value;
}
