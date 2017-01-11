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

#ifndef CAMERAUDP_H
#define CAMERAUDP_H

#include "gpstudio_com_common.h"
#include "cameraio.h"

class GPSTUDIO_COM_EXPORT CameraUDP : public CameraIO
{
public:
    CameraUDP();

    bool connect(const CameraInfo &info=CameraInfo());
    bool disconnect();
    bool resetDevice();
    bool isConnected() const;

    QByteArray read(const unsigned maxSize=512, const int timeOut=1000, bool *state=NULL);
    bool write(const QByteArray &array, const int timeOut=1000);

    static QVector<CameraInfo> avaibleCams();

    int sizePacket() const {return 1024;}

    int status() const;
};

#endif // CAMERAUDP_H
