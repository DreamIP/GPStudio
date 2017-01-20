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

#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include "gpstudio_gui_common.h"

#include <QGraphicsItem>
#include <QSvgRenderer>
#include <QMap>

class BlockLib;
class IOLib;
class ModelBlock;
class ModelComponentPart;
class Block;

class BlockPortItem;

class GPSTUDIO_GUI_EXPORT BlockItem : public QGraphicsItem
{
public:
    BlockItem();
    ~BlockItem();

    enum { Type = UserType + 1 };
    int type() const;

    QSize size() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString name() const;
    void setName(const QString &name);

    Block *block() const;
    ModelBlock *modelBlock() const;
    ModelComponentPart *modelPart() const;

    void updateBlock();
    void updatePos();

    void addPort(BlockPortItem *portItem);
    BlockPortItem *port(const QString &name) const;
    const QMap<QString, BlockPortItem *> &ports() const;

public:
    // model
    static QList<BlockItem *> fromModelBlock(ModelBlock *modelBlock);
    static BlockItem *fromModelComponentPart(ModelComponentPart *modelPart);

    // camera model
    static QList<BlockItem *> fromBlock(Block *block);


protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QString _name;

    QRectF _boundingRect;
    QSvgRenderer _svgRenderer;

    QMap<QString, BlockPortItem *> _ports;

    Block *_block;
    ModelBlock *_modelBlock;
    ModelComponentPart *_modelPart;
};

#endif // BLOCKITEM_H
