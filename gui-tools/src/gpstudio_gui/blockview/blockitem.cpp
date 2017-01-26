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
#include "blockitem.h"
#include "blockportitem.h"

#include <QPainter>
#include <QDebug>

#include "blockconnectoritem.h"

#include "lib_parser/lib.h"

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QCheckBox>
#include <QSlider>
#include <QStyleOptionGraphicsItem>

#include <model/model_block.h>
#include <camera/block.h>
#include <camera/camera.h>

#include "propertywidgets/propertywidgets.h"

BlockItem::BlockItem()
    : QGraphicsItem()
{
    _block = NULL;
    _modelPart = NULL;
    _modelBlock = NULL;

    setZValue(0);
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsScenePositionChanges, true);
    setCacheMode(QGraphicsItem::NoCache);

    updateBlock();
}

BlockItem::~BlockItem()
{
}

int BlockItem::type() const
{
    return Type;
}

QSize BlockItem::size() const
{
    return _boundingRect.size().toSize();
}

QRectF BlockItem::boundingRect() const
{
    return _boundingRect.adjusted(-2,-2,2,20);
}

void BlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if(isSelected())
    {
        if(_modelBlock->isIO())
            painter->setPen(QPen(Qt::red, 3, Qt::DashLine));
        else
            painter->setPen(QPen(QColor("orange"), 3));
    }
    else
    {
        if(_modelBlock->isIO())
            painter->setPen(QPen(Qt::darkRed, 1, Qt::DashLine));
        else
            painter->setPen(QPen(Qt::black, 1));
    }

    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    if(_svgRenderer.isValid() && lod>0.3)
    {
        _svgRenderer.render(painter, _boundingRect);
        painter->drawRoundedRect(_boundingRect,2,2);
    }
    else
    {
        painter->setBrush(Qt::lightGray);
        painter->drawRect(_boundingRect);
    }

    // block name
    painter->setPen(QPen(Qt::black, 1));
    QRectF textRect = QRectF(0, _boundingRect.height(), _boundingRect.width(), painter->fontMetrics().height() + 3);
    QFont font = painter->font();
    font.setPixelSize(12);
    if(isSelected())
        font.setBold(true);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignRight | Qt::AlignBottom, _name);
}

QString BlockItem::name() const
{
    return _name;
}

void BlockItem::setName(const QString &name)
{
    _name = name;
    update();
    foreach (BlockPortItem *port, _ports)
        port->setBlockName(_name);
}

Block *BlockItem::block() const
{
    return _block;
}

void BlockItem::updateBlock()
{
    prepareGeometryChange();
    if(_modelPart)
        _svgRenderer.load(_modelPart->draw().toUtf8());

    int inCount=0, outCount=0;
    foreach (BlockPortItem *portItem, _ports)
    {
        if(portItem->direction()==BlockPortItem::Output)
            outCount++;
        else
            inCount++;
    }

    if(_svgRenderer.isValid())
        _boundingRect = _svgRenderer.viewBoxF();
    else
    {
        qreal height = qMax(inCount, outCount) * 20 + 30;
        _boundingRect = QRectF(0,0,125,height);
    }
    _boundingRect.setX(0);
    _boundingRect.setY(0);

    float ratioW = 1;
    float ratioH = 1;
    if(_boundingRect.width()>300)
        ratioW = 300.0/_boundingRect.width();
    if(_boundingRect.height()>200)
        ratioH = 200.0/_boundingRect.height();
    float ratio = qMin(ratioW, ratioH);
    _boundingRect.setWidth(_boundingRect.width()*ratio);
    _boundingRect.setHeight(_boundingRect.height()*ratio);

    // port placement
    int inId=0, outId=0;
    foreach (BlockPortItem *portItem, _ports)
    {
        if(portItem->direction()==BlockPortItem::Output)
            portItem->setPos(_boundingRect.width(), (_boundingRect.height()/(outCount+1))*(++outId));
        else
            portItem->setPos(0, (_boundingRect.height()/(inCount+1))*(++inId));
    }
    update();
}

void BlockItem::updatePos()
{
    if(_modelPart)
        setPos(_modelPart->pos());
    foreach (BlockPortItem *portItem, _ports)
    {
        portItem->updateShape();
    }
}

void BlockItem::addPort(BlockPortItem *portItem)
{
    portItem->setParentItem(this);
    portItem->setBlockName(_name);
    _ports.insert(portItem->name(), portItem);
}

BlockPortItem *BlockItem::port(const QString &name) const
{
    QMap<QString, BlockPortItem* >::const_iterator it = _ports.find(name);
    if(it != _ports.end())
        return it.value();
    return NULL;
}

const QMap<QString, BlockPortItem *> &BlockItem::ports() const
{
    return _ports;
}

QVariant BlockItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        foreach (BlockPortItem *portItem, _ports)
        {
            portItem->updateShape();
        }
    }
    if (change == ItemSelectedHasChanged && scene())
    {
        if(isSelected())
        {
            setZValue(1);
            foreach (BlockPortItem *port, _ports)
            {
                foreach (BlockConnectorItem *connectorItem, port->connects())
                {
                    connectorItem->setHighlight(true);
                }
            }
        }
        else
        {
            setZValue(0);
            foreach (BlockPortItem *port, _ports)
            {
                foreach (BlockConnectorItem *connectorItem, port->connects())
                {
                    connectorItem->setHighlight(connectorItem->isSelected());
                }
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

ModelBlock *BlockItem::modelBlock() const
{
    return _modelBlock;
}

ModelComponentPart *BlockItem::modelPart() const
{
    return _modelPart;
}

QList<BlockItem *> BlockItem::fromModelBlock(ModelBlock *modelBlock)
{
    QList<BlockItem *> list;
    if(!modelBlock)
        return list;

    foreach (ModelComponentPart *part, modelBlock->parts())
    {
        BlockItem *item = fromModelComponentPart(part);
        if(item)
        {
            item->_modelBlock = modelBlock;
            list.append(item);
        }
    }

    return list;
}

BlockItem *BlockItem::fromModelComponentPart(ModelComponentPart *modelPart)
{
    if(!modelPart)
        return NULL;

    BlockItem *item = new BlockItem();

    item->setPos(modelPart->pos());
    if(modelPart->parent())
        item->setName(modelPart->parent()->name());
    item->_modelPart = modelPart;

    foreach (ModelFlow *flow, modelPart->parent()->flows())
    {
        if(modelPart->getFlow(flow->name()) || modelPart->flows().size()==0)
            item->addPort(BlockPortItem::fromModelFlow(flow));
    }

    item->updateBlock();

    return item;
}

QList<BlockItem *> BlockItem::fromBlock(Block *block)
{
    QList<BlockItem *> list = fromModelBlock(block->modelBlock());

    foreach (BlockItem * item, list)
    {
        Property *propertyEnable = NULL;
        propertyEnable = block->assocProperty()->path("enable");
        if(propertyEnable)
        {
            QWidget *propertyEnableWidget = PropertyWidget::getWidgetFromProperty(propertyEnable);
            QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(item);
            propertyEnableWidget->setGeometry(5,5,50,20);
            propertyEnableWidget->setAttribute(Qt::WA_NoSystemBackground);
            proxy->setWidget(propertyEnableWidget);
            proxy->setOpacity(0.999);
        }

        foreach (ModelComponentPartProperty *partProperty, item->modelPart()->properties())
        {
            Property *property = block->assocProperty()->path(partProperty->name());
            if(property)
            {
                QWidget *propertyWidget = PropertyWidget::getWidgetFromProperty(property);
                QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(item);
                propertyWidget->setGeometry(QRect(partProperty->pos(), partProperty->size()));
                propertyWidget->setAttribute(Qt::WA_NoSystemBackground);
                proxy->setWidget(propertyWidget);
                proxy->setOpacity(0.999);
            }
        }

        item->_block = block;
    }

    return list;
}
