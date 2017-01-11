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

#include "feature.h"

Feature::Feature()
{

}

int Feature::x() const
{
    return _x;
}

void Feature::setX(int x)
{
    _x = x;
}

int Feature::y() const
{
    return _y;
}

void Feature::setY(int y)
{
    _y = y;
}

QPoint Feature::pos() const
{
    return QPoint(_x, _y);
}

int Feature::w() const
{
    return _w;
}

void Feature::setW(int w)
{
    _w = w;
}

int Feature::h() const
{
    return _h;
}

void Feature::setH(int h)
{
    _h = h;
}

QRect Feature::rect() const
{
    return QRect(_x, _y, _w, _h);
}

int Feature::val() const
{
    return _val;
}

void Feature::setVal(int val)
{
    _val = val;
}

Feature::Type Feature::type() const
{
    return _type;
}

void Feature::setType(const Feature::Type &type)
{
    _type = type;
}
