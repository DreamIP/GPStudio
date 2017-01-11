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

#include "blockconnectoritem.h"

#include "blockportitem.h"

#include <QDebug>
#include <QPainter>
#include <QGraphicsScene>
#include <QCursor>
#include <QStyleOptionGraphicsItem>
#include "model/model_flow.h"

BlockConnectorItem::BlockConnectorItem(BlockPortItem *portItemOut, BlockPortItem *portItemIn)
    : _portItem1(portItemOut), _portItem2(portItemIn)
{
    setFlag(ItemIsSelectable, true);

    if(_portItem1)
        _portItem1->addConnect(this);
    if(_portItem2)
        _portItem2->addConnect(this);
    setZValue(-2);
    //_style=LineDraw;
    _style=CubicDraw;
    _highlight = false;
    updateShape(NULL);

    setCursor(Qt::CrossCursor);
}

BlockConnectorItem::~BlockConnectorItem()
{
}

int BlockConnectorItem::type() const
{
    return Type;
}

BlockConnectorItem::DrawStyle BlockConnectorItem::style() const
{
    return _style;
}

void BlockConnectorItem::setStyle(const BlockConnectorItem::DrawStyle &style)
{
    _style = style;
    update();
}

QRectF BlockConnectorItem::boundingRect() const
{
    return QRectF(_inPos, _outPos).normalized().adjusted(-5, -15, 5, 15);
}

QPainterPath BlockConnectorItem::shape() const
{
    QPen pen(Qt::black, 10);
    QPainterPathStroker ps;
    ps.setWidth(pen.width());
    ps.setCapStyle(pen.capStyle());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(_shape);
    p.addPath(_shape);

    if(_highlight || isSelected())
    {
        QPainterPath in;
        in.addRect(QRectF(_inSizePoint + QPointF(-10,-5), QSize(20,-10)));
        p.addPath(in);
        QPainterPath out;
        out.addRect(QRectF(_outSizePoint + QPointF(-20,-5), QSize(40,-10)));
        p.addPath(out);
    }
    return p;
}

void BlockConnectorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)

    // back draw
    painter->setBrush(QBrush(QColor(255, 0, 0, 100)));
    painter->setPen(QPen(Qt::white, 6));
    painter->drawPath(_shape);

    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    // front draw
    if(_highlight || isSelected())
    {
        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);

        painter->setPen(QPen(QColor(255,165,0), 4));
        painter->drawPath(_shape);

        // flow size draw
        if(lod>0.65)
        {
            painter->setPen(QPen(QColor(255,145,0), 2));

            painter->drawText(QRectF(_inSizePoint + QPointF(-10,-20), QSize(20,15)), Qt::AlignHCenter | Qt::AlignTop,
                              QString("%1").arg(_portItem1->modelFlow()->size()));
            painter->drawLine(_inSizePoint + QPointF(-2,-5), _inSizePoint + QPointF(2,5));

            QString outSizeText;
            if(_portItem1->modelFlow()->size() == _portItem2->modelFlow()->size())
                outSizeText = QString("%1").arg(_portItem2->modelFlow()->size());
            else
            {
                // TODO add msl lsb
                if(_portItem1->modelFlow()->size() > _portItem2->modelFlow()->size())
                    outSizeText = QString("[%1..%2]").arg(_portItem1->modelFlow()->size()-1)
                            .arg(_portItem1->modelFlow()->size()-_portItem2->modelFlow()->size());
                else
                    outSizeText = QString("[%1..0]").arg(_portItem1->modelFlow()->size()-1);
            }
            painter->drawText(QRectF(_outSizePoint + QPointF(-20,-20), QSize(40,15)), Qt::AlignHCenter | Qt::AlignTop, outSizeText);
            painter->drawLine(_outSizePoint + QPointF(-2,-5), _outSizePoint + QPointF(2,5));
        }
    }
    else
    {
        painter->setPen(QPen(Qt::black, 3));
        painter->drawPath(_shape);
    }

}

void BlockConnectorItem::updateShape(BlockPortItem *caller)
{
    bool inInit=false, outInit=false;
    BlockPortItem *outUpdate = NULL;
    int connectorId = 0;;

    prepareGeometryChange();

    // compute point start and end
    if(_portItem1)
    {
        if(_portItem1->direction()==BlockPortItem::Input)
        {
            _inPos = _portItem1->connectorPos(this);
            connectorId = _portItem1->connectorId(this);
            inInit = true;
        }
        else
        {
            QPointF outPos = _portItem1->connectorPos(this);
            if(outPos != _outPos && caller == _portItem1)
                outUpdate = _portItem2;
            _outPos = outPos;
            outInit = true;
        }
    }
    if(_portItem2)
    {
        if(_portItem2->direction()==BlockPortItem::Input)
        {
            _inPos = _portItem2->connectorPos(this);
            connectorId = _portItem2->connectorId(this);
            inInit = true;
        }
        else
        {
            QPointF outPos = _portItem2->connectorPos(this);
            if(outPos != _outPos && caller == _portItem2)
                outUpdate = _portItem1;
            _outPos = outPos;
            outInit = true;
        }
    }
    if(!inInit)
        _inPos = _endPos;
    if(!outInit)
        _outPos = _endPos;

    // compute shape
    QRectF rect = QRectF(_outPos, _inPos).normalized();
    int y1, y2;
    if(_outPos.y() > _inPos.y())
    {
        y1 = rect.bottom();
        y2 = rect.top();
    }
    else
    {
        y1 = rect.top();
        y2 = rect.bottom();
    }
    QPainterPath path;
    path.moveTo(_outPos);
    if(_style==LineDraw)
    {
        path.lineTo(QPoint(rect.center().x() + connectorId * 10, y1));
        path.lineTo(QPoint(rect.center().x() + connectorId * 10, y2));
        path.lineTo(_inPos);
        path.lineTo(QPoint(rect.center().x() + connectorId * 10, y2));
        path.lineTo(QPoint(rect.center().x() + connectorId * 10, y1));
        path.lineTo(_outPos);
    }
    if(_style==CubicDraw)
    {
        path.cubicTo(QPoint(rect.center().x() + connectorId * 10, y1), QPoint(rect.center().x() + connectorId * 10, y2), _inPos);
        path.cubicTo(QPoint(rect.center().x() + connectorId * 10, y2), QPoint(rect.center().x() + connectorId * 10, y1), _outPos);
    }

    _shape = path;
    _inSizePoint = _shape.pointAtPercent(_shape.percentAtLength(20));
    _outSizePoint = _shape.pointAtPercent(_shape.percentAtLength(_shape.length()/2-20));

    if(!_portItem1 && caller==NULL)
        _portItem2->updateShape();
    if(!_portItem2 && caller==NULL)
        _portItem1->updateShape();
    if(outUpdate)
        outUpdate->updateShape();
}

QPoint BlockConnectorItem::endPos() const
{
    return _endPos;
}

void BlockConnectorItem::setEndPos(const QPoint &endPos)
{
    _endPos = endPos;
    updateShape(NULL);
}

void BlockConnectorItem::disconnectPorts()
{
    if(_portItem1)
        _portItem1->removeConnect(this);
    if(_portItem2)
        _portItem2->removeConnect(this);
}

BlockPortItem *BlockConnectorItem::portItem1() const
{
    return _portItem1;
}

BlockPortItem *BlockConnectorItem::portItem2() const
{
    return _portItem2;
}

QPointF BlockConnectorItem::inPos() const
{
    return _inPos;
}

QPointF BlockConnectorItem::outPos() const
{
    return _outPos;
}

bool BlockConnectorItem::isHighlighted() const
{
    return _highlight;
}

void BlockConnectorItem::setHighlight(bool highlight)
{
    emit prepareGeometryChange();
    _highlight = highlight;
    if(_highlight || isSelected())
    {
        setZValue(-1);
    }
    else
    {
        setZValue(-2);
    }
}

QVariant BlockConnectorItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene())
    {
        if(isSelected())
            setZValue(-1);
        else
        {
            if(!_portItem1->parentItem()->isSelected() && !_portItem2->parentItem()->isSelected())
                _highlight = false;
            setZValue(-2);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
