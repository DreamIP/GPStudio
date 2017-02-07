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

#ifndef CAMERAIO_H
#define CAMERAIO_H

#include <QByteArray>
#include <QString>
#include <QVector>

#include "gpstudio_com_common.h"
#include "camerainfo.h"

class GPSTUDIO_COM_EXPORT CameraIO
{
public:
    CameraIO();
    virtual ~CameraIO();

    virtual bool connect(const CameraInfo &info=CameraInfo()) =0;
    virtual bool disconnect() =0;
    virtual bool resetDevice() =0;
    virtual bool isConnected() const =0;

    virtual QByteArray read(const unsigned maxSize=512, const int timeOut=1000, bool *state=NULL) =0;
    virtual bool write(const QByteArray &array, const int timeOut=1000) =0;

    virtual int sizePacket() const =0;

    virtual int status() const =0;

    static CameraIO *getCamIO(const QString &driver);
};

#endif // CAMERAIO_H
