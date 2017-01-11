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

#ifndef PROPERTYCLASS_H
#define PROPERTYCLASS_H

#include "gpstudio_lib_common.h"

#include <QScriptClass>
#include <QMap>

class Property;

class GPSTUDIO_LIB_EXPORT PropertyClass: public QObject, public QScriptClass
{
public:
    PropertyClass(QScriptEngine *engine, Property *linkedProperty);
    ~PropertyClass();

    virtual QString name() const;

    virtual QScriptValue::PropertyFlags propertyFlags ( const QScriptValue & object, const QScriptString & name, uint id );
    virtual QScriptClass::QueryFlags queryProperty ( const QScriptValue & object, const QScriptString & name, QScriptClass::QueryFlags flags, uint * id );
    virtual QScriptValue property(const QScriptValue &object, const QScriptString &name, uint id);
    virtual void setProperty(QScriptValue &object, const QScriptString &name, uint id, const QScriptValue &value);

private:
    Property *_linkedProperty;
    QMap<QString, PropertyClass*> _subPropertiesClasses;

    QScriptValue _proto;
};

#endif // PROPERTYCLASS_H
