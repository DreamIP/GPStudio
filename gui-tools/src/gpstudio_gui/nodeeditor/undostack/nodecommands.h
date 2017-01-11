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

#ifndef NODECOMMANDS_H
#define NODECOMMANDS_H

#include "gpstudio_gui_common.h"

#include <QUndoCommand>

#include "nodeeditor/gpnodeproject.h"

class GPSTUDIO_GUI_EXPORT NodeCommand : public QUndoCommand
{
public:
    NodeCommand(GPNodeProject *project);
protected:
    GPNodeProject *_project;
};

class GPSTUDIO_GUI_EXPORT NodeCmdRename : public NodeCommand
{
public:
    enum { Id = 0x0201 };
    NodeCmdRename(GPNodeProject *project, const QString &oldName, const QString &newName);
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    QString _oldName;
    QString _newName;
};

class GPSTUDIO_GUI_EXPORT NodeCmdConfigBoard : public NodeCommand
{
public:
    enum { Id = 0x0202 };
    NodeCmdConfigBoard(GPNodeProject *project,
                       const QString &oldBoard, const QStringList &oldIos,
                       const QString &newBoard, const QStringList &newIos);
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    QString _oldBoard;
    QString _newBoard;
    QStringList _oldIos;
    QStringList _newIos;
};

#endif // NODECOMMANDS_H
