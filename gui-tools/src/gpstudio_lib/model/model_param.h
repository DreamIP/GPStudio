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

#ifndef MODEL_PARAM_H
#define MODEL_PARAM_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QVariant>
#include <QList>
#include <QDomElement>

#include "model_parambitfield.h"

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelParam
{
public:
    ModelParam(ModelBlock *parent=NULL);
    ModelParam(const ModelParam &modelParam);
    ~ModelParam();

    const QString &name() const;
    void setName(const QString &name);

    const QString &type() const;
    void setType(const QString &type);

    qint32 regAddr() const;
    void setRegAddr(const qint32 &regAddr);
    qint32 absoluteAddr() const;

    const QVariant &value() const;
    void setValue(const QVariant &value);

    const QVariant &defaultValue() const;
    void setDefaultValue(const QVariant &defaultValue);

    const QVariant &min() const;
    void setMin(const QVariant &min);

    const QVariant &max() const;
    void setMax(const QVariant &max);

    bool isHard() const;
    void setHard(bool hard);

    const QString &propertyMap() const;
    void setPropertyMap(const QString &propertyMap);

    const QString &description() const;
    void setDescription(const QString &description);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

    bool isDynamicParam() const;

    QList<ModelParamBitField *> &paramBitFields();
    const QList<ModelParamBitField *> &paramBitFields() const;
    void addParamBitField(ModelParamBitField *bitField);
    void addParamBitFields(const QList<ModelParamBitField *> &bitFields);

public:
    static ModelParam *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelParam *> listFromNodeGenerated(const QDomElement &domElement);
    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    QString _type;
    qint32 _regAddr;
    QVariant _value;
    QVariant _defaultValue;
    QVariant _min;
    QVariant _max;
    bool _hard;
    QString _description;
    QString _propertyMap;

    ModelBlock *_parent;

    QList<ModelParamBitField *> _parambitfields;
};

#endif // MODEL_PARAM_H
