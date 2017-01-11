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

#ifndef VIEWERCOMMANDS_H
#define VIEWERCOMMANDS_H

#include "gpstudio_gui_common.h"

#include <QUndoCommand>

#include "nodeeditor/gpnodeproject.h"

class GPSTUDIO_GUI_EXPORT ViewerCommand : public QUndoCommand
{
public:
    ViewerCommand(GPNodeProject *project, const QString &viewerName=QString());
protected:
    GPNodeProject *_project;
    QString _viewerName;
};

class GPSTUDIO_GUI_EXPORT ViewerCmdRename : public ViewerCommand
{
public:
    enum { Id = 0x0301 };
    ViewerCmdRename(GPNodeProject *project, const QString &oldName, const QString &newName);
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    QString _newName;
};

class GPSTUDIO_GUI_EXPORT ViewerCmdAdd : public ViewerCommand
{
public:
    enum { Id = 0x0302 };
    ViewerCmdAdd(GPNodeProject *project, ModelViewer *viewer);
    ~ViewerCmdAdd();
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    ModelViewer *_viewer;
    ModelViewer *_backupViewer;
};

class GPSTUDIO_GUI_EXPORT ViewerCmdRemove : public ViewerCommand
{
public:
    enum { Id = 0x0303 };
    ViewerCmdRemove(GPNodeProject *project, ModelViewer *viewer);
    ~ViewerCmdRemove();
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    ModelViewer *_viewer;
    ModelViewer *_backupViewer;
};

class GPSTUDIO_GUI_EXPORT ViewerFlowCmdAdd : public ViewerCommand
{
public:
    enum { Id = 0x0304 };
    ViewerFlowCmdAdd(GPNodeProject *project, const QString &viewerName, ModelViewerFlow *viewerFlow);
    ~ViewerFlowCmdAdd();
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    QString _viewerFlowName;
    ModelViewerFlow *_viewerFlow;
    ModelViewerFlow *_backupViewerFlow;
};

class GPSTUDIO_GUI_EXPORT ViewerFlowCmdRemove : public ViewerCommand
{
public:
    enum { Id = 0x0305 };
    ViewerFlowCmdRemove(GPNodeProject *project, ModelViewerFlow *viewerFlow);
    ~ViewerFlowCmdRemove();
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    QString _viewerFlowName;
    ModelViewerFlow *_viewerFlow;
    ModelViewerFlow *_backupViewerFlow;
};

#endif // VIEWERCOMMANDS_H
