/****************************************************************************
** Copyright (C) 2016 Dream IP
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

#include "camerainfo.h"

CameraInfo::CameraInfo(const QString &name, const QString &driverType, const QString &addr)
{
    setName(name);;
    setDriverType(driverType);
    setAddr(addr);
}

QString CameraInfo::name() const
{
    return _name;
}

void CameraInfo::setName(const QString &name)
{
    _name = name;
}

QString CameraInfo::driverType() const
{
    return _driverType;
}

void CameraInfo::setDriverType(const QString &driverType)
{
    _driverType = driverType;
}

QString CameraInfo::addr() const
{
    return _addr;
}

void CameraInfo::setAddr(const QString &addr)
{
    _addr = addr;
}

bool CameraInfo::isValid() const
{
    if(_name.isEmpty() || _driverType.isEmpty())
        return false;
    else
        return true;
}

QDebug operator<<(QDebug d, const CameraInfo &cameraInfo)
{
    QString outDebug=QString("%1 connected at %2 by %3").arg(cameraInfo._name)
                                                        .arg(cameraInfo._addr)
                                                        .arg(cameraInfo._driverType);
    d << outDebug;
    return d;
}
