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

#include "block.h"

#include "model/model_block.h"

#include "camera.h"
#include "flow.h"
#include "register.h"

Block::Block()
{
    _assocProperty = NULL;
    _modelBlock = NULL;
    _parentCamera = NULL;
}

QString Block::name() const
{
    return _name;
}

Block *Block::fromModelBlock(ModelBlock *modelBlock)
{
    Block *block = new Block();
    block->_modelBlock = modelBlock;
    block->setName(modelBlock->name());

    Property *propertyBlock = Property::fromModelBlock(modelBlock);
    block->_assocProperty = propertyBlock;

    // block property
    foreach (ModelProperty *property, modelBlock->properties())
    {
        Property *paramprop = Property::fromModelProperty(property);
        propertyBlock->addSubProperty(paramprop);
    }

    // flow property
    foreach (ModelFlow *modelFlow, modelBlock->flows())
    {
        Flow *flow = Flow::fromModelFlow(modelFlow);
        block->_flows.append(flow);
        block->_flowsMap.insert(flow->name(), flow);
        propertyBlock->addSubProperty(flow->assocProperty());
    }

    // param property
    foreach (ModelParam *modelParam, modelBlock->params())
    {
        if(modelParam->isHard())
        {
            Property *propertyParam = Property::fromModelParam(modelParam);
            propertyBlock->addSubProperty(propertyParam);
        }
    }

    // clock
    foreach (ModelClock *modelClock, modelBlock->clocks())
    {
        Property *propertyClock = Property::fromModelClock(modelClock);
        propertyBlock->addSubProperty(propertyClock);
    }

    return block;
}

void Block::setName(const QString &name)
{
    if(_name != name)
    {
        QString oldName = _name;
        _name = name;
        if(_parentCamera)
            _parentCamera->updateKeyBlock(this, oldName);
    }
}

const QList<Flow *> &Block::flows() const
{
    return _flows;
}

Flow *Block::flow(int i) const
{
    if(i<_flows.count()) return _flows[i];
    return NULL;
}

Flow *Block::flow(QString name) const
{
    QMap<QString, Flow*>::const_iterator localConstFind = _flowsMap.constFind(name);
    if(localConstFind!=_flowsMap.constEnd()) return *localConstFind;
    return NULL;
}

Property *Block::assocProperty() const
{
    return _assocProperty;
}

ModelBlock *Block::modelBlock() const
{
    return _modelBlock;
}

Camera *Block::parentCamera() const
{
    return _parentCamera;
}

void Block::setParentCamera(Camera *parentCamera)
{
    _parentCamera = parentCamera;
}
