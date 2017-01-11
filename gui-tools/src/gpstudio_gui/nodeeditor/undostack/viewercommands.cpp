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

#include "viewercommands.h"

#include "model/model_gpviewer.h"
#include "model/model_viewer.h"
#include "model/model_viewerflow.h"

ViewerCommand::ViewerCommand(GPNodeProject *project, const QString &viewerName)
    : _project(project), _viewerName(viewerName)
{
}

// Rename viewer
ViewerCmdRename::ViewerCmdRename(GPNodeProject *project, const QString &oldName, const QString &newName)
    : ViewerCommand(project, oldName), _newName(newName)
{
    setText(QString("renamed viewer '%1'").arg(newName));
}

void ViewerCmdRename::undo()
{
    _project->cmdRenameViewer(_newName, _viewerName);
}

void ViewerCmdRename::redo()
{
    _project->cmdRenameViewer(_viewerName, _newName);
}

// Add viewer
ViewerCmdAdd::ViewerCmdAdd(GPNodeProject *project, ModelViewer *viewer)
    : ViewerCommand(project, viewer->name()), _viewer(viewer)
{
    _backupViewer = viewer;
    setText(QString("add viewer '%1'").arg(viewer->name()));
}

ViewerCmdAdd::~ViewerCmdAdd()
{
    delete _backupViewer;
}

void ViewerCmdAdd::undo()
{
    ModelViewer *viewer = _project->node()->gpViewer()->getViewer(_viewerName);
    if(viewer)
    {
        _backupViewer = new ModelViewer(*viewer);
    }
    _project->cmdRemoveViewer(_viewerName);
}

void ViewerCmdAdd::redo()
{
    _viewer = _backupViewer;
    _backupViewer = NULL;
    _project->cmdAddViewer(_viewer);
}

// Remove viewer
ViewerCmdRemove::ViewerCmdRemove(GPNodeProject *project, ModelViewer *viewer)
    : ViewerCommand(project, viewer->name()), _viewer(viewer)
{
    setText(QString("remove viewer '%1'").arg(viewer->name()));
}

ViewerCmdRemove::~ViewerCmdRemove()
{
    delete _backupViewer;
}

void ViewerCmdRemove::undo()
{
    _viewer = _backupViewer;
    _backupViewer = NULL;
    _project->cmdAddViewer(_viewer);
}

void ViewerCmdRemove::redo()
{
    ModelViewer *viewer = _project->node()->gpViewer()->getViewer(_viewerName);
    if(viewer)
    {
        _backupViewer = new ModelViewer(*viewer);
    }
    _project->cmdRemoveViewer(_viewerName);
}

// Add viewerFlow
ViewerFlowCmdAdd::ViewerFlowCmdAdd(GPNodeProject *project, const QString &viewerName, ModelViewerFlow *viewerFlow)
    : ViewerCommand(project, viewerName), _viewerFlowName(viewerFlow->flowName()), _viewerFlow(viewerFlow)
{
    _backupViewerFlow = viewerFlow;
    setText(QString("add viewer flow '%1' to viewer '%2'").arg(viewerFlow->flowName()).arg(_viewerName));
}

ViewerFlowCmdAdd::~ViewerFlowCmdAdd()
{
    delete _backupViewerFlow;
}

void ViewerFlowCmdAdd::undo()
{
    ModelViewerFlow *viewerFlow = _project->node()->gpViewer()->getViewerFlow(_viewerName, _viewerFlowName);
    if(viewerFlow)
    {
        _backupViewerFlow = new ModelViewerFlow(*viewerFlow);
        _project->cmdRemoveViewerFlow(_viewerName, _viewerFlowName);
    }
}

void ViewerFlowCmdAdd::redo()
{
    _viewerFlow = _backupViewerFlow;
    _backupViewerFlow = NULL;
    _project->cmdAddViewerFlow(_viewerName, _viewerFlow);
}

// Remove viewerFlow
ViewerFlowCmdRemove::ViewerFlowCmdRemove(GPNodeProject *project, ModelViewerFlow *viewerFlow)
    : ViewerCommand(project, ""), _viewerFlowName(viewerFlow->flowName()), _viewerFlow(viewerFlow)
{
    if(viewerFlow->viewer())
        _viewerName = viewerFlow->viewer()->name();
    setText(QString("remove viewer '%1'").arg(viewerFlow->flowName()));
}

ViewerFlowCmdRemove::~ViewerFlowCmdRemove()
{
    delete _backupViewerFlow;
}

void ViewerFlowCmdRemove::undo()
{
    _viewerFlow = _backupViewerFlow;
    _backupViewerFlow = NULL;
    _project->cmdAddViewerFlow(_viewerName, _viewerFlow);
}

void ViewerFlowCmdRemove::redo()
{
    ModelViewerFlow *viewerFlow = _project->node()->gpViewer()->getViewerFlow(_viewerName, _viewerFlowName);
    if(viewerFlow)
    {
        _backupViewerFlow = new ModelViewerFlow(*viewerFlow);
    }
    _project->cmdRemoveViewerFlow(_viewerName, _viewerFlowName);
}
