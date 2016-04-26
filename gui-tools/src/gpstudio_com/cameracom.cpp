/****************************************************************************
** Copyright (C) 2016 Dream IP
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

    if(cameraInfo.driverType().contains("USB", Qt::CaseInsensitive))
    {
        _cameraIO = new CameraUSB();
    }

    if(_cameraIO) _cameraIO->connect(cameraInfo);

    // TODO pass this part dynamic
    _outputFlow.append(new FlowCom(15));   // set param
    _paramFlow = _outputFlow[0];

    _outputFlow.append(new FlowCom(1));
    _outputFlow.append(new FlowCom(2));

    _inputFlow.append(new FlowCom(0x80));
    _inputFlow.append(new FlowCom(0x81));
    _inputFlow.append(new FlowCom(0x82));
    _inputFlow.append(new FlowCom(0x83));

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
    if(_cameraIO) return _cameraIO->isConnected();
    else return false;
}

void CameraCom::stop()
{
    _start=false;
    this->wait();
}

QVector<CameraInfo> CameraCom::avaibleCams()
{
    QVector<CameraInfo> avaibleCams;

    avaibleCams += CameraUSB::avaibleCams();

    return avaibleCams;
}

void CameraCom::run()
{
    QMap<int,int> prev_numpacket;

    for (int i=0;i<_inputFlow.size();i++)
    prev_numpacket[_inputFlow[i]->idFlow()] = -1;

    bool succes;

    while(_start)
    {
        const QByteArray &received = _cameraIO->read(512*128, 1, &succes);
//           const QByteArray &received = _cameraIO->read(512, 1, &succes);
//        qDebug() <<"rec: "<<received.mid(0,10).toHex();
        if(!succes)
        {
            qDebug()<<"fail";
            terminate();
        }

        int start=0;
        while(start<received.size())
        {
            const QByteArray &packet = received.mid(start, 512);

            unsigned char idFlow = packet[0];
            unsigned char flagFlow = packet[1];
            unsigned short numpacket = ((unsigned short)((unsigned char)packet[2])*256)+(unsigned char)packet[3];

            //qDebug()<<"receive:start"; qint64 time = QDateTime::currentDateTime().toMSecsSinceEpoch();
            for(int i=0; i<_inputFlow.size(); i++)
            {
                if(_inputFlow[i]->idFlow()==idFlow)
                {
                    if(prev_numpacket[idFlow]!=-1)
                    {
                        if(prev_numpacket[idFlow]+1 != numpacket)
                        {
                            int miss_packet = numpacket-prev_numpacket[idFlow]-1;
                            qDebug()<<"miss"<<miss_packet<<numpacket;
                            for(int j=0; j<miss_packet; j++) _inputFlow[i]->appendData(QByteArray(512,0));
                        }
                    }
                    _inputFlow[i]->appendData(packet);
                    prev_numpacket[idFlow] = numpacket;

                    if(flagFlow==0xBA)      // end of flow
                    {
                        prev_numpacket[idFlow]=-1;
                        _inputFlow[i]->validate();
                        emit flowReadyToRead(i);
                    }
                }
            }
            if(idFlow == 0xFD)
            {
                //emit flowReadyToRead(received);
            }
            //qDebug()<<"receive:stop"<<QDateTime::currentDateTime().toMSecsSinceEpoch()-time;

            start+=512;
        }
        for(int i=0; i<_outputFlow.size(); i++)
        {
            if(_outputFlow[i]->readyToSend())
            {
                const QByteArray data = _outputFlow[i]->dataToSend(_cameraIO->sizePacket());
                _cameraIO->write(data, 1);
            }
        }

        //QThread::sleep(100);
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
    //qDebug() << "param_trame: "<< byte.toHex();
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

const QList<FlowCom*> &CameraCom::outputFlow() const
{
    return _outputFlow;
}

QList<FlowCom*> &CameraCom::outputFlow()
{
    return _outputFlow;
}

const QList<FlowCom*> &CameraCom::inputFlow() const
{
    return _inputFlow;
}

QList<FlowCom*> &CameraCom::inputFlow()
{
    return _inputFlow;
}
