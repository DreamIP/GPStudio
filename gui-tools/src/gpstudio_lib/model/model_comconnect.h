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

#ifndef MODEL_COMCONNECT_H
#define MODEL_COMCONNECT_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QList>
#include <QDomElement>

class GPSTUDIO_LIB_EXPORT ModelComConnect
{
public:
    ModelComConnect();
    ~ModelComConnect();

    const QString &link() const;
    void setLink(const QString &link);

    const QString &id() const;
    void setId(const QString &id);

    const QString &type() const;
    void setType(const QString &type);

public:
    static ModelComConnect *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelComConnect *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _link;
    QString _id;
    QString _type;
};

#endif // MODEL_COMCONNECT_H
