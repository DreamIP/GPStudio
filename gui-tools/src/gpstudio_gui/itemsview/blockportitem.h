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

#ifndef BLOCKPORTITEM_H
#define BLOCKPORTITEM_H

#include "gpstudio_gui_common.h"

#include <QGraphicsItem>

class BlockConnectorItem;
class ModelFlow;

class GPSTUDIO_GUI_EXPORT BlockPortItem : public QGraphicsItem
{
public:
    BlockPortItem();
    ~BlockPortItem();

    enum { Type = UserType + 3 };
    int type() const;

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString name() const;
    void setName(const QString &name);

    QString blockName() const;
    void setBlockName(const QString &blockName);

    enum Direction {
        Input,
        Output
    };
    Direction direction() const;
    void setDirection(const Direction &direction);

    void updateShape();
    void addConnect(BlockConnectorItem *connectItem);
    void removeConnect(BlockConnectorItem *connectItem);

    const QList<BlockConnectorItem *> &connects() const;

    QPointF connectorPos(BlockConnectorItem *connectorItem) const;
    int connectorId(BlockConnectorItem *connectorItem) const;

    ModelFlow *modelFlow() const;
    void setModelFlow(ModelFlow *modelFlow);

public:
    static BlockPortItem *fromModelFlow(ModelFlow *modelFlow);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QString _name;
    QString _blockName;
    Direction _direction;
    ModelFlow *_modelFlow;
    bool _hover;

    QList<BlockConnectorItem *> _connects;
    QList<BlockConnectorItem *> _oldConnects;
};

#endif // BLOCKPORTITEM_H
