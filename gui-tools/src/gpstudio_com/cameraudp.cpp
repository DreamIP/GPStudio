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

#include <QUdpSocket>
#include <QHostAddress>

CameraUDP::CameraUDP()
{
}

bool CameraUDP::connect(const CameraInfo &info)
{
    Q_UNUSED(info);
    _udpSocket = new QUdpSocket();
    if (!_udpSocket->bind(QHostAddress("172.27.1.74"), 1947))
    {
        qDebug()<<"Failed bind";
    }
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

    QHostAddress sender;
    quint16 senderPort;
    QByteArray datagram;

    (*state) = true;
    if(!_udpSocket->hasPendingDatagrams())
        return QByteArray();

    datagram.resize(_udpSocket->pendingDatagramSize());

    _udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
    if(sender == QHostAddress("172.27.10.5"))
    {
        return datagram;
    }

    return QByteArray();
}

bool CameraUDP::write(const QByteArray &array, const int timeOut)
{
    Q_UNUSED(timeOut);
    if(array.size()==0)
        return true;
    _udpSocket->writeDatagram(array.data(), array.size(), QHostAddress("172.27.10.5"), 32768);
    return true;
}

QVector<CameraInfo> CameraUDP::avaibleCams(const CameraInfo &info)
{
    Q_UNUSED(info);
    QVector<CameraInfo> avaibleCams;
    avaibleCams.append(CameraInfo("DreamCam Ethernet", "Eth", "172.27.10.5"));
    return avaibleCams;
}

int CameraUDP::status() const
{
    return 0;
}
