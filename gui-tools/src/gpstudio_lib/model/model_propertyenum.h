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

#ifndef MODEL_BLOCKPROPERTYENUM_H
#define MODEL_BLOCKPROPERTYENUM_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QVariant>
#include <QDomElement>

class GPSTUDIO_LIB_EXPORT ModelPropertyEnum
{
public:
    ModelPropertyEnum();
    ModelPropertyEnum(const ModelPropertyEnum &modelPropertyEnum);
    ~ModelPropertyEnum();

    const QString &name() const;
    void setName(const QString &name);

    const QString &caption() const;
    void setCaption(const QString &caption);

    const QVariant &value() const;
    void setValue(const QVariant &value);

    const QString &description() const;
    void setDescription(const QString &description);

public:
    static ModelPropertyEnum *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelPropertyEnum *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _name;
    QString _caption;
    QVariant _value;
    QString _description;
};

#endif // MODEL_BLOCKPROPERTYENUM_H
