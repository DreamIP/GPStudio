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

#include "registerbitfield.h"

#include "model/model_parambitfield.h"
#include "model/model_param.h"
#include "model/model_block.h"

#include "register.h"

#include "camera.h"

#include <QRegExp>
#include <QDebug>

RegisterBitField::RegisterBitField()
{
    _parent = NULL;
}

RegisterBitField::~RegisterBitField()
{

}

uint RegisterBitField::mask() const
{
    return _mask;
}

void RegisterBitField::setMask(const uint &mask)
{
    _mask = mask;
}

uint RegisterBitField::value() const
{
    return _value;
}

void RegisterBitField::setValue(const uint &value)
{
    uint projectedValue=0;

    uint mask = 1;
    for(int i=0; i<_bits.count(); i++)
    {
        if(value&mask) projectedValue|=(1<<_bits[_bits.count()-i-1]);
        else projectedValue&=(~(1<<_bits[_bits.count()-i-1]));
        mask<<=1;
    }
    _value = projectedValue;
    //qDebug()<<"setVal bf: "<<value<<projectedValue<<_bits<<_mask;
    emit bitfieldChange(_value, _mask);
}

QString RegisterBitField::blockName() const
{
    return _blockName;
}

void RegisterBitField::setBlockName(const QString &blockName)
{
    _blockName = blockName;
}

QString RegisterBitField::propertyMap() const
{
    return _propertyMap;
}

void RegisterBitField::setPropertyMap(const QString &propertyMap)
{
    _propertyMap = propertyMap;
}

QStringList RegisterBitField::dependsProperties() const
{
    return ScriptEngine::dependsProperties(_propertyMap);
}

const QList<uint> &RegisterBitField::bits() const
{
    return _bits;
}

RegisterBitField *RegisterBitField::fromParamBitField(const ModelParamBitField *paramBitField)
{
    RegisterBitField *cameraRegisterBitField = new RegisterBitField();
    cameraRegisterBitField->setPropertyMap(paramBitField->propertyMap());
    if(paramBitField->parent())
    {
        if(paramBitField->parent()->parent()) cameraRegisterBitField->setBlockName(paramBitField->parent()->parent()->name());
    }

    cameraRegisterBitField->_bits=ModelParamBitField::listBitFromBitField(paramBitField->bitfield());

    uint mask = 0;
    foreach(uint bit, cameraRegisterBitField->_bits)
    {
        mask|=(1<<bit);
    }
    cameraRegisterBitField->_mask = mask;

    return cameraRegisterBitField;
}

void RegisterBitField::eval()
{
    setValue(ScriptEngine::getEngine().evalPropertyMap(_propertyMap).toUInt());
}

Register *RegisterBitField::parent() const
{
    return _parent;
}

void RegisterBitField::setParent(Register *parent)
{
    _parent = parent;
}
