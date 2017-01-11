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

#include "camerausb.h"

#include <QDebug>

CameraUSB::CameraUSB()
{
    _ctx = NULL;
    _devHandle = NULL;
    int ret = libusb_init(&_ctx); //initialize the library for the session
    if(ret < 0)
        qDebug()<<"Init Error "<<ret<<endl;
    //libusb_set_debug(_ctx, 3);
}

CameraUSB::~CameraUSB()
{
    disconnect();
    libusb_exit(_ctx);
}

bool CameraUSB::connect(const CameraInfo &info)
{
    if(info.addr().isEmpty())
    {
        qDebug()<<"addr empty";
        _devHandle = libusb_open_device_with_vid_pid(_ctx, 1204, 4099);
    }
    else
    {
        // get list usb device
        libusb_device **devs;
        ssize_t cnt = libusb_get_device_list(_ctx, &devs);
        for(int i = 0; i < cnt; i++)
        {
            libusb_device_descriptor desc;
            if(libusb_get_device_descriptor(devs[i], &desc)==LIBUSB_SUCCESS)
            {
                if(desc.idVendor==vendorId && desc.idProduct==productId)
                {
                    QString addr = QString("%1.%2").arg((int)libusb_get_bus_number(devs[i]))
                                                   .arg((int)libusb_get_device_address(devs[i]));
                    if(addr==info.addr())
                    {
                        libusb_open(devs[i], &_devHandle);
                    }
                }
            }
        }
        libusb_free_device_list(devs, 1);
    }

    if(_devHandle == NULL)
    {
        qDebug()<<"Cannot open device"<<endl;
        return false;
    }

    int ret = libusb_claim_interface(_devHandle, InterfaceNumber);
    if(ret != 0)
    {
        qDebug()<<"Cannot claim device"<<endl;
        return false;
    }
    if(libusb_reset_device(_devHandle)!= 0)
        qDebug()<<"Cannot reset device"<<endl;
    if(libusb_clear_halt(_devHandle, EP2)!= 0)
        qDebug()<<"Cannot clear EP2"<<endl;
    if(libusb_clear_halt(_devHandle, EP6)!= 0)
        qDebug()<<"Cannot clear EP6"<<endl;

    flush();

    //qDebug()<<libusb_get_max_packet_size(libusb_get_device(_devHandle), EP2);

    return true;
}

bool CameraUSB::disconnect()
{
    if(_devHandle)
    {
        libusb_release_interface(_devHandle, InterfaceNumber);
        libusb_close(_devHandle);
        _devHandle = NULL;
    }

    return true;
}

bool CameraUSB::resetDevice()
{
    if(_devHandle)
    {
        if(libusb_reset_device(_devHandle) != LIBUSB_SUCCESS)
            return false;
    }

    return true;
}

bool CameraUSB::isConnected() const
{
    return (_devHandle!=NULL);
}

QByteArray CameraUSB::read(const unsigned sizePacket, const int timeOut, bool *state)
{
    if(!_devHandle)
    {
        if(state)
            *state = false;
        return QByteArray();
    }

    unsigned char buffer[sizePacket];
    int transferredByte = 0;

    int ret = libusb_bulk_transfer(_devHandle, EP6, buffer, sizePacket, &transferredByte, timeOut);
    if(ret != 0)
    {
        if(ret==LIBUSB_ERROR_TIMEOUT)
        {
            /*qDebug()<<"Read timeout"<<endl;*/
        }
        else if(ret==LIBUSB_ERROR_NO_DEVICE)
        {
            // device disconnected
            qDebug()<<"Camera disconnected";
            if(state)
                *state = false;
            return QByteArray();
        }
        else
        {
            qDebug()<<"Cannot read packet"/*<<QString(libusb_strerror((enum libusb_error)ret))<<transferredByte*/;

            if(libusb_reset_device(_devHandle)!= 0)
                qDebug()<<"Cannot reset device"<<endl;
            if(libusb_clear_halt(_devHandle, EP2)!= 0)
                qDebug()<<"Cannot clear EP2"<<endl;
            if(libusb_clear_halt(_devHandle, EP6)!= 0)
                qDebug()<<"Cannot clear EP6"<<endl;

            if(state)
                *state = false;
            return QByteArray();
        }
    }
    if(state)
        *state = true;
    return QByteArray((const char *)buffer, transferredByte);
}

bool CameraUSB::write(const QByteArray &array, const int timeOut)
{
    if(!_devHandle) return false;

//    const int timeOut = 1000;
    int transferredByte;
    int ret = libusb_bulk_transfer(_devHandle, EP2, (unsigned char *)array.data(), array.size(), &transferredByte, timeOut);
    if(ret != LIBUSB_SUCCESS)
    {
        qDebug()<<"Cannot write packet"<<endl;
        return false;
    }
    return true;
}

void CameraUSB::flush()
{
    const int timeOut = 100;
    unsigned char buffer[1024];
    int transferredByte;
    while (libusb_bulk_transfer(_devHandle, EP6, buffer, 1024, &transferredByte, timeOut)==0 && transferredByte!=0);
}

QVector<CameraInfo> CameraUSB::avaibleCams()
{
    QVector<CameraInfo> avaibleCams;

    libusb_context *ctx = NULL;
    libusb_device **devs;
    ssize_t cnt;

    // create context
    if(libusb_init(&ctx) != LIBUSB_SUCCESS) return avaibleCams;

    // get list usb device
    cnt = libusb_get_device_list(ctx, &devs);
    for(int i = 0; i < cnt; i++)
    {
        libusb_device_descriptor desc;
        if(libusb_get_device_descriptor(devs[i], &desc)==LIBUSB_SUCCESS)
        {
            if(desc.idVendor==vendorId && desc.idProduct==productId)
            {
                QString addr = QString("%1.%2").arg((int)libusb_get_bus_number(devs[i]))
                                               .arg((int)libusb_get_device_address(devs[i]));
                avaibleCams.append(CameraInfo("DreamCam USB", "USB", addr));
            }
        }
    }
    libusb_free_device_list(devs, 1);

    // destroy usb context
    libusb_exit(ctx);
    return avaibleCams;
}

int CameraUSB::status() const
{
    unsigned char status[2];
    status[0]=0;
    status[1]=0;
    int ret = libusb_control_transfer(_devHandle, 0x82, 0, 0, EP2, status, 2, 1000);
    if(ret<0)
        return -1;
    return (*(unsigned short*)status);
}
