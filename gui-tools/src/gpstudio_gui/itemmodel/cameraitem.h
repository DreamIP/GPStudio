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

#ifndef CAMERAITEM_H
#define CAMERAITEM_H

#include "gpstudio_gui_common.h"

#include <QList>
#include <QVariant>
#include <QModelIndex>

class Camera;
class Block;
class Flow;
class ModelNode;
class ModelBlock;
class ModelFlow;
class ModelGPViewer;
class ModelViewer;
class ModelViewerFlow;

class GPSTUDIO_GUI_EXPORT CameraItem
{
public:
    CameraItem();
    CameraItem(const Camera *camera, uint filter=FAll);
    CameraItem(const Block *block, uint filter=FAll);
    CameraItem(const Flow *flow, uint filter=FAll);
    CameraItem(const ModelNode *node, uint filter=FAll);
    CameraItem(const ModelBlock *block, uint filter=FAll);
    CameraItem(const ModelFlow *flow, uint filter=FAll);
    CameraItem(const ModelGPViewer *gpViewer, uint filter=FAll);
    CameraItem(const ModelViewer *viewer, uint filter=FAll);
    CameraItem(const ModelViewerFlow *viewerFlow, uint filter=FAll);
    ~CameraItem();

    enum Type {
        CameraType,
        BlockType,
        FlowType,

        ModelNodeType,
        ModelBlockType,
        ModelFlowType,

        ModelGPViewerType,
        ModelViewerType,
        ModelViewerFlowType,

        NullType
    };

    Type type() const;
    void setType(const Type &type);

    const Camera *camera() const;
    const Block *block() const;
    const Flow *flow() const;

    const ModelNode *modelNode() const;
    const ModelBlock *modelBlock() const;
    const ModelFlow *modelFlow() const;

    const ModelGPViewer *modelGPViewer() const;
    const ModelViewer *modelViewer() const;
    const ModelViewerFlow *modelViewerFlow() const;

    const QList<CameraItem*> &childrens() const;
    const CameraItem *children(int i) const;
    CameraItem *children(const QString &name);
    int count() const;
    void clear();

    enum Filter {
        FBlock = 0x01,
        FFlowIn = 0x02,
        FFlowOut = 0x04,
        FFlow = FFlowIn | FFlowOut,
        FAll = FBlock | FFlow
    };

    CameraItem *append(const Camera *camera, uint filter=FAll);
    CameraItem *append(const Block *block, uint filter=FAll);
    CameraItem *append(const Flow *flow, uint filter=FAll);

    CameraItem *append(const ModelNode *node, uint filter=FAll);
    CameraItem *append(const ModelBlock *block, uint filter=FAll);
    CameraItem *append(const ModelFlow *flow, uint filter=FAll);

    CameraItem *append(const ModelGPViewer *gpViewer, uint filter=FAll);
    CameraItem *append(const ModelViewer *viewer, uint filter=FAll);
    CameraItem *append(const ModelViewerFlow *viewerFlow, uint filter=FAll);

    bool insertRow(int row, CameraItem *item);
    bool insertRow(int row, QList<CameraItem *> items);
    bool removeRows(int row, int count);

    int row();
    void *ptr() const;
    CameraItem *parent() const;

    // std interface
    QVariant value(int column, int role=Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
    Type _type;
    const void *_ptr;

    int _row;
    CameraItem *_parent;

    QList<CameraItem*> _childrens;
};

#endif // CAMERAITEM_H
