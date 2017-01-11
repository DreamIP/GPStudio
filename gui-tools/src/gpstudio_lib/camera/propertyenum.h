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

#ifndef PROPERTYENUM_H
#define PROPERTYENUM_H

#include "gpstudio_lib_common.h"

#include <QVariant>
#include <QString>

class GPSTUDIO_LIB_EXPORT PropertyEnum
{
public:
    PropertyEnum(const QString &name=QString(), const QVariant &value=QVariant());
    PropertyEnum(const QString &name=QString(), const QString &caption=QString(), const QVariant &value=QVariant());
    ~PropertyEnum();

    QString name() const;
    void setName(const QString &name);

    QString caption() const;
    void setCaption(const QString &caption);

    QVariant value() const;
    void setValue(const QVariant &value);

private:
    QString _name;
    QString _caption;
    QVariant _value;
};

#endif // PROPERTYENUM_H
