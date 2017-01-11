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

#include "camera.h"

#include <QDebug>
#include <QStringList>

#include "property.h"
#include "scriptengine/propertyclass.h"

#include "flowmanager.h"
#include "cameracom.h"

#include "lib_parser/lib.h"

#include <model/model_iocom.h>

Camera::Camera(const QString &fileCameraConfig)
    : _registermanager(this)
{
    _modelNode = NULL;
    _com = NULL;

    _comBlock = NULL;
    _fiBlock = NULL;

    if(!fileCameraConfig.isEmpty())
        loadFromFile(fileCameraConfig);
}

Camera::~Camera()
{
    delete _modelNode;
    delete _com;
}

bool Camera::loadFromFile(const QString &fileCameraConfig)
{
    ModelNode *node = ModelNode::readFromFile(fileCameraConfig);
    setNode(node);

    if(!node)
        return false;

    // load library with project IPs
    foreach (ModelBlock *block, node->blocks())
    {
        if(block->driver().endsWith(".proc") || block->driver().endsWith(".io"))
        {
            ModelFile *file = block->getDefFile();
            if(file)
                Lib::getLib().addIp(file->path());
        }
    }

    return true;
}

const ModelNode *Camera::node() const
{
    return _modelNode;
}

void Camera::setNode(ModelNode *node)
{
    if(_modelNode)
        delete _modelNode;
    _modelNode = node;

    if(!_modelNode)
        return;

    _paramsBlocks.removeAllSubProperties();
    _paramsBlocks.setName(node->name());

    foreach (ModelBlock *modelBlock, _modelNode->blocks())
        addBlock(modelBlock);
    if(node->isGeneratedFile())
        _paramsBlocks.setMode(Property::Run);

    ModelIOCom *iOCom = node->getIOCom();
    if(iOCom)
        _comBlock = block(iOCom->name());

    _fiBlock = block("fi");

    _registermanager.setNode(node);
    ScriptEngine::getEngine().setRootProperty(&_paramsBlocks);
    _registermanager.start();

    _flowManager = new FlowManager(this);
}

RegisterManager &Camera::registermanager()
{
    return _registermanager;
}

Block *Camera::fiBlock() const
{
    return _fiBlock;
}

Block *Camera::comBlock() const
{
    if(!_modelNode)
        return NULL;
    ModelIOCom *iOCom = _modelNode->getIOCom();
    if(iOCom)
        return block(iOCom->name());
    return NULL;
}

const Property &Camera::rootProperty() const
{
    return _paramsBlocks;
}

Property &Camera::rootProperty()
{
    return _paramsBlocks;
}

RegisterManager &Camera::registers()
{
    return _registermanager;
}

uint Camera::evalPropertyMap(const QString &propertyMap)
{
    return ScriptEngine::getEngine().evalPropertyMap(propertyMap).toUInt();
}

void Camera::setRegister(uint addr, uint value)
{
    _registermanager.setRegister(addr, value);
    emit registerDataChanged();
}

void Camera::updateKeyBlock(Block *block, const QString &oldKey)
{
    _blocksMap.remove(oldKey);
    _blocksMap.insert(block->name(), block);
}

const QList<Block*> &Camera::blocks() const
{
    return _blocks;
}

Block *Camera::block(int i) const
{
    if(i<_blocks.count())
        return _blocks[i];
    return NULL;
}

Block *Camera::block(QString name) const
{
    QMap<QString, Block*>::const_iterator localConstFind = _blocksMap.constFind(name);
    if(localConstFind!=_blocksMap.constEnd())
        return *localConstFind;
    return NULL;
}

QByteArray Camera::registerData() const
{
    return _registermanager.registerData();
}

void Camera::addBlock(Block *block)
{
    _blocks.append(block);
    _blocksMap.insert(block->name(), block);
    block->setParentCamera(this);

    _paramsBlocks.addSubProperty(block->assocProperty());
}

void Camera::addBlock(ModelBlock *modelBlock)
{
    addBlock(Block::fromModelBlock(modelBlock));
}

void Camera::removeBlock(Block *block)
{
    _blocks.removeOne(block);
    _blocksMap.remove(block->name());

    _paramsBlocks.removeSubProperty(block->assocProperty());
}

void Camera::removeBlock(ModelBlock *modelBlock)
{
    Block *blockptr = block(modelBlock->name());
    if(blockptr)
        removeBlock(blockptr);
}

CameraCom *Camera::com() const
{
    return _com;
}

FlowManager *Camera::flowManager() const
{
    return _flowManager;
}

bool Camera::isConnected() const
{
    if(_com)
        return _com->isConnected();
    else
        return false;
}

void Camera::connectCam(const CameraInfo &cameraInfo)
{
    _com = new CameraCom(cameraInfo);

    if(_com->isConnected())
    {
        //_registermanager.evalAll();
    }

    connect(_com, SIGNAL(flowReadyToRead(int)), _flowManager, SLOT(processFlow(int)));
}
