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

#ifndef FEATUREITEM_H
#define FEATUREITEM_H

#include "gpstudio_gui_common.h"

#include <QGraphicsItem>

#include "feature.h"

class GPSTUDIO_GUI_EXPORT FeatureItem : public QGraphicsItem
{
public:
    FeatureItem(const Feature &feature=Feature());

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    Feature feature() const;

protected:
    Feature _feature;
    void setFeature(const Feature &feature);
    QRectF _boudingRect;
};

#endif // FEATUREITEM_H
