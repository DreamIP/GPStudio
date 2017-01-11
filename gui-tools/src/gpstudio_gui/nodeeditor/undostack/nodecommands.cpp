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

#include "nodecommands.h"

NodeCommand::NodeCommand(GPNodeProject *project)
    : _project(project)
{
}

// Rename node
NodeCmdRename::NodeCmdRename(GPNodeProject *project, const QString &oldName, const QString &newName)
    : NodeCommand(project), _oldName(oldName), _newName(newName)
{
    setText(QString("renamed node '%1'").arg(newName));
}

void NodeCmdRename::undo()
{
    _project->cmdRenameNode(_oldName);
}

void NodeCmdRename::redo()
{
    _project->cmdRenameNode(_newName);
}

// ConfigBoard
NodeCmdConfigBoard::NodeCmdConfigBoard(GPNodeProject *project,
                                       const QString &oldBoard, const QStringList &oldIos,
                                       const QString &newBoard, const QStringList &newIos)
    : NodeCommand(project),
      _oldBoard(oldBoard), _newBoard(newBoard),
      _oldIos(oldIos), _newIos(newIos)
{
    setText(QString("replce board with '%1'").arg(newBoard));
}

void NodeCmdConfigBoard::undo()
{
    _project->cmdConfigBoard(_oldBoard, _oldIos);
}

void NodeCmdConfigBoard::redo()
{
    _project->cmdConfigBoard(_newBoard, _newIos);
}
