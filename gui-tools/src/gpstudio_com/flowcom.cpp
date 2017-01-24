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

#include "flowcom.h"

#include <QDebug>
#include <QMutexLocker>

FlowCom::FlowCom(const int idFlow)
    : _idFlow(idFlow)
{
}

FlowCom::FlowCom(const FlowCom &other)
    : _idFlow(other._idFlow), _numPacket(other._numPacket), _flowDataToSend(other._flowDataToSend)
{
}

unsigned char FlowCom::idFlow() const
{
    return _idFlow;
}

void FlowCom::setIdFlow(const unsigned char idFlow)
{
    _idFlow = idFlow;
}

unsigned int FlowCom::numPacket() const
{
    return _numPacket;
}

bool FlowCom::readyToSend() const
{
    if(_flowDataToSend.size()!=0)
        return true;
    else
        return false;
}

QByteArray FlowCom::dataToSend(const int size)
{
    QByteArray dataToSend;

    if(_flowDataToSend.head().empty())
        _flowDataToSend.dequeue();
    if(_flowDataToSend.empty())
        return QByteArray();
    if(size<=4)
        return QByteArray();

    bool newPack = _flowDataToSend.head().isNewPack();
    QByteArray data = _flowDataToSend.head().getPart(size-4);

    dataToSend.append(_idFlow);
    if(_flowDataToSend.head().empty())
    {
        if(newPack)
            dataToSend.append(0xBC);    // start + end
        else
        {
            _flowDataToSend.dequeue();
            dataToSend.append(0xBA);    // end
        }
    }
    else
    {
        if(newPack)
            dataToSend.append(0xAA);    // start
        else
            dataToSend.append(0xCC);    // middle
    }

    dataToSend.append((unsigned char)_numPacket);
    dataToSend.append((unsigned char)_numPacket/256);
    dataToSend.append(data);

       // rajouter des 0 en fin de packet
   // for(int i=dataToSend.size(); i<size; i++) dataToSend.append((char)0);

    return dataToSend;
}

void FlowCom::send(const FlowPackage &flowData)
{
    _flowDataToSend.append(flowData);
}

void FlowCom::send(const QImage &image)
{
    send(FlowPackage(image));
}

void FlowCom::send(const QByteArray &data)
{
    send(FlowPackage(data));
}

void FlowCom::appendData(const QByteArray &data)
{
    _current.appendData(data.mid(4));
}

void FlowCom::appendData(const FlowPackage &data)
{
    _current = data;
    validate();
}

FlowPackage FlowCom::getData()
{
    QMutexLocker locker(&_mutexDataRead);
    return _lastFlowData;
}

void FlowCom::validate()
{
    _mutexDataRead.lock();
    _lastFlowData = _current;
    _current.clear();
    _mutexDataRead.unlock();
}

unsigned FlowCom::getSize() const
{
    return _current.data().size();
}
