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

#include "confignodedialog.h"
#include "gpnodeproject.h"

#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

#include "undostack/nodecommands.h"
#include "undostack/blockcommands.h"
#include "undostack/viewercommands.h"

#include <model/model_fiblock.h>
#include <model/model_gpviewer.h>
#include <model/model_viewerflow.h>

#include "lib_parser/lib.h"

GPNodeProject::GPNodeProject(QObject *parent)
    : QObject(parent)
{
    _nodeEditorWindow = NULL;
    _node = NULL;
    _camera = NULL;
    _modified = false;
    _undoStack = new QUndoStack();
}

GPNodeProject::~GPNodeProject()
{
    delete _undoStack;
    delete _node;
    delete _camera;
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
    if(!closeProject())
        return;

    setPath("");
    setNode(new ModelNode("unsaved_project"));
    setModified(false);
}

bool GPNodeProject::openProject(const QString &nodeFileName)
{
    ModelNode *node;
    QString fileName;

    if(!closeProject())
        return false;

    if(nodeFileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(_nodeEditorWindow, "Open node project", "", "Node project (*.node)");
        if(fileName.isEmpty())
        {
            newProject();
            return false;
        }
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
    {
        _camera = NULL;
        return false;
    }

    // load library with project IPs
    foreach (ModelBlock *block, node->blocks())
    {
        if(block->driver().endsWith(".proc") || block->driver().endsWith(".io"))
        {
            Lib::getLib().addIp(block->path() + "/" + block->driver());
        }
    }

    setPath(fileName);
    setModified(false);
    setNode(node);

    return true;
}

bool GPNodeProject::saveProject()
{
    return saveProjectAs(_path);
}

bool GPNodeProject::saveProjectAs(const QString &nodeFileName)
{
    QString fileName;

    if(nodeFileName.isEmpty())
    {
        QFileDialog fileDialog(_nodeEditorWindow);
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        fileDialog.setDefaultSuffix(".node");
        fileDialog.setNameFilter("Node project (*.node)");
        fileDialog.setWindowTitle("Save node project");
        if (fileDialog.exec())
            fileName = fileDialog.selectedFiles().first();
        if(fileName.isEmpty())
            return false;
    }
    else
        fileName = nodeFileName;

    if(!fileName.endsWith(".node"))
        fileName.append(".node");
    setPath(fileName);

    _node->saveToFile(_path);
    setModified(false);
    return true;
}

bool GPNodeProject::closeProject()
{
    if(_modified)
    {
        QMessageBox::StandardButton res;
        res = QMessageBox::question(_nodeEditorWindow, "Project modified", "Would you like to save the project before close it ?", QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);
        if(res==QMessageBox::Save)
            saveProject();
        if(res==QMessageBox::Cancel)
            return false;
    }

    _modified = false;
    _undoStack->clear();
    setNode(NULL);
    delete _node;
    return true;
}

void GPNodeProject::configBoard()
{
    ConfigNodeDialog configNodeDialog(_nodeEditorWindow);
    configNodeDialog.setProject(this);
    if(configNodeDialog.exec() == QDialog::Accepted)
    {
        QString boardName;
        if(_node->board())
            boardName = _node->board()->name();

        _undoStack->push(new NodeCmdConfigBoard(this,
                         boardName, _node->iosList(),
                         configNodeDialog.boardName(), configNodeDialog.iosName()));
    }
}

void GPNodeProject::renameNode(const QString &oldName, const QString &newName)
{
    _undoStack->push(new NodeCmdRename(this, oldName, newName));
}

void GPNodeProject::setPath(const QString &path)
{
    _path = path;
    /*if(_node)
        _node->setName(name());*/
    emit nodePathChanged(_path);
}

void GPNodeProject::setModified(bool modified)
{
    _modified = modified;
    emit nodeModified(_modified);
}

QWidget *GPNodeProject::nodeEditorWindow() const
{
    return _nodeEditorWindow;
}

void GPNodeProject::setNodeEditorWindow(QWidget *nodeEditorWindow)
{
    _nodeEditorWindow = nodeEditorWindow;
}

void GPNodeProject::cmdRenameBlock(const QString &block_name, const QString &newName)
{
    ModelBlock *modelBlock = _node->getBlock(block_name);
    if(!modelBlock)
        return;

    // rename connection to this block
    ModelFIBlock *fiBlock = _node->getFIBlock();
    if(!fiBlock)
    {
        fiBlock = new ModelFIBlock();
        _node->addBlock(fiBlock);
    }
    foreach (ModelFlowConnect *flowConnect, fiBlock->flowConnects())
    {
        if(flowConnect->fromblock()==block_name)
            flowConnect->setFromblock(newName);
        if(flowConnect->toblock()==block_name)
            flowConnect->setToblock(newName);
    }

    modelBlock->setName(newName);
    Block *block = _camera->block(block_name);
    if(block)
        block->setName(newName);
    emit blockUpdated(modelBlock);
    setModified(true);
}

void GPNodeProject::cmdMoveBlockTo(const QString &block_name, const QString &part_name, QPoint pos)
{
    ModelBlock *modelBlock = _node->getBlock(block_name);
    if(modelBlock)
    {
        ModelComponentPart *part = modelBlock->getPart(part_name);
        if(part)
        {
            part->setPos(pos);
            emit blockUpdated(modelBlock);
            setModified(true);
        }
    }
}

void GPNodeProject::cmdAddBlock(ModelBlock *modelBlock)
{
    ModelFIBlock *fiBlock = _node->getFIBlock();
    if(!fiBlock)
    {
        fiBlock = new ModelFIBlock();
        _node->addBlock(fiBlock);
        _camera->addBlock(fiBlock);
    }

    _node->addBlock(modelBlock);
    _camera->addBlock(modelBlock);

    emit blockAdded(modelBlock);
    setModified(true);
}

void GPNodeProject::cmdRemoveBlock(const QString &block_name)
{
    ModelFIBlock *fiBlock = _node->getFIBlock();
    if(!fiBlock)
    {
        fiBlock = new ModelFIBlock();
        _node->addBlock(fiBlock);
    }

    foreach (ModelFlowConnect *flowConnect, fiBlock->flowConnects(block_name))
    {
        cmdDisconnectFlow(*flowConnect);
    }

    ModelBlock *block = _node->getBlock(block_name);
    _node->removeBlock(block);
    _camera->removeBlock(block);
    emit blockRemoved(block_name);
    setModified(true);
    if(block)
        delete block;
}

void GPNodeProject::cmdConnectFlow(const ModelFlowConnect &flowConnect)
{
    ModelFIBlock *fiBlock = _node->getFIBlock();
    if(!fiBlock)
    {
        fiBlock = new ModelFIBlock();
        _node->addBlock(fiBlock);
    }

    fiBlock->connectFlow(flowConnect);
    emit blockConnected(flowConnect);
    setModified(true);
}

void GPNodeProject::cmdDisconnectFlow(const ModelFlowConnect &flowConnect)
{
    ModelFIBlock *fiBlock = _node->getFIBlock();
    if(!fiBlock)
    {
        fiBlock = new ModelFIBlock();
        _node->addBlock(fiBlock);
    }

    fiBlock->disConnectFlow(flowConnect);
    emit blockDisconected(flowConnect);
    setModified(true);
}

void GPNodeProject::cmdRenameNode(QString nodeName)
{
    if(_node->name() != nodeName)
    {
        _node->setName(nodeName);
        setModified(true);
    }
}

void GPNodeProject::cmdConfigBoard(QString boardName, QStringList iosName)
{
    int count = 0;
    foreach (QString ioName, _node->iosList())
    {
        count++;
        if(!iosName.contains(ioName))
            cmdRemoveBlock(ioName);
    }

    BoardLib *boardLib = Lib::getLib().board(boardName);
    if(!boardLib)
        return;

    ModelBoard *board = new ModelBoard(*boardLib->modelBoard());
    _node->setBoard(board);

    foreach (QString ioName, iosName)
    {
        if(_node->getBlock(ioName) == NULL)
        {
            QString ioDriver = boardLib->io(ioName)->driver();
            BlockLib *ioLib = Lib::getLib().io(ioDriver);
            if(ioLib)
            {
                count++;
                ModelIO *io;
                if(ioLib->type()==BlockLib::IOCom)
                    io = new ModelIOCom(*ioLib->modelIOCom());
                else
                    io = new ModelIO(*ioLib->modelIO());
                io->setName(ioName);
                int count2=0;
                foreach (ModelComponentPart *part, io->parts())
                    part->setPos(QPoint(count*200, (count2++)*200));
                cmdAddBlock(io);
            }
        }
    }

    count = 0;
    if(_node->gpViewer()->viewers().isEmpty() && _node->getIOCom() != NULL)
    {
        foreach (ModelFlow *flow, _node->getIOCom()->flowsIn())
        {
            ModelViewer *viewer = new ModelViewer(QString("viewer %1").arg(count));
            viewer->addViewerFlow(new ModelViewerFlow(flow->name()));
            cmdAddViewer(viewer);
            count++;
        }
    }
}

void GPNodeProject::cmdSetParam(const QString &blockName, const QString &paramName, const QVariant &value)
{
    bool ok = false;
    // param
    if(!ok)
    {
        ModelParam *param = _node->getParam(blockName, paramName);
        if(param)
        {
            param->setValue(value);
            ok = true;
        }
    }
    // property
    if(!ok)
    {
        ModelProperty *property = _node->getPropertyPath(blockName, paramName);
        if(property)
        {
            property->setValue(value);
            ok = true;
        }
    }
    // clock
    if(!ok)
    {
        ModelClock *clock = _node->getClock(blockName, paramName);
        if(clock)
        {
            clock->setTypical(value.toInt());
            ok = true;
        }
    }
    // flow
    if(!ok)
    {
        ModelFlow *flow = _node->getFlow(blockName, paramName);
        if(flow)
        {
            flow->setSize(value.toInt());
            ok = true;
        }
    }

    if(ok)
    {
        Property *assocProperty = _camera->rootProperty().path(blockName + "." + paramName);
        if(assocProperty)
            assocProperty->setValue(value);

        //emit blockUpdated(block);
        setModified(true);
    }
}

void GPNodeProject::cmdRenameViewer(const QString &viewerName, QString newName)
{
    ModelViewer *viewer = _node->gpViewer()->getViewer(viewerName);
    if(viewer)
    {
        viewer->setName(newName);
        setModified(true);
        emit viewerUpdated(viewer);
    }
}

void GPNodeProject::cmdAddViewer(ModelViewer *viewer)
{
    _node->gpViewer()->addViewer(viewer);
    setModified(true);
    emit viewerAdded(viewer);
}

void GPNodeProject::cmdRemoveViewer(const QString &viewerName)
{
    ModelViewer *viewer = _node->gpViewer()->getViewer(viewerName);
    if(viewer)
    {
        _node->gpViewer()->removeViewer(viewer);
        emit viewerRemoved(viewer->name());
        setModified(true);
        delete viewer;
    }
}

void GPNodeProject::cmdAddViewerFlow(const QString &viewerName, ModelViewerFlow *viewerFlow)
{
    ModelViewer *viewer;

    if(viewerName.isEmpty())
        viewer = new ModelViewer(newViewerName());
    else
        viewer = _node->gpViewer()->getViewer(viewerName);
    if(viewer)
    {
        if(viewer->getViewerFlow(viewerFlow->flowName()) == NULL)
        {
            viewer->addViewerFlow(viewerFlow);
            emit viewerFlowAdded(viewerFlow);
            setModified(true);
        }
    }
}

void GPNodeProject::cmdRemoveViewerFlow(const QString &viewerName, const QString &viewerFlowName)
{
    ModelViewerFlow *viewerFlow = _node->gpViewer()->getViewerFlow(viewerName, viewerFlowName);
    if(viewerFlow)
    {
        viewerFlow->viewer()->removeViewerFlow(viewerFlow);
        emit viewerFlowRemoved(viewerName, viewerFlow->flowName());
        setModified(true);
        delete viewerFlow;
    }
}

QUndoStack *GPNodeProject::undoStack() const
{
    return _undoStack;
}

void GPNodeProject::setNode(ModelNode *node)
{
    _node = node;
    _camera = new Camera();
    _camera->setNode(node);
    emit nodeChanged(_node);
}

Camera *GPNodeProject::camera() const
{
    return _camera;
}

QString GPNodeProject::newBlockName(const QString &driver) const
{
    int i = 1;
    QString name = QString("%1_%2").arg(driver).arg(i);
    while(_node->getBlock(name)!=NULL)
        name = QString("%1_%2").arg(driver).arg(++i);
    return name;
}

QString GPNodeProject::newViewerName() const
{
    int i = 1;
    QString name = QString("viewer_%1").arg(i);
    while(_node->gpViewer()->getViewer(name)!=NULL)
        name = QString("viewer_%1").arg(++i);
    return name;
}

QPoint GPNodeProject::newBlockPos() const
{
    QPoint pos(0,0);
    bool find;
    do
    {
        QRect rect = QRect(pos-QPoint(10,10), QSize(20,20));
        find = false;
        foreach (ModelBlock *block, _node->blocks())
        {
            foreach (ModelComponentPart *part, block->parts())
            {
                if(rect.contains(part->pos()))
                {
                    find = true;
                    pos += QPoint(20,20);
                    break;
                }
            }
            if(find)
                break;
        }
    } while(find);
    return pos;
}

void GPNodeProject::moveBlock(const QString &block_name, const QString &part_name, const QPoint &oldPos, const QPoint &newPos)
{
    _undoStack->push(new BlockCmdMove(this, block_name, part_name, oldPos, newPos));
}

void GPNodeProject::renameBlock(const QString &block_name, const QString &newName)
{
    QString name = newName;
    QRegExp nameChecker("^[a-zA-Z][a-zA-Z0-9_]*$");

    if(name.isEmpty())
        name = QInputDialog::getText(NULL, "Enter a new name for this block", "New name", QLineEdit::Normal, block_name);

    if(block_name == name || name.isEmpty())
        return;
    ModelBlock *block = _node->getBlock(name);
    while(block != NULL || nameChecker.indexIn(name)==-1)
    {
        if(block != NULL)
        {
            name = QInputDialog::getText(NULL, "Enter a new name for this block", "This name already exists, try another name", QLineEdit::Normal, name + "_1");
            if(name.isEmpty())
                return;
        }
        else
        {
            name = QInputDialog::getText(NULL, "Enter a new name for this block", "Invalid name, try another name", QLineEdit::Normal, name.replace(QRegExp("\\W"),""));
            if(name.isEmpty())
                return;
        }
        block = _node->getBlock(name);
    }

    if(!name.isEmpty())
        _undoStack->push(new BlockCmdRename(this, block_name, name));
}

void GPNodeProject::addBlock(ModelBlock *block)
{
    if(block->name().isEmpty() || block->name()==block->driver())
    {
        block->setName(newBlockName(block->driver()));
    }
    _undoStack->push(new BlockCmdAdd(this, block));
}

void GPNodeProject::addBlock(const QString &driver, const QPoint &pos)
{
    BlockLib *processLib = Lib::getLib().process(driver);
    if(!processLib)
        return;

    QPoint posBlock = pos;
    if(pos.isNull())
        posBlock = newBlockPos();

    ModelProcess *modelProcess = new ModelProcess(*processLib->modelProcess());
    foreach (ModelComponentPart *part, modelProcess->parts())
    {
        part->setPos(posBlock);
        posBlock += QPoint(20,20);
    }
    addBlock(modelProcess);
}

void GPNodeProject::removeBlock(ModelBlock *block)
{
    _undoStack->push(new BlockCmdRemove(this, block));
}

void GPNodeProject::connectBlockFlows(const ModelFlowConnect &flowConnect)
{
    _undoStack->push(new BlockCmdConnectFlow(this, flowConnect));
}

void GPNodeProject::disConnectBlockFlows(const ModelFlowConnect &flowConnect)
{
    _undoStack->push(new BlockCmdDisconnectFlow(this, flowConnect));
}

void GPNodeProject::blockSetParam(const QString &blockName, const QString &paramName, const QVariant &value)
{
    QVariant oldValue;

    // param
    if(!oldValue.isValid())
    {
        ModelParam *param = _node->getParam(blockName, paramName);
        if(param)
            oldValue = param->value();
    }
    // property
    if(!oldValue.isValid())
    {
        ModelProperty *property = _node->getPropertyPath(blockName, paramName);
        if(property)
            oldValue = property->value();
    }
    // clock
    if(!oldValue.isValid())
    {
        ModelClock *clock = _node->getClock(blockName, paramName);
        if(clock)
            oldValue = clock->typical();
    }
    // flow
    if(!oldValue.isValid())
    {
        ModelFlow *flow = _node->getFlow(blockName, paramName);
        if(flow)
            oldValue = flow->size();
    }

    if(oldValue.isValid())
        _undoStack->push(new BlockCmdParamSet(this, blockName, paramName, oldValue, value));
}

void GPNodeProject::renameViewer(const QString &viewerName, const QString &newName)
{
    _undoStack->push(new ViewerCmdRename(this, viewerName, newName));
}

void GPNodeProject::addViewer(ModelViewer *viewer)
{
    if(viewer->name().isEmpty())
        viewer->setName(newViewerName());
    if(_node->gpViewer()->getViewer(viewer->name()) == NULL)
        _undoStack->push(new ViewerCmdAdd(this, viewer));
}

void GPNodeProject::removeViewer(ModelViewer *viewer)
{
    if(_node->gpViewer()->viewers().contains(viewer))
        _undoStack->push(new ViewerCmdRemove(this, viewer));
}

void GPNodeProject::addViewerFlow(const QString &viewerName, ModelViewerFlow *viewerFlow)
{
    if(_node->gpViewer()->getViewerFlow(viewerName, viewerFlow->flowName()) == NULL)
        _undoStack->push(new ViewerFlowCmdAdd(this, viewerName, viewerFlow));
}

void GPNodeProject::removeViewerFlow(ModelViewerFlow *viewerFlow)
{
    if(!viewerFlow->viewer())
        return;
    _undoStack->push(new ViewerFlowCmdRemove(this, viewerFlow));
}

void GPNodeProject::beginMacro(const QString &text)
{
    _undoStack->beginMacro(text);
}

void GPNodeProject::endMacro()
{
    _undoStack->endMacro();
}
