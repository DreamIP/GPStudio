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

#ifndef BLOCKVIEW_H
#define BLOCKVIEW_H

#include "gpstudio_gui_common.h"

#include <QGraphicsView>

#include "blockscene.h"
class BlockItem;
class BlockConnectorItem;

#include "model/model_node.h"
#include "lib_parser/lib.h"

#include "nodeeditor/gpnodeproject.h"

class GPSTUDIO_GUI_EXPORT BlockView : public QGraphicsView
{
    Q_OBJECT
public:
    BlockView(QWidget *parent=NULL);
    ~BlockView();

    void attachProject(GPNodeProject *project);

    bool loadFromNode(const ModelNode *node);
    bool loadFromCam(const Camera *camera);

    BlockScene *blockScene() const;
    void setBlockScene(BlockScene *scene);

    bool editMode() const;
    void setEditMode(bool editMode);

    void alignCenter(int align);

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    void setZoomLevel(int step);
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
#endif // QT_NO_CONTEXTMENU

protected slots:
    void updateSelection();

public slots:
    void clearSelection();
    void selectBlock(QString blocksName);
    void changeNode(ModelNode *node);

    void updateBlock(ModelBlock *block);
    void addBlock(ModelBlock *block);
    void removeBlock(const QString &block_name);
    void connectBlock(const ModelFlowConnect &flowConnect);
    void disconnectBlock(const ModelFlowConnect &flowConnect);

    void zoomIn();
    void zoomOut();
    void zoomFit();

    void alignVerticalCenter();
    void alignHorizontalCenter();

    void selectFlowCom(const QString &flowCom);
    void selectFlowCom(const QString &viewer, const QString &viewerFlow);
    //void selectFlowCom(const QStringList &flowComs);

signals:
    void blockDetailsRequest(QString blockName);
    void blockSelected(QString blockName);

    void blockAdded(const QString driver, const QPoint pos);
    void blockRenamed(const QString block_name, const QString newName);
    void blockMoved(const QString block_name, const QString part_name, const QPoint oldPos, const QPoint newPos);
    void blockDeleted(ModelBlock *block);
    void blockPortConnected(ModelFlowConnect flowConnect);
    void blockPortDisconnected(ModelFlowConnect flowConnect);

    void beginMacroAsked(QString text);
    void endMacroAsked();

    void centerAvailable(bool);

private:
    GPNodeProject *_project;
    const ModelNode *_node;

    BlockScene *_scene;
    bool _editMode;

    QPointF _refDrag;
    QPointF _centerDrag;

    // connector system
    QGraphicsRectItem *_rectSelect;
    BlockPortItem *_startConnectItem;
    BlockConnectorItem *_lineConector;
};

#endif // BLOCKVIEW_H
