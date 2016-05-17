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

#include "gpnodeproject.h"

#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>

#include "undostack/blockcommands.h"

GPNodeProject::GPNodeProject(QObject *parent)
    : QObject(parent)
{
    _node = NULL;
    _undoStack = new QUndoStack();
}

GPNodeProject::~GPNodeProject()
{
    delete _undoStack;
    delete _node;
}

QString GPNodeProject::name() const
{
    if(_path.isEmpty())
        return QString("new node");
    else
        return QFileInfo(_path).baseName();
}

QString GPNodeProject::path() const
{
    return _path;
}

bool GPNodeProject::isModified() const
{
    return _modified;
}

ModelNode *GPNodeProject::node() const
{
    return _node;
}

void GPNodeProject::newProject()
{
    closeProject();

    setPath("");
    setNode(new ModelNode());
    setModified(false);
}

bool GPNodeProject::openProject(const QString &nodeFileName)
{
    ModelNode *node;
    QString fileName;

    closeProject();

    if(nodeFileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(0, "Open node project", "", "Node project (*.node)");
        if(fileName.isEmpty())
            return false;
    }
    else
        fileName = nodeFileName;

    if(!QFile::exists(fileName))
    {
        qDebug()<<"Cannot find file "<<fileName;
        return false;
    }

    node = ModelNode::readFromFile(fileName);
    if(!node)
        return false;

    setPath(fileName);
    setModified(false);
    setNode(node);

    return true;
}

bool GPNodeProject::saveProject()
{
    setModified(false);
    return true;
}

bool GPNodeProject::saveProjectAs(const QString &nodeFileName)
{
    setModified(false);
    return true;
}

void GPNodeProject::closeProject()
{
    if(_modified)
    {
        saveProject();
    }

    setNode(NULL);
    delete _node;
}

void GPNodeProject::setPath(const QString &path)
{
    _path = path;
    emit nodePathChanged(_path);
}

void GPNodeProject::setModified(bool modified)
{
    _modified = modified;
    emit nodeModified(_modified);
}

void GPNodeProject::cmdRenameBlock(ModelBlock *block, const QString &name)
{
    block->setName(name);
    emit blockUpdated(block);
}

void GPNodeProject::cmdMoveBlockTo(ModelBlock *block, QPoint pos)
{
    block->setPos(pos);
    emit blockUpdated(block);
}

void GPNodeProject::cmdAddBlock(ModelBlock *block)
{
    _node->addBlock(block);
    emit blockAdded(block);
}

void GPNodeProject::cmdRemoveBlock(ModelBlock *block)
{
    _node->removeBlock(block);
    emit blockRemoved(block);
}

QUndoStack *GPNodeProject::undoStack() const
{
    return _undoStack;
}

void GPNodeProject::setNode(ModelNode *node)
{
    _node = node;
    emit nodeChanged(_node);
}

void GPNodeProject::moveBlock(ModelBlock *block, const QPoint &newPos)
{
    _undoStack->push(new BlockCmdMove(this, block, block->pos(), newPos));
}

void GPNodeProject::renameBlock(ModelBlock *block, const QString &newName)
{
    _undoStack->push(new BlockCmdRename(this, block, block->name(), newName));
}

void GPNodeProject::addBlock(ModelBlock *block)
{
    if(block->name().isEmpty())
        block->setName(QString("%1_%2").arg(block->driver()).arg(_node->blocks().count()));
    if(block->name()==block->driver())
        block->setName(QString("%1_%2").arg(block->driver()).arg(_node->blocks().count()));
    _undoStack->push(new BlockCmdAdd(this, block));
}

void GPNodeProject::removeBlock(ModelBlock *block)
{
    _undoStack->push(new BlockCmdRemove(this, block));
}
