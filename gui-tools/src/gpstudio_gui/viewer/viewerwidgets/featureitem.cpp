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

#include "featureitem.h"

#include <QPainter>

FeatureItem::FeatureItem(const Feature &feature)
{
    setFeature(feature);
}

QRectF FeatureItem::boundingRect() const
{
    return _boudingRect;
}

void FeatureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // TODO get color from viewer properties
    painter->setPen(Qt::red);

    switch (_feature.type())
    {
    case Feature::Point:
        painter->drawLine(_feature.x()-5, _feature.y()-5, _feature.x()+5, _feature.y()+5);
        painter->drawLine(_feature.x()+5, _feature.y()-5, _feature.x()-5, _feature.y()+5);
        break;
    case Feature::PointValue:
        painter->drawLine(_feature.x()-5, _feature.y()-5, _feature.x()+5, _feature.y()+5);
        painter->drawLine(_feature.x()+5, _feature.y()-5, _feature.x()-5, _feature.y()+5);
        // TODO add value
        break;
    case Feature::Rect:
        painter->drawRect(QRectF(_feature.x(), _feature.y(), _feature.w(), _feature.w()));
        break;
    case Feature::RectValue:
        painter->drawRect(QRectF(_feature.x(), _feature.y(), _feature.w(), _feature.w()));
        // TODO add value
        break;
    }
}

Feature FeatureItem::feature() const
{
    return _feature;
}

void FeatureItem::setFeature(const Feature &feature)
{
    _feature = feature;

    // bounding rect computation
    switch (_feature.type())
    {
    case Feature::Point:
    case Feature::PointValue:
        _boudingRect = QRectF(feature.x()-5, feature.y()-5, 10, 10);
        break;
    case Feature::Rect:
    case Feature::RectValue:
        _boudingRect = QRectF(feature.x()-2, feature.y()-2, feature.w()+4, feature.w()+4);
        break;
    }
}
