/****************************************************************************
** Copyright (C) 2016 Dream IP
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

#ifndef BLOCKSCENE_H
#define BLOCKSCENE_H

#include "gpstudio_gui_common.h"

#include <QGraphicsScene>
#include "blockitem.h"

#include "lib_parser/lib.h"

#include "model/model_node.h"
#include "camera/camera.h"

class ModelFlow;
class ModelFlowConnect;
class BlockConnectorItem;

class GPSTUDIO_GUI_EXPORT BlockScene : public QGraphicsScene
{
    Q_OBJECT
public:
    BlockScene();
    ~BlockScene();

    bool loadFromNode(const ModelNode *node);
    bool loadFromCamera(const Camera *camera);

    void addBlock(ModelBlock *blockModel);
    void addBlock(Block *block);
    void removeBlock(ModelBlock *blockModel);
    void removeBlock(const QString &block_name);

    QList<BlockItem *> block(const QString &name) const;
    BlockPortItem *port(const QString &blockName, const QString &portName) const;
    QList<BlockItem *> block(ModelBlock *modelBlock) const;

    void connectBlockPort(const ModelFlowConnect &flowConnect);
    void connectBlockPort(ModelFlow *fromflow, ModelFlow *toflow);
    void connectBlockPort(const QString &fromblock, const QString &fromflow, const QString &toblock, const QString &toflow);
    void connectBlockPorts(const QList<ModelFlowConnect *> &connections);

    void disconnectBlockPort(const ModelFlowConnect &flowConnect);
    void disconnectBlockPort(ModelFlow *fromFlow, ModelFlow *toFlow);
    void disconnectBlockPort(const QString &fromblock, const QString &fromflow, const QString &toblock, const QString &toflow);

    BlockConnectorItem *getConnector(const ModelFlowConnect &flowConnect);
    BlockConnectorItem *getConnector(const QString &fromblock, const QString &fromflow, const QString &toblock, const QString &toflow);

protected:
    QMap<QString, BlockItem* > _blocksName;
    friend class BlockView;
    void updateKeyBlock(BlockItem *block, const QString &oldKey, const QString &newKey);
    QMap<ModelBlock*, BlockItem* > _blocksModel;

    void connectBlockPort(BlockPortItem *fromflowItem, BlockPortItem *toflowItem);
};

#endif // BLOCKSCENE_H
