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

#ifndef CAMERAUSB_H
#define CAMERAUSB_H

#include "gpstudio_com_common.h"
#include "cameraio.h"

#include <../../thirdparts/libusb-1.0/libusb.h>

class GPSTUDIO_COM_EXPORT CameraUSB : public CameraIO
{
public:
    CameraUSB();
    virtual ~CameraUSB();

    bool connect(const CameraInfo &info=CameraInfo());
    bool disconnect();
    bool resetDevice();
    bool isConnected() const;

    QByteArray read(const unsigned sizePacket=512, const int timeOut=1000, bool *state=NULL);
    bool write(const QByteArray &array, const int timeOut=1000);

    static QVector<CameraInfo> avaibleCams(const CameraInfo &info);

    int sizePacket() const {return 512;}

    int status() const;

private:
    void flush();

private:
    libusb_context *_ctx;
    libusb_device_handle *_devHandle;
    libusb_device *_device;

    uint16_t _vendorId;
    uint16_t _productId;
    int _epOut;
    int _epIn;
    int _interfaceNumber;

    /*const static uint16_t vendorId = 0x0403;
    const static uint16_t productId = 0x601E;
    const static int EPOUT = 0x02;
    const static int EPIN = 0x82;
    const static int InterfaceNumber = 1;*/
};

#endif // CAMERAUSB_H
