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

#ifndef MODEL_PARAMBITFIELD_H
#define MODEL_PARAMBITFIELD_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QVariant>
#include <QDomElement>

class ModelParam;

class GPSTUDIO_LIB_EXPORT ModelParamBitField
{
public:
    ModelParamBitField();
    ModelParamBitField(const ModelParamBitField &modelParamBitField);
    ~ModelParamBitField();

    const QString &name() const;
    void setName(const QString &name);

    const QString &type() const;
    void setType(const QString &type);

    const QVariant &value() const;
    void setValue(const QVariant &value);

    const QString &bitfield() const;
    void setBitfield(const QString &bitfield);

    const QString &description() const;
    void setDescription(const QString &description);

    const QString &propertyMap() const;
    void setPropertyMap(const QString &propertyMap);

    ModelParam *parent() const;
    void setParent(ModelParam *parent);

public:
    static ModelParamBitField *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelParamBitField *> listFromNodeGenerated(const QDomElement &domElement);

    static QList<uint> listBitFromBitField(const QString &bitField);

protected:
    QString _name;
    QString _type;
    QVariant _value;
    QString _bitfield;
    QString _description;
    QString _propertyMap;

    ModelParam *_parent;
};

#endif // MODEL_PARAMBITFIELD_H
