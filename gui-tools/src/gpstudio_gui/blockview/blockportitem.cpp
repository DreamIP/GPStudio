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

#include "blockportitem.h"
#include "blockconnectoritem.h"

#include <QPainter>
#include <QDebug>
#include <QCursor>
#include <QStyleOptionGraphicsItem>

#include "lib_parser/blocklib.h"

#include "model/model_flow.h"

BlockPortItem::BlockPortItem()
{
    setFlag(ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    _direction = Input;
    setCursor(Qt::CrossCursor);
    _hover = false;
}

BlockPortItem::~BlockPortItem()
{
}

int BlockPortItem::type() const
{
    return Type;
}

QRectF BlockPortItem::boundingRect() const
{
    if(_connects.size()<=1)
    {
        if(_direction==Input)
            return QRectF(-15,-15,90,30);
        else
            return QRectF(-90,-15,105,30);
    }
    else
    {
        if(_direction==Input)
            return QRectF(-15,-20,90,40);
        else
            return QRectF(-90,-20,105,40);
    }
}

QPainterPath BlockPortItem::shape() const
{
    QPainterPath path;

    if(_connects.size()<=1)
    {
        path.moveTo(-12, -12);
        path.lineTo(12, 0);
        path.lineTo(-12, 12);
        path.closeSubpath();
    }
    else
    {
        path.moveTo(-12, -17);
        path.lineTo(12, -5);
        path.lineTo(12, 5);
        path.lineTo(-12, 17);
        path.closeSubpath();
    }

    return path;
}

void BlockPortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); Q_UNUSED(widget);

    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    QPainterPath path;

    int pwidth = 9;
    if(_hover)
        pwidth = 11;

    if(_connects.size() > 1 && _direction==Input)
    {
        path.moveTo(-pwidth, -pwidth-5);
        path.lineTo(pwidth, -5);
        path.lineTo(pwidth, 5);
        path.lineTo(-pwidth, pwidth+5);
        path.closeSubpath();
    }
    else
    {
        path.moveTo(-pwidth, -pwidth);
        path.lineTo(pwidth, 0);
        path.lineTo(-pwidth, pwidth);
        path.closeSubpath();
    }

    painter->setPen(QPen(Qt::transparent));

    if(_direction==Input)
    {
        QLinearGradient grad(-15,-15,15,15);
        grad.setColorAt(0, QColor("blue"));
        grad.setColorAt(1, QColor("blue").darker());
        painter->setBrush(grad);
        painter->drawPath(path);

        painter->setPen(QPen());
        if(_name != "in" && lod > 0.3)
            painter->drawText(QRectF(10,-10,80,20), Qt::AlignLeft | Qt::AlignVCenter, _name);
    }
    else
    {
        QLinearGradient grad(-15,-15,15,15);
        grad.setColorAt(0, QColor("orange"));
        grad.setColorAt(1, QColor("orange").darker());
        painter->setBrush(grad);
        painter->drawPath(path);

        painter->setPen(QPen());
        if(_name != "out" && lod > 0.3)
            painter->drawText(QRectF(-90,-10,80,20), Qt::AlignRight | Qt::AlignVCenter, _name);
    }

    painter->setPen(QPen(Qt::white));
    QFont font = painter->font();
    if(_hover)
        font.setBold(true);
    painter->setFont(font);
    if(_connects.size() > 1 && _direction==Input)
        painter->drawText(QRect(-9,-9,18,18),"M", Qt::AlignCenter | Qt::AlignVCenter);
}

QString BlockPortItem::name() const
{
    return _name;
}

void BlockPortItem::setName(const QString &name)
{
    _name = name;
    update();
}

QString BlockPortItem::blockName() const
{
    return _blockName;
}

void BlockPortItem::setBlockName(const QString &blockName)
{
    _blockName = blockName;
}

BlockPortItem::Direction BlockPortItem::direction() const
{
    return _direction;
}

void BlockPortItem::setDirection(const BlockPortItem::Direction &direction)
{
    _direction = direction;
    update();
}

bool topConnectorSort(BlockConnectorItem *c1, BlockConnectorItem *c2)
{
    return c1->outPos().y() < c2->outPos().y();
}

void BlockPortItem::updateShape()
{
    qSort(_connects.begin(), _connects.end(), topConnectorSort);
    prepareGeometryChange();
    foreach (BlockConnectorItem *connectItem, _connects)
    {
        connectItem->updateShape(this);
    }
}

void BlockPortItem::addConnect(BlockConnectorItem *connectItem)
{
    prepareGeometryChange();
    _connects.append(connectItem);
    updateShape();
}

void BlockPortItem::removeConnect(BlockConnectorItem *connectItem)
{
    prepareGeometryChange();
    _connects.removeOne(connectItem);
    updateShape();
}

const QList<BlockConnectorItem *> &BlockPortItem::connects() const
{
    return _connects;
}

QPointF BlockPortItem::connectorPos(BlockConnectorItem *connectorItem) const
{
    if(_connects.size() > 1 && _direction==Input)
    {
        int h = 15;
        if(_connects.size()>2)
            h = 20;
        int id = _connects.indexOf(connectorItem);
        int t = (-h / 2) + (h / (_connects.size() - 1)) * id;
        return scenePos()+QPointF(-7,t);
    }
    if(_direction==Input)
        return scenePos()+QPointF(-7,0);
    else
        return scenePos()+QPointF(4,0);
}

int BlockPortItem::connectorId(BlockConnectorItem *connectorItem) const
{
    if(_connects.size() > 1 && _direction==Input)
        return _connects.indexOf(connectorItem);
    return 0;
}

BlockPortItem *BlockPortItem::fromModelFlow(ModelFlow *modelFlow)
{
    BlockPortItem *item = new BlockPortItem();

    item->_name = modelFlow->name();
    if(modelFlow->type()=="in")
        item->_direction = Input;
    else
        item->_direction = Output;

    item->setModelFlow(modelFlow);

    return item;
}

void BlockPortItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    prepareGeometryChange();
    _hover = true;
    update();
    QGraphicsItem::hoverEnterEvent(event);
}

void BlockPortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    prepareGeometryChange();
    _hover = false;
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

ModelFlow *BlockPortItem::modelFlow() const
{
    return _modelFlow;
}

void BlockPortItem::setModelFlow(ModelFlow *modelFlow)
{
    prepareGeometryChange();
    _modelFlow = modelFlow;
    update();
}
