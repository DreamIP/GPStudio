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

#ifndef BLOCKCOMMANDS_H
#define BLOCKCOMMANDS_H

#include "gpstudio_gui_common.h"

#include <QUndoCommand>

#include "nodeeditor/gpnodeproject.h"
#include "model/model_block.h"

#include <model/model_flowconnect.h>

class GPSTUDIO_GUI_EXPORT BlockCommand : public QUndoCommand
{
public:
    BlockCommand(GPNodeProject *project, const QString &block_name=QString());

protected:
    GPNodeProject *_project;
    QString _block_name;
};

class GPSTUDIO_GUI_EXPORT BlockCmdRename : public BlockCommand
{
public:
    enum { Id = 0x0101 };
    BlockCmdRename(GPNodeProject *project, const QString &oldName, const QString &newName);
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    QString _newName;
};

class GPSTUDIO_GUI_EXPORT BlockCmdMove : public BlockCommand
{
public:
    enum { Id = 0x0102 };
    BlockCmdMove(GPNodeProject *project, const QString &block_name, const QString &part_name, const QPoint &oldPos, const QPoint &newPos);
    void undo();
    void redo();
    int id() const { return Id; }
    bool mergeWith(const QUndoCommand *other);

protected:
    QString _part_name;
    QPoint _oldPos;
    QPoint _newPos;
};

class GPSTUDIO_GUI_EXPORT BlockCmdAdd : public BlockCommand
{
public:
    enum { Id = 0x0103 };
    BlockCmdAdd(GPNodeProject *project, ModelBlock *block);
    ~BlockCmdAdd();
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    ModelBlock *_block;
    ModelBlock *_backupBlock;
};

class GPSTUDIO_GUI_EXPORT BlockCmdRemove : public BlockCommand
{
public:
    enum { Id = 0x0104 };
    BlockCmdRemove(GPNodeProject *project, ModelBlock *block);
    ~BlockCmdRemove();
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    ModelBlock *_block;
    ModelBlock *_backupBlock;
    QList<ModelFlowConnect> _flowConnects;
};

class GPSTUDIO_GUI_EXPORT BlockCmdConnectFlow : public BlockCommand
{
public:
    enum { Id = 0x0105 };
    BlockCmdConnectFlow(GPNodeProject *project, const ModelFlowConnect &flowConnect);
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    ModelFlowConnect _flowConnect;
};

class GPSTUDIO_GUI_EXPORT BlockCmdDisconnectFlow : public BlockCommand
{
public:
    enum { Id = 0x0106 };
    BlockCmdDisconnectFlow(GPNodeProject *project, const ModelFlowConnect &flowConnect);
    void undo();
    void redo();
    int id() const { return Id; }

protected:
    ModelFlowConnect _flowConnect;
};

class GPSTUDIO_GUI_EXPORT BlockCmdParamSet : public BlockCommand
{
public:
    enum { Id = 0x0107 };
    BlockCmdParamSet(GPNodeProject *project, const QString &blockName, const QString &paramName, const QVariant &oldValue, const QVariant &newValue);
    void undo();
    void redo();
    int id() const { return Id; }
    bool mergeWith(const QUndoCommand *other);

protected:
    QString _paramName;
    QVariant _oldValue;
    QVariant _newValue;
};

#endif // BLOCKCOMMANDS_H
