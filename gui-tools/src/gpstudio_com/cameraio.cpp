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

#include "cameraio.h"

#include "camerausb.h"
#include "cameraudp.h"

CameraIO::CameraIO()
{
}

CameraIO::~CameraIO()
{
}

CameraIO *CameraIO::getCamIO(const QString &driver)
{
    if(driver.contains("USB", Qt::CaseInsensitive))
        return new CameraUSB();
    if(driver.contains("UDP", Qt::CaseInsensitive))
        return new CameraUDP();
    return NULL;
}

QVector<CameraInfo> CameraIO::avaibleCams(const CameraInfo &info)
{
    if(info.driverType().contains("USB", Qt::CaseInsensitive))
        return CameraUSB::avaibleCams(info);
    if(info.driverType().contains("UDP", Qt::CaseInsensitive))
        return CameraUDP::avaibleCams(info);
    return QVector<CameraInfo>();
}
