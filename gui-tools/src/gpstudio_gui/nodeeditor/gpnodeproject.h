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

#ifndef GPNODEPROJECT_H
#define GPNODEPROJECT_H

#include "gpstudio_gui_common.h"

#include "model/model_node.h"

#include <QStringList>
#include <QUndoStack>

#include <model/model_flowconnect.h>

#include <camera/camera.h>
#include <model/model_viewer.h>

class GPSTUDIO_GUI_EXPORT GPNodeProject : public QObject
{
    Q_OBJECT
public:
    GPNodeProject(QObject *parent=NULL);
    ~GPNodeProject();

    QString name() const;
    QString path() const;
    bool isModified() const;
    ModelNode *node() const;
    Camera *camera() const;

    QString newViewerName() const;
    QString newBlockName(const QString &driver) const;
    QPoint newBlockPos() const;

    QUndoStack *undoStack() const;

    QWidget *nodeEditorWindow() const;
    void setNodeEditorWindow(QWidget *nodeEditorWindow);

public slots:
    // project commands
    void newProject();
    bool openProject(const QString &nodeFileName=QString());
    bool saveProject();
    bool saveProjectAs(const QString &nodeFileName=QString());
    bool closeProject();

    void configBoard();

    void renameNode(const QString &oldName, const QString &newName);

    // block commands
    void moveBlock(const QString &block_name, const QString &part_name, const QPoint &oldPos, const QPoint &newPos);
    void renameBlock(const QString &block_name, const QString &newName);
    void addBlock(ModelBlock *block);
    void addBlock(const QString &driver, const QPoint &pos);
    void removeBlock(ModelBlock *block);
    void connectBlockFlows(const ModelFlowConnect &flowConnect);
    void disConnectBlockFlows(const ModelFlowConnect &flowConnect);
    void blockSetParam(const QString &blockName, const QString &paramName, const QVariant &value);

    // viewer commands
    void renameViewer(const QString &viewerName, const QString &newName);
    void addViewer(ModelViewer *viewer);
    void removeViewer(ModelViewer *viewer);
    void addViewerFlow(const QString &viewerName, ModelViewerFlow *viewerFlow);
    void removeViewerFlow(ModelViewerFlow *viewerFlow);

    // macro commands
    void beginMacro(const QString &text);
    void endMacro();

signals:
    void nodeChanged(ModelNode *node);
    void nodePathChanged(QString path);
    void nodeModified(bool modified);

    void blockUpdated(ModelBlock *block);
    void blockAdded(ModelBlock *block);
    void blockRemoved(QString blockName);
    void blockConnected(ModelFlowConnect flowConnect);
    void blockDisconected(ModelFlowConnect flowConnect);

    void viewerUpdated(ModelViewer *viewer);
    void viewerAdded(ModelViewer *viewer);
    void viewerRemoved(QString viewerName);

    void viewerFlowUpdated(ModelViewerFlow *viewerFlow);
    void viewerFlowAdded(ModelViewerFlow *viewerFlow);
    void viewerFlowRemoved(QString viewerName, QString viewerFlowName);

private:
    void setPath(const QString &path);
    QString _path;

    void setNode(ModelNode *node);
    ModelNode *_node;
    Camera *_camera;

    void setModified(bool modified);
    bool _modified;

    QUndoStack *_undoStack;
    QWidget *_nodeEditorWindow;

protected:
    // === commands from undo stack ===
    // block commands
    friend class BlockCmdRename;
    void cmdRenameBlock(const QString &block_name, const QString &newName);

    friend class BlockCmdMove;
    void cmdMoveBlockTo(const QString &block_name, const QString &part_name, QPoint pos);

    friend class BlockCmdAdd;
    friend class BlockCmdRemove;
    void cmdAddBlock(ModelBlock *block);
    void cmdRemoveBlock(const QString &block_name);

    friend class BlockCmdConnectFlow;
    friend class BlockCmdDisconnectFlow;
    void cmdConnectFlow(const ModelFlowConnect &flowConnect);
    void cmdDisconnectFlow(const ModelFlowConnect &flowConnect);

    // node commands
    friend class NodeCmdRename;
    void cmdRenameNode(QString nodeName);

    friend class NodeCmdConfigBoard;
    void cmdConfigBoard(QString boardName, QStringList iosName);

    friend class BlockCmdParamSet;
    void cmdSetParam(const QString &blockName, const QString &paramName, const QVariant &value);

    // viewer commands
    friend class ViewerCmdRename;
    void cmdRenameViewer(const QString &viewerName, QString newName);

    friend class ViewerCmdAdd;
    friend class ViewerCmdRemove;
    void cmdAddViewer(ModelViewer *viewer);
    void cmdRemoveViewer(const QString &viewerName);

    // viewerFlow commands
    friend class ViewerFlowCmdAdd;
    friend class ViewerFlowCmdRemove;
    void cmdAddViewerFlow(const QString &viewerName, ModelViewerFlow *viewerFlow);
    void cmdRemoveViewerFlow(const QString &viewerFlowName, const QString &viewerName);
};

#endif // GPNODEPROJECT_H
