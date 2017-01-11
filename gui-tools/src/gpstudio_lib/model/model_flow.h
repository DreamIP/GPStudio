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

#ifndef MODEL_FLOW_H
#define MODEL_FLOW_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>

#include "model_property.h"

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelFlow
{
public:
    ModelFlow(ModelBlock *parent=NULL);
    ModelFlow(const ModelFlow &modelFlow);
    ~ModelFlow();

    const QString &name() const;
    void setName(const QString &name);

    const QString &type() const;
    void setType(const QString &type);

    quint8 size() const;
    void setSize(const quint8 &size);

    const QString &description() const;
    void setDescription(const QString &description);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

    QList<ModelProperty *> &properties();
    const QList<ModelProperty *> &properties() const;
    void addProperty(ModelProperty *property);
    void addProperties(const QList<ModelProperty *> &properties);
    ModelProperty *getBlockProperty(const QString &name) const;

public:
    static ModelFlow *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelFlow *> listFromNodeGenerated(const QDomElement &domElement);
    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    QString _type;
    quint8 _size;
    QString _description;

    QList<ModelProperty *> _properties;

    ModelBlock *_parent;
};

#endif // MODEL_FLOW_H
