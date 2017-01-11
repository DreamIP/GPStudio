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

#ifndef BLOCKCONNECTORITEM_H
#define BLOCKCONNECTORITEM_H

#include "gpstudio_gui_common.h"

#include <QGraphicsItem>

class BlockPortItem;

class GPSTUDIO_GUI_EXPORT BlockConnectorItem : public QGraphicsItem
{
public:
    BlockConnectorItem(BlockPortItem *portItemOut, BlockPortItem *portItemIn = NULL);
    ~BlockConnectorItem();

    enum { Type = UserType + 2 };
    int type() const;

    enum DrawStyle {
        LineDraw,
        CubicDraw
    };
    DrawStyle style() const;
    void setStyle(const DrawStyle &style);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void updateShape(BlockPortItem *caller);

    QPoint endPos() const;
    void setEndPos(const QPoint &endPos);

    void disconnectPorts();
    BlockPortItem *portItem1() const;
    BlockPortItem *portItem2() const;

    QPointF inPos() const;
    QPointF outPos() const;

    bool isHighlighted() const;
    void setHighlight(bool highlight);

private:
    BlockPortItem *_portItem1;
    BlockPortItem *_portItem2;
    QPoint _endPos;

    QPointF _inPos;
    QPointF _inSizePoint;
    QPointF _outPos;
    QPointF _outSizePoint;
    QPainterPath _shape;

    DrawStyle _style;
    bool _highlight;

    // QGraphicsItem interface
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

#endif // BLOCKCONNECTORITEM_H
