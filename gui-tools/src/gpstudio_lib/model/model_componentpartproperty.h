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

#ifndef MODELCOMPONENTPARTPROPERTY_H
#define MODELCOMPONENTPARTPROPERTY_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QList>
#include <QDomElement>
#include <QPoint>
#include <QSize>

class ModelComponentPart;

class GPSTUDIO_LIB_EXPORT ModelComponentPartProperty
{
public:
    ModelComponentPartProperty();
    ModelComponentPartProperty(const ModelComponentPartProperty &other);
    ~ModelComponentPartProperty();

    QString name() const;
    void setName(const QString &name);

    QPoint pos() const;
    void setPos(const QPoint &pos);

    QSize size() const;
    void setSize(const QSize &size);

    ModelComponentPart *parent() const;
    void setParent(ModelComponentPart *parent);

public:
    static ModelComponentPartProperty *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelComponentPartProperty *> listFromNodeGenerated(const QDomElement &domElement);

    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    QPoint _pos;
    QSize _size;

    ModelComponentPart *_parent;
};

#endif // MODELCOMPONENTPARTPROPERTY_H
