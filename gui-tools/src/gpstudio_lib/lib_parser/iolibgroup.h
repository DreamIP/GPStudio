/****************************************************************************
** Copyright (C) 2014 Dream IP
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

#ifndef IOLIBGROUP_H
#define IOLIBGROUP_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QStringList>
#include <QList>

class GPSTUDIO_LIB_EXPORT IOLibGroup
{
public:
    IOLibGroup(const QString &name=QString());
    ~IOLibGroup();

    QString name() const;
    void setName(const QString &name);

    bool isOptional() const;
    void setOptional(bool isOptional);

    const QStringList &ios() const;
    void addIos(const QString &name);

private:
    QString _name;
    QStringList _ios;
    bool _optional;
};

#endif // IOLIBGROUP_H
