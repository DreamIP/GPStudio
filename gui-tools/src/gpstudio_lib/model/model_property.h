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

#ifndef MODEL_BLOCKPROPERTY_H
#define MODEL_BLOCKPROPERTY_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QVariant>
#include <QList>
#include <QMap>
#include <QDomElement>

class ModelBlock;

#include "model_propertyenum.h"

class GPSTUDIO_LIB_EXPORT ModelProperty
{
public:
    ModelProperty();
    ModelProperty(const ModelProperty &modelProperty);
    ~ModelProperty();

    const QString &name() const;
    void setName(const QString &name);

    const QString &caption() const;
    void setCaption(const QString &caption);

    const QString &type() const;
    void setType(const QString &type);

    const QVariant &value() const;
    void setValue(const QVariant &value);

    const QString &min() const;
    void setMin(const QString &min);

    const QString &max() const;
    void setMax(const QString &max);

    const QString &step() const;
    void setStep(const QString &step);

    const QString &assert() const;
    void setAssert(const QString &assert);

    const QString &propertymap() const;
    void setPropertymap(const QString &propertymap);

    const QString &onchange() const;
    void setOnchange(const QString &onchange);

    const QString &description() const;
    void setDescription(const QString &description);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

    QList<ModelProperty *> &properties();
    const QList<ModelProperty *> &properties() const;
    void addProperty(ModelProperty *property);
    void addProperty(QList<ModelProperty *> properties);
    ModelProperty *getProperty(const QString &name) const;
    ModelProperty *getPropertyPath(const QString &path) const;
    QString getPath() const;

    QList<ModelPropertyEnum *> &propertyEnums();
    const QList<ModelPropertyEnum *> &propertyEnums() const;
    void addPropertyEnum(ModelPropertyEnum *propertyEnum);
    void addPropertyEnum(QList<ModelPropertyEnum *> propertyEnums);

public:
    static ModelProperty *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelProperty *> listFromNodeGenerated(const QDomElement &domElement);
    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    QString _caption;
    QString _type;
    QVariant _value;
    QString _min;
    QString _max;
    QString _step;
    QString _assert;
    QString _onchange;
    QString _propertymap;
    QString _description;

    ModelBlock *_parent;
    ModelProperty *_parentProperty;

    QList<ModelProperty *> _properties;
    QMap<QString, ModelProperty*> _propertiesMap;

    QList<ModelPropertyEnum *> _propertyEnums;
};

#endif // MODEL_BLOCKPROPERTY_H
