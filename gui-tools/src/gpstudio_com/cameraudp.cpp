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

#include "cameraudp.h"

CameraUDP::CameraUDP()
{
}

bool CameraUDP::connect(const CameraInfo &info)
{
    Q_UNUSED(info);
    return true;
}

bool CameraUDP::disconnect()
{
    return true;
}

bool CameraUDP::resetDevice()
{
    return true;
}

bool CameraUDP::isConnected() const
{
    return true;
}

QByteArray CameraUDP::read(const unsigned maxSize, const int timeOut, bool *state)
{
    Q_UNUSED(maxSize);
    Q_UNUSED(timeOut);
    Q_UNUSED(state);
    return QByteArray();
}

bool CameraUDP::write(const QByteArray &array, const int timeOut)
{
    Q_UNUSED(array);
    Q_UNUSED(timeOut);
    return true;
}

QVector<CameraInfo> CameraUDP::avaibleCams()
{
    QVector<CameraInfo> avaibleCams;
    return avaibleCams;
}

int CameraUDP::status() const
{
    return 0;
}
