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

#ifndef MODEL_RESET_H
#define MODEL_RESET_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelReset
{
public:
    ModelReset(ModelBlock *parent=NULL);
    ~ModelReset();

    enum Direction {DirUndef, DirIn, DirOut};

    const QString &name() const;
    void setName(const QString &name);

    const QString &group() const;
    void setGroup(const QString &group);

    Direction direction() const;
    void setDirection(const Direction &direction);

    const QString &description() const;
    void setDescription(const QString &description);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

public:
    static ModelReset *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelReset *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _name;
    QString _group;
    Direction _direction;
    QString _description;

    ModelBlock *_parent;
};

#endif // MODEL_RESET_H
