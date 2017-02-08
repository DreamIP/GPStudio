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

#include "scriptengine.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMainWindow>
#include <QApplication>
#include <QDir>

#include "camera/property.h"
#include "camera/camera.h"

ScriptEngine *ScriptEngine::_instance = NULL;

QScriptValue ScriptEngine::echo(QScriptContext *context, QScriptEngine *)
{
    for(int i=0; i<context->argumentCount(); i++)
        ScriptEngine::getEngine().echo(context->argument(i).toString());
    return QScriptValue();
}

QScriptValue ScriptEngine::call(QScriptContext *context, QScriptEngine *)
{
    if(context->argumentCount() == 0)
        return QScriptValue();

    QString fileName = context->argument(0).toString();
    QFile scriptFile(getEngine()._path+"/"+fileName);
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
        return QScriptValue("bad file"); // error
    }
    QTextStream stream(&scriptFile);
    QString programm = stream.readAll();
    scriptFile.close();
    return getEngine().engine()->evaluate(programm, fileName);
}

ScriptEngine::ScriptEngine(QObject *parent) : QObject(parent)
{
    _rootProperty = NULL;

    // log2 function
    _engine.evaluate("Math.__proto__.log2=function(x){ return this.log(x) / this.log(2); }");

    // echo funtion
    _engine.globalObject().setProperty("echo", _engine.newFunction(echo), QScriptValue::ReadOnly | QScriptValue::Undeletable);

    // call funtion
    _engine.globalObject().setProperty("call", _engine.newFunction(call), QScriptValue::ReadOnly | QScriptValue::Undeletable);
}

ScriptEngine::~ScriptEngine()
{
}

QScriptEngine *ScriptEngine::engine()
{
    return &_engine;
}

const Property *ScriptEngine::path(const QString &path) const
{
    if(_rootProperty) return _rootProperty->path(path);
    return NULL;
}

const Property *ScriptEngine::rootProperty() const
{
    return _rootProperty;
}

void ScriptEngine::setRootProperty(Property *rootProperty)
{
    _rootProperty = rootProperty;

    foreach (Property *property, _rootProperty->subProperties())
    {
        computePropertyMap(property, _rootProperty);

        PropertyClass *prop=new PropertyClass(&_engine, property);
        QScriptValue value = _engine.newObject(prop);
        _engine.globalObject().setProperty(property->name(), value);
    }
}

QScriptValue ScriptEngine::eval(const QString &propertyMap)
{
    QString toEval = propertyMap;
    toEval.replace(".in.",".__in.");

    const QScriptValue &result = _engine.evaluate(toEval);
    if(result.isError())
    {
        QString error = result.toString();
        error.replace("__in","in");
        //qDebug()<<"Script engine: error evaluating "<<propertyMap<<error;
    }

    return result;
}

QVariant ScriptEngine::evalPropertyMap(const QString &propertyMap)
{
    return eval(propertyMap).toVariant();
}

QStringList ScriptEngine::dependsProperties(const QString &expression)
{
    QRegExp exp("([a-zA-Z_]+[a-zA-Z0-9_]*\\.?)+");
    int pos=0;
    QStringList props;
    while((pos = exp.indexIn(expression, pos)) != -1)
    {
        pos += exp.matchedLength();
        props.append(exp.cap(0));
    }
    props.removeDuplicates();
    return props;
}

void ScriptEngine::echo(const QString &text)
{
    emit echoAppended(text);
}

void ScriptEngine::computePropertyMap(Property *property, Property *paramsProps)
{
    foreach (Property *subProperty, property->subPropertiesMap())
    {
        const QStringList &deps = subProperty->dependsProperties();
        foreach (QString propPath, deps)
        {
            const Property *prop = paramsProps->path(propPath);
            if(prop) connect(prop, SIGNAL(valueChanged(QVariant)), subProperty, SLOT(eval()));
        }

        computePropertyMap(subProperty, paramsProps);
    }
}

QString ScriptEngine::getPath() const
{
    return _path;
}

void ScriptEngine::setPath(const QString &path)
{
    _path = path;
}
