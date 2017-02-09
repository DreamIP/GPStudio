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

#include "propertyclass.h"

#include <QDebug>
#include <QScriptString>
#include <QScriptEngine>

#include "camera/property.h"

//#define __PROP_DEBUG__

PropertyClass::PropertyClass(QScriptEngine * engine, Property *linkedProperty)
    : QScriptClass(engine), _linkedProperty(linkedProperty)
{
#ifdef __PROP_DEBUG__
    qDebug()<<'\t'<<"==== create prop class";
#endif
}

PropertyClass::~PropertyClass()
{
#ifdef __PROP_DEBUG__
    qDebug()<<'\t'<<"del prop class";
#endif
    QMapIterator<QString, PropertyClass*> it(_subPropertiesClasses);
    while (it.hasNext())
    {
        it.next();
        delete it.value();
    }
}

QString PropertyClass::name() const
{
#ifdef __PROP_DEBUG__
    qDebug()<<'\t'<<"name call $$$$";
#endif
    return "property";
}

QScriptValue::PropertyFlags PropertyClass::propertyFlags(const QScriptValue &object, const QScriptString &name, uint id)
{
    Q_UNUSED(object);
    Q_UNUSED(id);
#ifdef __PROP_DEBUG__
    qDebug()<<_linkedProperty->name()+".propertyFlags"<<name;
#else
    Q_UNUSED(name);
#endif
    return QScriptValue::PropertyGetter | QScriptValue::PropertySetter;
}

QScriptClass::QueryFlags PropertyClass::queryProperty(const QScriptValue & object, const QScriptString & name, QScriptClass::QueryFlags flags, uint * id )
{
    Q_UNUSED(object);
    Q_UNUSED(id);
#ifdef __PROP_DEBUG__
    qDebug()<<_linkedProperty->name()+".querry"<<name<<flags;
#else
    Q_UNUSED(name);
#endif
    return flags;
}

QScriptValue PropertyClass::property(const QScriptValue &object, const QScriptString &name, uint id)
{
    Q_UNUSED(id);
#ifdef __PROP_DEBUG__
    qDebug()<<_linkedProperty->name()+".get"<<name.toString()<<object.toVariant();
#else
    Q_UNUSED(object);
#endif

    if(name.toString()=="toString")
        return QScriptValue(_linkedProperty->value().toString());

    if(name.toString()=="value" || name.toString()=="valueOf")
    {
        switch (_linkedProperty->type())
        {
        case Property::Group:
            return QScriptValue();
            break;
        case Property::FlowType:
            return QScriptValue();
            break;
        case Property::BlockType:
            return QScriptValue();
            break;
        case Property::String:
            return QScriptValue(_linkedProperty->value().toString());
        case Property::FlowDataType:
            return QScriptValue(_linkedProperty->value().toString());
        case Property::Int:
        case Property::SInt:
            return QScriptValue(_linkedProperty->value().toInt());
        case Property::Enum:
            return QScriptValue(_linkedProperty->value().toString());
        case Property::Bool:
            return QScriptValue(_linkedProperty->value().toBool());
        case Property::Matrix:
            if(_linkedProperty->value().type()==QVariant::List)
            {
                QList<QList<QVariant> > lines;
                for(int x=0;x<3;x++)
                {
                    QList<QVariant> line;
                    for(int y=0;y<3;y++)
                    {
                        QString key = QString("m%1%2").arg(x).arg(y);
                        if(_linkedProperty->subPropertiesMap().contains(key))
                            line.append(_linkedProperty->subPropertiesMap()[key]->value());
                    }
                    lines.append(line);
                }
                return qScriptValueFromSequence(engine(),lines);
            }
            return QScriptValue(_linkedProperty->value().toInt());
        default:
            return QScriptValue(_linkedProperty->value().toInt());
        }
    }

    if(name.toString()=="bits")
        return QScriptValue(_linkedProperty->bits());

    if(name.toString()=="map")
        return QScriptValue(_linkedProperty->propertymap());

    QString filteredName = name.toString();
    if(filteredName=="__in")
        filteredName="in";

    if(_subPropertiesClasses.contains(filteredName))
    {
        PropertyClass *prop = _subPropertiesClasses[filteredName];
        return engine()->newObject(prop);
    }
    else
    {
        if(_linkedProperty->subPropertiesMap().contains(filteredName))
        {
            PropertyClass *prop = new PropertyClass(engine(), &(*_linkedProperty)[filteredName]);
            _subPropertiesClasses.insert(filteredName, prop);
            return engine()->newObject(prop);
        }
    }

    return QScriptValue();
}

void PropertyClass::setProperty(QScriptValue &object, const QScriptString &name, uint id, const QScriptValue &value)
{
    Q_UNUSED(id);
    Q_UNUSED(object);
#ifdef __PROP_DEBUG__
    qDebug()<<_linkedProperty->name()+".set"<<name<<value.toVariant();
#endif
    if(name.toString()=="value")
        _linkedProperty->setValue(value.toVariant());
}
