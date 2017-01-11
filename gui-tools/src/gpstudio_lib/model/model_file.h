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

#ifndef MODEL_FILE_H
#define MODEL_FILE_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelFile
{
public:
    ModelFile(ModelBlock *parent=NULL);
    ~ModelFile();

    const QString &name() const;
    void setName(const QString &name);

    const QString &type() const;
    void setType(const QString &type);

    const QString &group() const;
    void setGroup(const QString &group);

    const QString &path() const;
    void setPath(const QString &path);

    const QString &description() const;
    void setDescription(const QString &description);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

public:
    static ModelFile *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelFile *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _name;
    QString _type;
    QString _group;
    QString _path;
    QString _description;

    ModelBlock *_parent;
};

#endif // MODEL_FILE_H
