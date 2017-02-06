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

#ifndef CAMERAINFO_H
#define CAMERAINFO_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QDebug>

#include "gpstudio_com_common.h"

class GPSTUDIO_COM_EXPORT CameraInfo
{
public:
    CameraInfo(const QString &name=QString(), const QString &driverType=QString(), const QString &addr=QString());
    CameraInfo(const CameraInfo &other);

    QString name() const;
    void setName(const QString &name);

    QString driverType() const;
    void setDriverType(const QString &driverType);

    QString addr() const;
    void setAddr(const QString &addr);

    QMap<QString, QVariant> paramsCom() const;
    void setParam(const QString &name, const QVariant &value);
    QVariant getParam(const QString &name) const;

    bool isValid() const;

    friend QDebug operator<< (QDebug d, const CameraInfo &cameraInfo);

private:
    QString _name;
    QString _driverType;
    QString _addr;

    QMap<QString, QVariant> _paramsCom;
};

#endif // CAMERAINFO_H
