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

#include "camera/property.h"
#include "flowpackage.h"

#include <QDebug>

Feature::Feature()
{
    _x = 0;
    _y = 0;
    _w = 0;
    _h = 0;
    _val = 0;
    _type = Invalid;
}

Feature::Feature(int16_t x, int16_t y)
{
    _x = x;
    _y = y;
    _w = 0;
    _h = 0;
    _val = 0;
    _type = Point;
}

Feature::Feature(int16_t x, int16_t y, int16_t value)
{
    _x = x;
    _y = y;
    _w = 0;
    _h = 0;
    _val = value;
    _type = PointValue;
}

Feature::Feature(int16_t x, int16_t y, int16_t w, int16_t h)
{
    _x = x;
    _y = y;
    _w = w;
    _h = h;
    _val = 0;
    _type = Rect;
}

Feature::Feature(int16_t x, int16_t y, int16_t w, int16_t h, int16_t value)
{
    _x = x;
    _y = y;
    _w = w;
    _h = h;
    _val = value;
    _type = RectValue;
}

Feature::Feature(const Feature &other)
{
    _x = other._x;
    _y = other._y;
    _w = other._w;
    _h = other._h;
    _val = other._val;
    _type = other._type;
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

Feature::Type Feature::stringToType(const QString &string)
{
    if(string == "point")
        return Point;
    if(string == "pointvalue")
        return PointValue;
    if(string == "rect")
        return Rect;
    if(string == "rectvalue")
        return RectValue;
    return Invalid;
}

QList<Feature> Feature::fromData(const FlowPackage &package, Property *flow)
{
    QList<Feature> features;
    Type type = stringToType(flow->property("featuretype").toString());

    if(type == Feature::Invalid || package.empty())
        return features;

    int16_t *ptr = (int16_t*)package.data().data();
    int16_t *end = ptr + package.data().size() / 2;

    switch (type)
    {
    case Feature::Point:
        while (ptr <= end - 2)
        {
            features.append(Feature(ptr[0], ptr[1]));
            ptr += 2;
        }
        break;
    case Feature::PointValue:
        while (ptr <= end - 3)
        {
            features.append(Feature(ptr[0], ptr[1], ptr[2]));
            ptr += 3;
        }
        break;
    case Feature::Rect:
        while (ptr <= end - 4)
        {
            features.append(Feature(ptr[0], ptr[1], ptr[2], ptr[3]));
            ptr += 4;
        }
        break;
    case Feature::RectValue:
        while (ptr <= end - 5)
        {
            features.append(Feature(ptr[0], ptr[1], ptr[2], ptr[3], ptr[4]));
            ptr += 5;
        }
        break;
    default:
        break;
    }
    return features;
}
