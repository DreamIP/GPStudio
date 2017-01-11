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

#ifndef MODEL_PORT_H
#define MODEL_PORT_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelPort
{
public:
    ModelPort(ModelBlock *parent=NULL);
    ~ModelPort();

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

public:
    static ModelPort *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelPort *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _name;
    QString _type;
    quint8 _size;
    QString _description;

    ModelBlock *_parent;
};

#endif // MODEL_PORT_H
