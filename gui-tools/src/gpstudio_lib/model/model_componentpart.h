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

#ifndef MODELCOMPONENTPART_H
#define MODELCOMPONENTPART_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QList>
#include <QDomElement>
#include <QPoint>

#include "model_componentpartflow.h"
#include "model_componentpartproperty.h"

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelComponentPart
{
public:
    ModelComponentPart();
    ModelComponentPart(const ModelComponentPart &other);
    ~ModelComponentPart();

    QString name() const;
    void setName(const QString &name);

    QPoint pos() const;
    void setPos(const QPoint &pos);

    QString draw() const;
    void setDraw(const QString &draw);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

    QList<ModelComponentPartFlow *> &flows();
    const QList<ModelComponentPartFlow *> &flows() const;
    void addFlow(ModelComponentPartFlow *flow);
    void addFlows(const QList<ModelComponentPartFlow *> &flows);
    ModelComponentPartFlow *getFlow(const QString &name) const;

    QList<ModelComponentPartProperty *> &properties();
    const QList<ModelComponentPartProperty *> &properties() const;
    void addProperty(ModelComponentPartProperty *property);
    void addProperties(const QList<ModelComponentPartProperty *> &properties);
    ModelComponentPartProperty *getProperty(const QString &name) const;

public:
    static ModelComponentPart *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelComponentPart *> listFromNodeGenerated(const QDomElement &domElement);

    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    QPoint _pos;
    QString _draw;

    QList<ModelComponentPartFlow *> _flows;
    QList<ModelComponentPartProperty *> _properties;

    ModelBlock *_parent;
};

#endif // MODELCOMPONENTPART_H
