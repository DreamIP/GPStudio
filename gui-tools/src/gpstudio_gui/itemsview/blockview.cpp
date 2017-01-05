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

#include "blockview.h"

#include <QDebug>
#include <QMouseEvent>
#include <QMimeData>
#include <qmath.h>
#include <QMenu>

#include "blockitem.h"
#include "blockconnectoritem.h"
#include "blockportitem.h"

BlockView::BlockView(QWidget *parent)
    : QGraphicsView(parent)
{
    _project = NULL;
    _editMode = false;
    _scene = new BlockScene();
    scale(0.75, 0.75);

    _startConnectItem = NULL;
    _lineConector = NULL;
    _rectSelect = NULL;

    setScene(_scene);

    setAcceptDrops(true);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::TextAntialiasing, true);

    setDragMode(QGraphicsView::RubberBandDrag);

    connect(_scene, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));
}

BlockView::~BlockView()
{
}

void BlockView::attachProject(GPNodeProject *project)
{
    if(_project)
        disconnect(_project);

    _project = project;

    connect(_project, SIGNAL(nodeChanged(ModelNode*)),
            this, SLOT(changeNode(ModelNode*)));
    connect(_project, SIGNAL(blockUpdated(ModelBlock*)),
            this, SLOT(updateBlock(ModelBlock*)));
    connect(_project, SIGNAL(blockAdded(ModelBlock*)),
            this, SLOT(addBlock(ModelBlock*)));
    connect(_project, SIGNAL(blockRemoved(QString)),
            this, SLOT(removeBlock(QString)));
    connect(_project, SIGNAL(blockConnected(ModelFlowConnect)),
            this, SLOT(connectBlock(ModelFlowConnect)));
    connect(_project, SIGNAL(blockDisconected(ModelFlowConnect)),
            this, SLOT(disconnectBlock(ModelFlowConnect)));

    connect(this, SIGNAL(blockAdded(QString,QPoint)),
            _project, SLOT(addBlock(QString,QPoint)));
    connect(this, SIGNAL(blockRenamed(QString,QString)),
            _project, SLOT(renameBlock(QString,QString)));
    connect(this, SIGNAL(blockMoved(QString,QString,QPoint,QPoint)),
            _project, SLOT(moveBlock(QString,QString,QPoint,QPoint)));
    connect(this, SIGNAL(blockDeleted(ModelBlock*)),
            _project, SLOT(removeBlock(ModelBlock*)));
    connect(this, SIGNAL(blockPortConnected(ModelFlowConnect)),
            _project, SLOT(connectBlockFlows(ModelFlowConnect)));
    connect(this, SIGNAL(blockPortDisconnected(ModelFlowConnect)),
            _project, SLOT(disConnectBlockFlows(ModelFlowConnect)));

    connect(this, SIGNAL(beginMacroAsked(QString)),
            _project, SLOT(beginMacro(QString)));
    connect(this, SIGNAL(endMacroAsked()),
            _project, SLOT(endMacro()));

    if(project->node())
    {
        changeNode(project->node());
    }
}

void BlockView::dragEnterEvent(QDragEnterEvent *event)
{
    QGraphicsView::dragEnterEvent(event);
    if(_editMode)
    {
        if(event->mimeData()->hasFormat("ip/process"))
            event->accept();
    }
}

void BlockView::dragMoveEvent(QDragMoveEvent *event)
{
    QGraphicsView::dragMoveEvent(event);
    if(_editMode)
    {
        if(event->mimeData()->hasFormat("ip/process"))
            event->accept();
    }
}

void BlockView::dropEvent(QDropEvent *event)
{
    QGraphicsView::dropEvent(event);
    if(_editMode)
    {
        QString driver = event->mimeData()->data("ip/process");
        QPoint pos = mapToScene(event->pos()).toPoint();

        QGraphicsItem *item = scene()->itemAt(pos, QTransform());
        BlockConnectorItem *connectorItem = qgraphicsitem_cast<BlockConnectorItem*>(item);
        if(connectorItem)
        {
            BlockLib *blockLib = Lib::getLib().process(driver);
            if(!blockLib)
                return;
            if(blockLib->modelProcess()->flowsIn().empty())
                return;
            QString flowInName = blockLib->modelProcess()->flowsIn().first()->name();
            if(blockLib->modelProcess()->flowsOut().empty())
                return;
            QString flowOutName = blockLib->modelProcess()->flowsOut().first()->name();

            emit beginMacroAsked("Added block on connect");

            ModelFlowConnect oldConnect = ModelFlowConnect(connectorItem->portItem1()->blockName(),
                                                           connectorItem->portItem1()->name(),
                                                           connectorItem->portItem2()->blockName(),
                                                           connectorItem->portItem2()->name());
            emit blockPortDisconnected(oldConnect);

            QString newBlockName = _project->newBlockName(driver);
            emit blockAdded(driver, pos);

            ModelFlowConnect connect1 = ModelFlowConnect(oldConnect.fromblock(),
                                                         oldConnect.fromflow(),
                                                         newBlockName,
                                                         flowInName);
            emit blockPortConnected(connect1);

            ModelFlowConnect connect2 = ModelFlowConnect(newBlockName,
                                                         flowOutName,
                                                         oldConnect.toblock(),
                                                         oldConnect.toflow());
            emit blockPortConnected(connect2);

            emit endMacroAsked();

        }
        else
            emit blockAdded(driver, pos);
    }
}

void BlockView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        BlockPortItem *processItem = qgraphicsitem_cast<BlockPortItem*>(itemAt(event->pos()));
        if(_editMode && processItem)
        {
            _startConnectItem = processItem;
            _lineConector = new BlockConnectorItem(_startConnectItem);
            blockScene()->addItem(_lineConector);
            _lineConector->setEndPos(mapToScene(event->pos()).toPoint());
        }
        if(!_editMode && processItem)
        {
            emit blockSelected("fi");
        }
        setDragMode(QGraphicsView::RubberBandDrag);
    }
    if(event->button() == Qt::MidButton)
    {
        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::ClosedHandCursor);

        _refDrag = mapToScene(event->pos());
        _centerDrag = mapToScene(this->viewport()->rect()).boundingRect().center();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void BlockView::mouseMoveEvent(QMouseEvent *event)
{
    if(_startConnectItem)
        _lineConector->setEndPos(mapToScene(event->pos()).toPoint());

    if ((event->buttons() & Qt::MidButton) == Qt::MidButton)
    {
        setCursor(Qt::ClosedHandCursor);
        QPointF move = _refDrag - mapToScene(event->pos());
        centerOn(_centerDrag + move);
        _centerDrag = mapToScene(viewport()->rect()).boundingRect().center();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void BlockView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if(event->button() == Qt::MidButton)
    {
        setCursor(Qt::ArrowCursor);
    }

    // move blocks
    QList<QPair<BlockItem*,QPoint> > movedBlocks;
    foreach (QGraphicsItem *item, _scene->selectedItems())
    {
        BlockItem *blockItem = qgraphicsitem_cast<BlockItem*>(item);
        if(blockItem)
            if(blockItem->pos() != blockItem->modelPart()->pos())
                movedBlocks.append(qMakePair(blockItem, blockItem->pos().toPoint()));
    }
    if(movedBlocks.size()>1)
        emit beginMacroAsked("multiple blocks moved");
    for (int i=0; i<movedBlocks.size(); i++)
    {
        QPair<BlockItem*,QPoint> pairMove = movedBlocks.at(i);
        BlockItem *blockItem = pairMove.first;
        emit blockMoved(blockItem->name(), blockItem->modelPart()->name(), blockItem->modelPart()->pos(), pairMove.second);
    }
    if(movedBlocks.size()>1)
        emit endMacroAsked();

    // draw connexions
    if(_startConnectItem)
    {
        BlockPortItem *processItem = qgraphicsitem_cast<BlockPortItem*>(itemAt(event->pos()));
        if(processItem && processItem!=_startConnectItem
                && _startConnectItem->modelFlow()->type() != processItem->modelFlow()->type()
                && _startConnectItem->modelFlow()->parent() != processItem->modelFlow()->parent())
        {
            ModelFlowConnect connect(_startConnectItem->blockName(), _startConnectItem->name(),
                                     processItem->blockName(),       processItem->name());
            if(_scene->getConnector(connect)==NULL)
                emit blockPortConnected(connect);
        }

        _lineConector->disconnectPorts();
        scene()->removeItem(_lineConector);
        delete _lineConector;
        _startConnectItem = NULL;
        _lineConector = NULL;
    }
}

void BlockView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);

    QGraphicsItem *item = _scene->itemAt(mapToScene(event->pos()), QTransform());
    BlockItem *blockItem = qgraphicsitem_cast<BlockItem *>(item);
    if(blockItem)
        emit blockDetailsRequest(blockItem->name());
}

void BlockView::updateSelection()
{
    if(_scene->selectedItems().count()==0)
    {
        emit blockSelected("");
        return;
    }

    QString selectedBlocksName;
    foreach (QGraphicsItem *item, _scene->selectedItems())
    {
        BlockItem *blockItem = qgraphicsitem_cast<BlockItem *>(item);
        if(blockItem)
            selectedBlocksName.append(blockItem->name()+";");
    }
    if(!selectedBlocksName.isEmpty())
        emit blockSelected(selectedBlocksName);
}

void BlockView::selectBlock(QString blocksName)
{
    _scene->blockSignals(true);
    _scene->clearSelection();

    foreach (QString blockName, blocksName.split(";"))
    {
        foreach(BlockItem *blockItem, _scene->block(blockName))
        {
            blockItem->setSelected(true);
            blockItem->ensureVisible();
        }
    }

    _scene->blockSignals(false);
}

void BlockView::updateBlock(ModelBlock *block)
{
    foreach(BlockItem *blockItem, _scene->block(block))
    {
        blockItem->updatePos();
        if(blockItem->name() != block->name())
        {
            _scene->updateKeyBlock(blockItem, blockItem->name(), block->name());
            blockItem->setName(block->name());
        }
    }
}

void BlockView::addBlock(ModelBlock *block)
{
    _scene->addBlock(block);
}

void BlockView::removeBlock(const QString &block_name)
{
    _scene->removeBlock(block_name);
}

void BlockView::connectBlock(const ModelFlowConnect &flowConnect)
{
    _scene->connectBlockPort(flowConnect);
}

void BlockView::disconnectBlock(const ModelFlowConnect &flowConnect)
{
    _scene->disconnectBlockPort(flowConnect);
}

void BlockView::changeNode(ModelNode *node)
{
    loadFromNode(node);
}

void BlockView::setBlockScene(BlockScene *scene)
{
    _scene = scene;
    setScene(scene);
}

BlockScene *BlockView::blockScene() const
{
    return _scene;
}

bool BlockView::loadFromNode(const ModelNode *node)
{
    return _scene->loadFromNode(node);
}

bool BlockView::loadFromCam(const Camera *camera)
{
    return _scene->loadFromCamera(camera);
}

void BlockView::zoomIn()
{
    setZoomLevel(1);
}

void BlockView::zoomOut()
{
    setZoomLevel(-1);
}

void BlockView::zoomFit()
{
    fitInView(_scene->itemsBoundingRect().adjusted(-20, -20, 20, 20), Qt::KeepAspectRatio);
}

bool BlockView::editMode() const
{
    return _editMode;
}

void BlockView::setEditMode(bool editMode)
{
    _editMode = editMode;
}

void BlockView::setZoomLevel(int step)
{
    double scaleLvl = qPow(1.2, step);
    double zoom = transform().m22();
    if(scaleLvl>1 && zoom>8)
        return;
    if(scaleLvl<1 && zoom<0.2)
        return;
    scale(scaleLvl, scaleLvl);
}

void BlockView::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    setZoomLevel(numSteps);
}

void BlockView::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Plus)
        zoomIn();
    if(event->key()==Qt::Key_Minus)
        zoomOut();
    if(event->key()==Qt::Key_Asterisk)
        zoomFit();
    if(event->key()==Qt::Key_A && event->modifiers() & Qt::ControlModifier && event->modifiers() & Qt::ShiftModifier) // select all
    {
        scene()->clearSelection();
        return;
    }
    if(event->key()==Qt::Key_A && event->modifiers() & Qt::ControlModifier) // select all
    {
        blockSignals(true);
        foreach(QGraphicsItem *item, scene()->items())
            item->setSelected(true);
        blockSignals(false);
        updateSelection();
    }
    if(event->key()==Qt::Key_F2)
    {
        if(_scene->selectedItems().count()==1)
        {
            QGraphicsItem *item = _scene->selectedItems().at(0);
            BlockItem *blockItem = qgraphicsitem_cast<BlockItem *>(item);
            if(blockItem)
            {
                if(!blockItem->modelBlock()->isIO())
                    emit blockRenamed(blockItem->name(), "");
            }
        }
    }
    if(event->key()==Qt::Key_Delete || event->key()==Qt::Key_Backspace)
    {
        QList<ModelBlock*> block2delete;
        QList<ModelFlowConnect> link2delete;
        foreach (QGraphicsItem *item, _scene->selectedItems())
        {
            BlockItem *blockItem = qgraphicsitem_cast<BlockItem *>(item);
            if(blockItem)
            {
                if(!blockItem->modelBlock()->isIO())
                    block2delete.append(blockItem->modelBlock());
            }
            else
            {
                BlockConnectorItem *connectorItem = qgraphicsitem_cast<BlockConnectorItem *>(item);
                if(connectorItem)
                    link2delete.append(ModelFlowConnect(connectorItem->portItem1()->blockName(),
                                                        connectorItem->portItem1()->name(),
                                                        connectorItem->portItem2()->blockName(),
                                                        connectorItem->portItem2()->name()));
            }
        }
        foreach (const ModelFlowConnect &connect, link2delete)
        {
            foreach (ModelBlock *block, block2delete)
            {
                if(connect.fromblock()==block->name() || connect.toblock()==block->name())
                {
                    link2delete.removeOne(connect);
                }
            }
        }
        if(block2delete.count()>1)
            emit beginMacroAsked("multiple blocks suppression");
        else if(link2delete.count()>1)
            emit beginMacroAsked("multiple links suppression");
        foreach (ModelBlock *block, block2delete)
            emit blockDeleted(block);
        foreach (ModelFlowConnect connect, link2delete)
            emit blockPortDisconnected(connect);

        if(block2delete.count()>1 || link2delete.count()>1)
            emit endMacroAsked();
    }
    if(event->key()==Qt::Key_Up || event->key()==Qt::Key_Down || event->key()==Qt::Key_Left || event->key()==Qt::Key_Right)
    {
        int dec = 20;
        QPoint move;
        if(event->key()==Qt::Key_Up)
            move.setY(-dec);
        if(event->key()==Qt::Key_Down)
            move.setY(dec);
        if(event->key()==Qt::Key_Left)
            move.setX(-dec);
        if(event->key()==Qt::Key_Right)
            move.setX(dec);

        QList<BlockItem*> movedBlocks;
        foreach (QGraphicsItem *item, _scene->selectedItems())
        {
            BlockItem *blockItem = qgraphicsitem_cast<BlockItem *>(item);
            if(blockItem)
                movedBlocks.append(blockItem);
        }
        if(movedBlocks.size()>1)
            emit beginMacroAsked("multiple blocks moved");
        foreach (BlockItem *blockItem, movedBlocks)
            emit blockMoved(blockItem->name(), blockItem->modelPart()->name(), blockItem->modelPart()->pos(), blockItem->modelPart()->pos()+move);
        if(movedBlocks.size()>1)
            emit endMacroAsked();
    }

    QGraphicsView::keyPressEvent(event);
}

#ifndef QT_NO_CONTEXTMENU
void BlockView::contextMenuEvent(QContextMenuEvent *event)
{
    QGraphicsItem *item;
    item = _scene->itemAt(mapToScene(event->pos()), QTransform());
    if(item)
    {
        if(!scene()->selectedItems().count()==1 || !item->isSelected())
        {
            scene()->clearSelection();
            item->setSelected(true);
        }

        BlockItem *blockItem = qgraphicsitem_cast<BlockItem *>(item);
        if(blockItem)
        {
            QMenu menu;
            QAction *renameAction=NULL, *deleteAction=NULL;
            if(_editMode)
            {
                renameAction = menu.addAction("Rename");
                renameAction->setShortcut(Qt::Key_F2);
                deleteAction = menu.addAction("Delete");
                deleteAction->setShortcut(Qt::Key_Delete);
                if(blockItem->modelBlock()->isIO())
                {
                    renameAction->setEnabled(false);
                    deleteAction->setEnabled(false);
                }
            }
            QAction *infosIPAction = menu.addAction("View implementation files");
            QFont boldFont = infosIPAction->font();
            boldFont.setBold(true);
            infosIPAction->setFont(boldFont);
            QAction *trigered = menu.exec(event->globalPos());
            if(trigered == deleteAction)
                emit blockDeleted(blockItem->modelBlock());
            else if(trigered == renameAction)
                emit blockRenamed(blockItem->name(), "");
            else if(trigered == infosIPAction)
                emit blockDetailsRequest(blockItem->name());
            return;
        }

        BlockConnectorItem *blockConnectorItem = qgraphicsitem_cast<BlockConnectorItem *>(item);
        if(blockConnectorItem)
        {
            QMenu menu;
            QAction *deleteAction=NULL;
            if(_editMode)
            {
                deleteAction = menu.addAction("Delete");
                deleteAction->setShortcut(Qt::Key_Delete);
            }
            else
                return;
            QAction *trigered = menu.exec(event->globalPos());
            if(trigered == deleteAction)
            {
                ModelFlowConnect connect = ModelFlowConnect(blockConnectorItem->portItem1()->blockName(),
                                                            blockConnectorItem->portItem1()->name(),
                                                            blockConnectorItem->portItem2()->blockName(),
                                                            blockConnectorItem->portItem2()->name());
                emit blockPortDisconnected(connect);
            }
            return;
        }
    }
}
#endif // QT_NO_CONTEXTMENU
