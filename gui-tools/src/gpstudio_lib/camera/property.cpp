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

#include "property.h"

#include <QDebug>

#include "model/model_property.h"
#include "model/model_flow.h"

#include "camera.h"

Property::Property(QString name)
    : _name(name)
{
    _parent = NULL;
    _type = Group;
    _row = 0;
    _bits = 0;
    _modelProperty = NULL;
    _fixed = false;
    _const = false;
}

Property::~Property()
{
    foreach (Property *property, _subProperties)
        delete property;
    foreach (PropertyEnum *propertyEnum, _enumsMap)
        delete propertyEnum;
}

const QString &Property::name() const
{
    return _name;
}

void Property::setName(const QString &name)
{
    _name = name;
}

const QString &Property::caption() const
{
    return _caption;
}

void Property::setCaption(const QString &caption)
{
    _caption = caption;
}

const QVariant &Property::value() const
{
    return _value;
}

void Property::setValue(bool value)
{
    setValue(QVariant(value));
}

void Property::setValue(int value)
{
    setValue(QVariant(value));
}

void Property::setValue(const QVariant &value)
{
    bool valueChangedb = false;
    if(value != _value && _value.isValid())
        valueChangedb=true;
    _value=value;

    if(_type==Matrix)
    {
        if(value.type()==QVariant::List)
        {
            int x=0;
            foreach (QVariant line, value.toList())
            {
                if(line.type()==QVariant::List)
                {
                    int y=0;
                    foreach (QVariant item, line.toList())
                    {
                        QString key = QString("m%1%2").arg(x).arg(y);
                        if(_subPropertiesMap.contains(key)) _subPropertiesMap[key]->setValue(item.toInt());
                        y++;
                    }
                }
                x++;
            }
        }
    }
    else
    {
        if(!_enumsMap.isEmpty())
        {
            if(_enumsMap.contains(value.toString()))
            {
                setBits(_enumsMap[value.toString()]->value().toUInt());
            }
        }
        else
        {
            setBits(value.toUInt());
        }
    }

    if(valueChangedb)
        emit valueChanged(QVariant(value));

    ScriptEngine::getEngine().evalPropertyMap(_onchange);
}

uint Property::bits() const
{
    return _bits;
}

void Property::setBits(const uint bits)
{
    if(bits != _bits)
    {
        _bits = bits;
        emit bitsChanged(bits);
    }
    else _bits = bits;
}

void Property::eval()
{
    if(_propertyMap.isEmpty())
        return;

    QScriptValue value = ScriptEngine::getEngine().eval(_propertyMap);
    if(!value.isError())
        setValue(value.toVariant());
}

const QVariant &Property::min() const
{
    return _min;
}

void Property::setMin(const QVariant &min)
{
    _min = min;
}

const QVariant &Property::max() const
{
    return _max;
}

void Property::setMax(const QVariant &max)
{
    _max = max;
}

const QVariant &Property::step() const
{
    return _step;
}

void Property::setStep(const QVariant &step)
{
    _step = step;
}

const QMap<QString, PropertyEnum *> &Property::enumsMap() const
{
    return _enumsMap;
}

const QList<PropertyEnum *> Property::enums() const
{
    return _enumsMap.values();
}

Property::Type Property::type() const
{
    return _type;
}

QVariant::Type Property::variantType() const
{
    switch (_type)
    {
    case Property::String:
    case Property::StringType:
        return QVariant::String;
    case Property::Int:
    case Property::SInt:
        return QVariant::Int;
    case Property::Bool:
        return QVariant::Bool;
    case Property::Enum:
        return QVariant::StringList;
    case Property::Matrix:
        return QVariant::StringList;
    default:
        return QVariant::Invalid;
    }
}

void Property::setType(const Property::Type &type)
{
    _type = type;
}

const QString &Property::propertymap() const
{
    return _propertyMap;
}

void Property::setPropertymap(const QString &propertymap)
{
    _propertyMap = propertymap;
}

const QString &Property::onchange() const
{
    return _onchange;
}

void Property::setOnchange(const QString &onchange)
{
    _onchange = onchange;
}

Property &Property::operator[](const QString &name)
{
    return *path(name);
}

const QList<Property *> &Property::subProperties() const
{
    return _subProperties;
}

Property *Property::parent() const
{
    return _parent;
}

const ModelProperty *Property::modelProperty() const
{
    return _modelProperty;
}

QString Property::blockName() const
{
    if(_type==BlockType)
        return _name;

    if(!_parent)
        return QString();

    return _parent->blockName();
}

QString Property::getPath() const
{
    if(_type==BlockType)
        return QString();

    if(!_parent)
        return QString();

    if(_parent->type()!=BlockType)
        return _parent->getPath()+"."+_name;
    return _name;
}

void Property::setParent(Property *parent)
{
    _parent = parent;
}

bool Property::isFixed() const
{
    return _fixed;
}

bool Property::isConst() const
{
    return _const;
}

Property::Mode Property::mode() const
{
    return _mode;
}

void Property::setMode(Property::Mode mode)
{
    _mode = mode;
    foreach (Property *property, _subProperties)
        property->setMode(mode);
}

int Property::row() const
{
    return _row;
}

void Property::setRow(int row)
{
    _row = row;
}

Property *Property::path(const QString &path) const
{
    if(path.isEmpty() || path=="value" || path=="bits")
        return (Property *)this;
    int index = path.indexOf(".");
    if(index==-1)
    {
        if(_subPropertiesMap.contains(path))
            return _subPropertiesMap[path];
        else return NULL;
    }
    if(_subPropertiesMap.contains(path.left(index)))
        return _subPropertiesMap[path.left(index)]->path(path.mid(index+1));
    return NULL;
}

QVariant Property::property(const QString &path) const
{
    Property *prop = this->path(path);
    if(prop)
        return prop->value();
    return QVariant();
}

QStringList Property::dependsProperties() const
{
    return ScriptEngine::dependsProperties(_propertyMap);
}

const QMap<QString, Property *> &Property::subPropertiesMap() const
{
    return _subPropertiesMap;
}

void Property::addSubProperty(Property *property)
{
    property->setParent(this);
    property->setRow(_subProperties.count());
    _subPropertiesMap.insert(property->name(), property);
    _subProperties.append(property);
    emit subPropertyChange();
}

void Property::removeSubProperty(Property *property)
{
    _subPropertiesMap.remove(property->name());
    _subProperties.removeOne(property);
    delete property;
    emit subPropertyChange();
}

void Property::removeAllSubProperties()
{
    foreach (Property *property, _subProperties)
        delete property;
    _subPropertiesMap.clear();
    _subProperties.clear();
}

Property *Property::fromModelProperty(const ModelProperty *modelProperty)
{
    Property *propertyParam = new Property(modelProperty->name());
    propertyParam->setCaption(modelProperty->caption());
    propertyParam->setOnchange(modelProperty->onchange());
    propertyParam->setPropertymap(modelProperty->propertymap());
    propertyParam->_modelProperty = modelProperty;
    if(!modelProperty->propertyEnums().empty())
    {
        foreach (ModelPropertyEnum *blockPropertyEnum, modelProperty->propertyEnums())
        {
            PropertyEnum *propertyEnum = new PropertyEnum(blockPropertyEnum->name(), blockPropertyEnum->caption(), blockPropertyEnum->value());
            propertyParam->_enumsMap.insert(blockPropertyEnum->name(), propertyEnum);
        }
        propertyParam->setType(Property::Enum);
        propertyParam->setValue(modelProperty->value());
    }
    else if(modelProperty->type()=="int" || modelProperty->type()=="sint")
    {
        if(modelProperty->type()=="int") propertyParam->setType(Int);
        if(modelProperty->type()=="sint") propertyParam->setType(SInt);
        propertyParam->setValue(modelProperty->value().toInt());
        propertyParam->setMin(modelProperty->min());
        propertyParam->setMax(modelProperty->max());
        propertyParam->setStep(modelProperty->step().toInt());
    }
    else if(modelProperty->type()=="matrix")
    {
        propertyParam->setType(Matrix);
        propertyParam->setValue(QVariant(modelProperty->value()).toInt());
    }
    else if(modelProperty->type()=="bool")
    {
        propertyParam->setType(Bool);
        propertyParam->setValue(QVariant(modelProperty->value()).toBool());
    }
    else if(modelProperty->type()=="group")
        propertyParam->setType(Group);
    else if(modelProperty->type()=="flowtype")
    {
        propertyParam->setType(FlowDataType);
        propertyParam->setValue(modelProperty->value());
    }
    else
    {
        propertyParam->setType(String);
        propertyParam->setValue(modelProperty->value());
    }

    // sub properties
    foreach (ModelProperty *subBlockProperty, modelProperty->properties())
    {
        propertyParam->addSubProperty(Property::fromModelProperty(subBlockProperty));
    }

    return propertyParam;
}

Property *Property::fromModelFlow(const ModelFlow *modelFlow)
{
    Property *propertyFlow = new Property(modelFlow->name());
    propertyFlow->setCaption(modelFlow->name());
    propertyFlow->setType(FlowType);
    propertyFlow->setValue((int)modelFlow->size());
    propertyFlow->_fixed = true;
    propertyFlow->_const = true;

    // sub properties
    foreach (ModelProperty *subBlockProperty, modelFlow->properties())
    {
        propertyFlow->addSubProperty(Property::fromModelProperty(subBlockProperty));
    }

    return propertyFlow;
}

Property *Property::fromModelBlock(const ModelBlock *modelBlock)
{
    Property *propertyBlock = new Property(modelBlock->name());
    propertyBlock->setCaption(modelBlock->name() + " (" + modelBlock->driver() + ")");
    propertyBlock->setType(BlockType);

    return propertyBlock;
}

Property *Property::fromModelParam(const ModelParam *modelParam)
{
    Property *propertyParam = new Property(modelParam->name());
    propertyParam->setCaption(modelParam->name());
    propertyParam->setType(StringType);
    propertyParam->setValue(modelParam->value().toString());
    propertyParam->_fixed = true;
    propertyParam->_const = true;

    return propertyParam;
}

Property *Property::fromModelClock(const ModelClock *modelClock)
{
    Property *propertyParam = new Property(modelClock->name());
    propertyParam->setCaption(modelClock->name());
    propertyParam->setType(Clock);
    propertyParam->setValue(modelClock->typical());
    propertyParam->_fixed = true;
    propertyParam->_const = true;

    return propertyParam;
}
