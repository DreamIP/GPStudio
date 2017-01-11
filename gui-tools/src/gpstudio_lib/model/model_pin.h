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

#ifndef MODEL_PIN_H
#define MODEL_PIN_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>
#include <QList>

#include "model_attribute.h"

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelPin
{
public:
    ModelPin(ModelBlock *parent=NULL);
    ModelPin(const ModelPin &modelPin);
    ~ModelPin();

    const QString &name() const;
    void setName(const QString &name);

    const QString &mapTo() const;
    void setMapTo(const QString &mapTo);

    QList<ModelAttribute *> &attributes();
    const QList<ModelAttribute *> &attributes() const;
    void addAttribute(ModelAttribute *attribute);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

public:
    static ModelPin *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelPin *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _name;
    QString _mapTo;

    ModelBlock *_parent;

    QList<ModelAttribute *> _attributes;
};

#endif // MODEL_PIN_H
