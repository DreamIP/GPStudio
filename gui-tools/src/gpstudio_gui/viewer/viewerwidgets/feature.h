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

#ifndef FEATURE_H
#define FEATURE_H

#include "gpstudio_gui_common.h"

#include <QPoint>
#include <QRect>
#include <QList>

class FlowPackage;
class Property;

class GPSTUDIO_GUI_EXPORT Feature
{
public:
    enum Type {
        Invalid,
        Point,
        PointValue,
        Rect,
        RectValue
    };

    Feature();
    Feature(int16_t x, int16_t y);
    Feature(int16_t x, int16_t y, int16_t value);
    Feature(int16_t x, int16_t y, int16_t w, int16_t h);
    Feature(int16_t x, int16_t y, int16_t w, int16_t h, int16_t value);
    Feature(const Feature &other);

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);

    QPoint pos() const;

    int w() const;
    void setW(int w);

    int h() const;
    void setH(int h);

    QRect rect() const;

    int val() const;
    void setVal(int val);

    Type type() const;
    void setType(const Type &type);

protected:
    int _x;
    int _y;
    int _w;
    int _h;
    int _val;
    Type _type;

public:
    static Type stringToType(const QString &string);
    static QList<Feature> fromData(const FlowPackage &package, Property *flow);
};

#endif // FEATURE_H
