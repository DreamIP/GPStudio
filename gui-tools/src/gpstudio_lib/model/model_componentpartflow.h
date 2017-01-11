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

#ifndef MODELCOMPONENTPARTFLOW_H
#define MODELCOMPONENTPARTFLOW_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QList>
#include <QDomElement>
#include <QPoint>

class ModelComponentPart;

class GPSTUDIO_LIB_EXPORT ModelComponentPartFlow
{
public:
    ModelComponentPartFlow();
    ModelComponentPartFlow(const ModelComponentPartFlow &other);
    ~ModelComponentPartFlow();

    QString name() const;
    void setName(const QString &name);

    QPoint pos() const;
    void setPos(const QPoint &pos);

    ModelComponentPart *parent() const;
    void setParent(ModelComponentPart *parent);

public:
    static ModelComponentPartFlow *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelComponentPartFlow *> listFromNodeGenerated(const QDomElement &domElement);

    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    QPoint _pos;

    ModelComponentPart *_parent;
};

#endif // MODELCOMPONENTPARTFLOW_H
