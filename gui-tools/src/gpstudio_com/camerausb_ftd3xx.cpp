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

#include "camerausb_ftd3xx.h"

#include <QDebug>

#include "../../thirdparts/ftd3xx/ftd3xx.h"

CameraUSB_FTD3XX::CameraUSB_FTD3XX()
{
    _ctx = NULL;
    _devHandle = NULL;
    _device = NULL;
}

CameraUSB_FTD3XX::~CameraUSB_FTD3XX()
{
    disconnect();
}

bool CameraUSB_FTD3XX::connect(const CameraInfo &info)
{
    int ret;

    // com settings
    _vendorId = info.getParam("vendorId").toString().toInt(0, 16);
    _productId = info.getParam("productId").toString().toInt(0, 16);
    _epIn = info.getParam("EPIN").toString().toInt(0, 16);
    _epOut = info.getParam("EPOUT").toString().toInt(0, 16);
    _interfaceNumber = info.getParam("interface").toString().toInt(0, 16);
    if(_vendorId == 0 || _productId == 0)
    {
        qDebug()<<"USB bad vendorId/productId settings";
        return false;
    }

    DWORD count;
    FT_DEVICE_LIST_INFO_NODE nodes[16];

    FT_CreateDeviceInfoList(&count);
    printf("Total %u device(s)\r\n", count);
    if (!count)
            return false;

    if (FT_OK != FT_GetDeviceInfoList(nodes, &count))
            return false;
    return true;
}

bool CameraUSB_FTD3XX::disconnect()
{
    if(_devHandle)
    {

        _devHandle = NULL;
    }

    return true;
}

bool CameraUSB_FTD3XX::resetDevice()
{
    if(_devHandle)
    {

    }

    return true;
}

bool CameraUSB_FTD3XX::isConnected() const
{
    return (_devHandle!=NULL);
}

QByteArray CameraUSB_FTD3XX::read(const unsigned sizePacket, const int timeOut, bool *state)
{
    return QByteArray();
}

bool CameraUSB_FTD3XX::write(const QByteArray &array, const int timeOut)
{
    if(!_devHandle)
        return false;

    int transferredByte = 0;

    return true;
}

QVector<CameraInfo> CameraUSB_FTD3XX::avaibleCams(const CameraInfo &info)
{
    QVector<CameraInfo> avaibleCams;

    libusb_context *ctx = NULL;
    libusb_device **devs;
    ssize_t cnt;

    int vendorId = info.getParam("vendorId").toString().toInt(0, 16);
    int productId = info.getParam("productId").toString().toInt(0, 16);

    return avaibleCams;
}
