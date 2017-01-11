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

#include "register.h"

#include <QDebug>
#include <QRegExp>
#include <QStringList>

#include "model/model_param.h"
#include "model/model_block.h"

#include "camera.h"

Register::Register(const QString &name, const uint &addr)
    : _name(name), _addr(addr)
{
    _value=0;
}

Register::~Register()
{
}

QString Register::name() const
{
    return _name;
}

void Register::setName(const QString &name)
{
    _name = name;
}

uint Register::addr() const
{
    return _addr;
}

void Register::setAddr(const uint &addr)
{
    _addr = addr;
}

uint Register::value() const
{
    return _value;
}

void Register::setValue(const uint &value)
{
    //if(_value!=value)
    {
        _value = value;
        emit registerChange(_addr, value);
    }
}

void Register::setValue(const QVariant &value)
{
    setValue(value.toUInt());
}

void Register::eval()
{
    setValue(ScriptEngine::getEngine().evalPropertyMap(_propertyMap));
}

void Register::setField(uint value, uint mask)
{
    //qDebug()<<"setfiel"<<value<<mask<<(((~mask)&_value)|(mask&value));
    setValue(((~mask)&_value)|(mask&value));
}

const ModelParam *Register::modelParam() const
{
    return _modelParam;
}

QString Register::blockName() const
{
    return _blockName;
}

void Register::setBlockName(const QString &blockName)
{
    _blockName = blockName;
}

QString Register::propertyMap() const
{
    return _propertyMap;
}

void Register::setPropertyMap(const QString &propertyMap)
{
    _propertyMap = propertyMap;
}

QStringList Register::dependsProperties() const
{
    return ScriptEngine::dependsProperties(_propertyMap);
}

Camera *Register::camera() const
{
    return _camera;
}

void Register::setCamera(Camera *camera)
{
    _camera = camera;
}

void Register::addBitField(RegisterBitField *bitField)
{
    bitField->setParent(this);
    connect(bitField, SIGNAL(bitfieldChange(uint,uint)), this, SLOT(setField(uint,uint)));
    _bitFields.append(bitField);
}

const QList<RegisterBitField *> &Register::bitFields() const
{
    return _bitFields;
}

Register *Register::fromParam(const ModelParam *modelParam)
{
    Register *cameraRegister = new Register(modelParam->name(), modelParam->absoluteAddr());
    cameraRegister->_modelParam = modelParam;
    cameraRegister->setPropertyMap(modelParam->propertyMap());
    if(modelParam->parent()) cameraRegister->setBlockName(modelParam->parent()->name());

    foreach(ModelParamBitField *bitField, modelParam->paramBitFields())
    {
        RegisterBitField *camBitField = RegisterBitField::fromParamBitField(bitField);
        cameraRegister->addBitField(camBitField);
    }

    return cameraRegister;
}
