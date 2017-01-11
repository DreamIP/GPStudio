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

#ifndef CAMERAREGISTERBITFIELD_H
#define CAMERAREGISTERBITFIELD_H

#include "gpstudio_lib_common.h"

#include <QObject>
#include <QStringList>
#include <QList>

class ModelParamBitField;
class Camera;
class Register;

class RegisterBitField : public QObject
{
    Q_OBJECT
public:
    RegisterBitField();
    ~RegisterBitField();

    uint mask() const;
    void setMask(const uint &mask);

    uint value() const;
    void setValue(const uint &value);

    QString blockName() const;
    void setBlockName(const QString &blockName);

    QString propertyMap() const;
    void setPropertyMap(const QString &propertyMap);
    QStringList dependsProperties() const;

    Register *parent() const;
    void setParent(Register *parent);

    const QList<uint> &bits() const;

public:
    static RegisterBitField *fromParamBitField(const ModelParamBitField *paramBitField);

signals:
    void bitfieldChange(uint mask, uint value);

public slots:
    void eval();

private:
    uint _mask;
    uint _value;
    QString _blockName;
    QString _propertyMap;

    QList<uint> _bits;

    Register *_parent;
};

#endif // CAMERAREGISTERBITFIELD_H
