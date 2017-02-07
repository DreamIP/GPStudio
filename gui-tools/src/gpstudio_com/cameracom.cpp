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

#include "cameracom.h"

// camera io driver
#include "camerausb.h"

#include <QDebug>
#include <QDateTime>

CameraCom::CameraCom(const CameraInfo &cameraInfo)
{
    _cameraIO = NULL;
    _info = cameraInfo;

    _cameraIO = CameraIO::getCamIO(cameraInfo.driverType());

    if(_cameraIO)
        _cameraIO->connect(cameraInfo);

    FlowCom *flowCom;
    foreach (CameraInfoChannel channel, cameraInfo.channels())
    {
        switch (channel.channelType())
        {
        case CameraInfoChannel::FlowIn:
            _inputFlows.append(new FlowCom(channel.id()));
            break;
        case CameraInfoChannel::FlowOut:
            _outputFlows.append(new FlowCom(channel.id()));
            break;
        case CameraInfoChannel::ParamIn:
            // TODO implement me
            break;
        case CameraInfoChannel::ParamOut:
            flowCom = new FlowCom(channel.id());
            _outputFlows.append(flowCom);   // set param
            _paramFlow = flowCom;
            break;
        case CameraInfoChannel::UndefChannel:
            break;
        }
    }

    _start=true;
    start(QThread::NormalPriority);
}

CameraCom::~CameraCom()
{
    terminate();
    delete _cameraIO;
}

bool CameraCom::isConnected() const
{
    if(_cameraIO)
        return _cameraIO->isConnected();
    else
        return false;
}

void CameraCom::stop()
{
    _start=false;
    this->wait();
}

QVector<CameraInfo> CameraCom::avaibleCams(const CameraInfo &info)
{
    QVector<CameraInfo> avaibleCams;

    avaibleCams += CameraUSB::avaibleCams(info);

    return avaibleCams;
}

FlowCom *CameraCom::inputFlow(unsigned char idFlow) const
{
    for(int i=0; i<_inputFlows.size(); i++)
    {
        if(_inputFlows[i]->idFlow() == idFlow)
            return _inputFlows[i];
    }
    return NULL;
}

const QList<FlowCom*> &CameraCom::inputFlows() const
{
    return _inputFlows;
}

QList<FlowCom*> &CameraCom::inputFlows()
{
    return _inputFlows;
}

FlowCom *CameraCom::outputFlow(unsigned char idFlow) const
{
    for(int i=0; i<_outputFlows.size(); i++)
    {
        if(_outputFlows[i]->idFlow() == idFlow)
            return _outputFlows[i];
    }
    return NULL;
}

const QList<FlowCom*> &CameraCom::outputFlows() const
{
    return _outputFlows;
}

QList<FlowCom*> &CameraCom::outputFlows()
{
    return _outputFlows;
}

void CameraCom::run()
{
    QMap<int,int> prev_numpacket;

    for (int i=0;i<_inputFlows.size();i++)
        prev_numpacket[_inputFlows[i]->idFlow()] = -1;

    bool succes;

    while(_start && _cameraIO != NULL)
    {
        const QByteArray &received = _cameraIO->read(512*128, 1, &succes);
        if(!succes)
        {
            qDebug()<<"Com failed to read";
            _cameraIO->disconnect();
            emit disconnected();
            return;
        }

        int start=0;
        while(start<received.size())
        {
            const QByteArray &packet = received.mid(start, 512);
            if(packet.size()<=4)
            {
                start+=512;
                continue;
            }

            unsigned char idFlow = packet[0];
            unsigned char flagFlow = packet[1];
            unsigned short numpacket = ((unsigned short)((unsigned char)packet[2])*256)+(unsigned char)packet[3];

            for(int i=0; i<_inputFlows.size(); i++)
            {
                if(_inputFlows[i]->idFlow()==idFlow)
                {
                    if(prev_numpacket[idFlow]!=-1)
                    {
                        if(prev_numpacket[idFlow]+1 != numpacket)
                        {
                            int miss_packet = numpacket-prev_numpacket[idFlow]-1;
                            qDebug()<<"miss"<<miss_packet<<numpacket;
                            for(int j=0; j<miss_packet; j++)
                                _inputFlows[i]->appendData(QByteArray(512,0));
                        }
                    }
                    _inputFlows[i]->appendData(packet);
                    prev_numpacket[idFlow] = numpacket;

                    if(flagFlow==0xBA)      // end of flow
                    {
                        prev_numpacket[idFlow]=-1;
                        _inputFlows[i]->validate();
                        emit flowReadyToRead(i);
                    }
                }
            }
            if(idFlow == 0xFD)
            {
                //emit flowReadyToRead(received);
            }

            start+=512;
        }
        for(int i=0; i<_outputFlows.size(); i++)
        {
            if(_outputFlows[i]->readyToSend())
            {
                const QByteArray data = _outputFlows[i]->dataToSend(_cameraIO->sizePacket());
                succes = _cameraIO->write(data, 1);
                if(!succes)
                {
                    qDebug()<<"Com failed to write";
                    _cameraIO->disconnect();
                    emit disconnected();
                    return;
                }
            }
        }
    }
}

const CameraInfo CameraCom::info() const
{
    return _info;
}

CameraIO *CameraCom::cameraIO() const
{
    return _cameraIO;
}

void CameraCom::writeParam(const unsigned int addr, const unsigned int value)
{
    QByteArray paramFlow;

    // addr
    paramFlow.append((char)(addr >> 24));
    paramFlow.append((char)(addr >> 16));
    paramFlow.append((char)(addr >> 8));
    paramFlow.append((char)addr);
    // data
    paramFlow.append((char)(value >> 24));
    paramFlow.append((char)(value >> 16));
    paramFlow.append((char)(value >> 8));
    paramFlow.append((char)value);

    _paramFlow->send(paramFlow);
}

void CameraCom::writeParam(const unsigned int addr, const unsigned int *data, const unsigned size)
{
    QByteArray paramFlow;
    unsigned int addrCurrent = addr;

    for(unsigned int i=0; i<size; i++)
    {
        unsigned int valueCurrent = data[i];

        // addr
        paramFlow.append((char)(addrCurrent >> 24));
        paramFlow.append((char)(addrCurrent >> 16));
        paramFlow.append((char)(addrCurrent >> 8));
        paramFlow.append((char)addrCurrent);
        // data
        paramFlow.append((char)(valueCurrent >> 24));
        paramFlow.append((char)(valueCurrent >> 16));
        paramFlow.append((char)(valueCurrent >> 8));
        paramFlow.append((char)valueCurrent);

        addrCurrent++;
    }

    _paramFlow->send(paramFlow);
}

void CameraCom::fakeDataReceived(int idFlow, const FlowPackage &package)
{
    if(idFlow >= _inputFlows.size())
        return;

    _inputFlows[idFlow]->appendData(package);
    emit flowReadyToRead(idFlow);
}
