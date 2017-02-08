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

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "gpstudio_lib_common.h"

#include <QObject>
#include <QScriptEngine>

#include "propertyclass.h"
class Camera;

class GPSTUDIO_LIB_EXPORT ScriptEngine : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ScriptEngine)

private:
    explicit ScriptEngine(QObject *parent = 0);

public:
    ~ScriptEngine();

    QScriptEngine *engine();

    const Property *path(const QString &path) const;
    const Property *rootProperty() const;
    void setRootProperty(Property *rootProperty);

    QScriptValue eval(const QString &propertyMap);
    QVariant evalPropertyMap(const QString &propertyMap);

    static QStringList dependsProperties(const QString &expression);
    static inline ScriptEngine &getEngine() {if(!_instance) _instance = new ScriptEngine(); return *_instance;}

    void echo(const QString &text);
    static QScriptValue echo(QScriptContext *context, QScriptEngine *);
    static QScriptValue call(QScriptContext *context, QScriptEngine *);

    QString getPath() const;
    void setPath(const QString &path);

signals:
    void echoAppended(QString text);

private:
    static void computePropertyMap(Property *property, Property *paramsProps);

private:
    QScriptEngine _engine;
    static ScriptEngine *_instance;
    QString _path;

    Property *_rootProperty;
};

#endif // SCRIPTENGINE_H
