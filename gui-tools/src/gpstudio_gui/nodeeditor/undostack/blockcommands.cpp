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

#include "blockcommands.h"

#include <QDebug>

#include "model/model_fiblock.h"
#include "model/model_process.h"
#include "model/model_iocom.h"

BlockCommand::BlockCommand(GPNodeProject *project, const QString &block_name)
    : _project(project), _block_name(block_name)
{
}

// Rename block
BlockCmdRename::BlockCmdRename(GPNodeProject *project, const QString &oldName, const QString &newName)
    : BlockCommand(project, oldName), _newName(newName)
{
    setText(QString("renamed block '%1'").arg(newName));
}

void BlockCmdRename::undo()
{
    _project->cmdRenameBlock(_newName, _block_name);
}

void BlockCmdRename::redo()
{
    _project->cmdRenameBlock(_block_name, _newName);
}

// Move block
BlockCmdMove::BlockCmdMove(GPNodeProject *project, const QString &block_name, const QString &part_name, const QPoint &oldPos, const QPoint &newPos)
    : BlockCommand(project, block_name), _part_name(part_name), _oldPos(oldPos), _newPos(newPos)
{
    setText(QString("moved block '%1'").arg(block_name));
}

void BlockCmdMove::undo()
{
    _project->cmdMoveBlockTo(_block_name, _part_name, _oldPos);
}

void BlockCmdMove::redo()
{
    _project->cmdMoveBlockTo(_block_name, _part_name, _newPos);
}

bool BlockCmdMove::mergeWith(const QUndoCommand *other)
{
    if(other->id() != id())
        return false;
    const BlockCmdMove *otherMove = static_cast<const BlockCmdMove*>(other);
    if(otherMove->_block_name != _block_name)
        return false;
    _newPos = otherMove->_newPos;
    return true;
}

// Add block
BlockCmdAdd::BlockCmdAdd(GPNodeProject *project, ModelBlock *block)
    : BlockCommand(project, block->name()), _block(block)
{
    _backupBlock = block;
    setText(QString("add block '%1'").arg(block->name()));
}

BlockCmdAdd::~BlockCmdAdd()
{
    delete _backupBlock;
}

void BlockCmdAdd::undo()
{
    ModelBlock *block = _project->node()->getBlock(_block_name);
    if(block)
    {
        // backup block
        if(block->type()==ModelBlock::Process)
            _backupBlock = new ModelProcess(*static_cast<ModelProcess*>(block));
        else if(block->type()==ModelBlock::IO)
            _backupBlock = new ModelIO(*static_cast<ModelIO*>(block));
        else if(block->type()==ModelBlock::IOCom)
            _backupBlock = new ModelIOCom(*static_cast<ModelIOCom*>(block));
    }

    _project->cmdRemoveBlock(_block_name);
}

void BlockCmdAdd::redo()
{
    _block = _backupBlock;
    _backupBlock = NULL;
    _project->cmdAddBlock(_block);
}

// Remove block
BlockCmdRemove::BlockCmdRemove(GPNodeProject *project, ModelBlock *block)
    : BlockCommand(project, block->name()), _block(block)
{
    setText(QString("remove block '%1'").arg(block->name()));
}

BlockCmdRemove::~BlockCmdRemove()
{
    delete _backupBlock;
}

void BlockCmdRemove::undo()
{
    _block = _backupBlock;
    _project->cmdAddBlock(_block);
    _backupBlock = NULL;

    foreach (ModelFlowConnect flowConnect, _flowConnects)
        _project->cmdConnectFlow(flowConnect);
}

void BlockCmdRemove::redo()
{
    ModelBlock *block = _project->node()->getBlock(_block_name);
    if(block)
    {
        // backup block
        if(block->type()==ModelBlock::Process)
            _backupBlock = new ModelProcess(*static_cast<ModelProcess*>(block));
        else if(block->type()==ModelBlock::IO)
            _backupBlock = new ModelIO(*static_cast<ModelIO*>(block));
        else if(block->type()==ModelBlock::IOCom)
            _backupBlock = new ModelIOCom(*static_cast<ModelIOCom*>(block));
    }

    // backup connection to/from block
    _flowConnects.clear();
    QList<ModelFlowConnect *> _flowConnectsPtr;
    _flowConnectsPtr = _project->node()->getFIBlock()->flowConnects(_block_name);
    foreach (ModelFlowConnect *flowConnect, _flowConnectsPtr)
        _flowConnects.append(*flowConnect);

    _project->cmdRemoveBlock(_block_name);
}

// Flow connection
BlockCmdConnectFlow::BlockCmdConnectFlow(GPNodeProject *project, const ModelFlowConnect &flowConnect)
    : BlockCommand(project), _flowConnect(flowConnect)
{
    setText(QString("connect flow '%1.%2' to '%3.%4'")
            .arg(flowConnect.fromblock()).arg(flowConnect.fromflow())
            .arg(flowConnect.toblock()).arg(flowConnect.toflow()));
}

void BlockCmdConnectFlow::undo()
{
    _project->cmdDisconnectFlow(_flowConnect);
}

void BlockCmdConnectFlow::redo()
{
    _project->cmdConnectFlow(_flowConnect);
}

// Flow disconnection
BlockCmdDisconnectFlow::BlockCmdDisconnectFlow(GPNodeProject *project, const ModelFlowConnect &flowConnect)
    : BlockCommand(project), _flowConnect(flowConnect)
{
    setText(QString("disconnect flow '%1.%2' to '%3.%4'")
            .arg(flowConnect.fromblock()).arg(flowConnect.fromflow())
            .arg(flowConnect.toblock()).arg(flowConnect.toflow()));
}

void BlockCmdDisconnectFlow::undo()
{
    _project->cmdConnectFlow(_flowConnect);
}

void BlockCmdDisconnectFlow::redo()
{
    _project->cmdDisconnectFlow(_flowConnect);
}

// Set param
BlockCmdParamSet::BlockCmdParamSet(GPNodeProject *project, const QString &blockName, const QString &paramName, const QVariant &oldValue, const QVariant &newValue)
    : BlockCommand(project, blockName), _paramName(paramName), _oldValue(oldValue), _newValue(newValue)
{
    setText(QString("change param '%1.%2'").arg(blockName).arg(paramName));
}

void BlockCmdParamSet::undo()
{
    _project->cmdSetParam(_block_name, _paramName, _oldValue);
}

void BlockCmdParamSet::redo()
{
    _project->cmdSetParam(_block_name, _paramName, _newValue);
}

bool BlockCmdParamSet::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id())
        return false;
    const BlockCmdParamSet *otherSetParam = static_cast<const BlockCmdParamSet*>(other);
    if(_block_name != otherSetParam->_block_name)
        return false;
    if(_paramName != otherSetParam->_paramName)
        return false;
    _newValue = otherSetParam->_newValue;
    return true;
}
