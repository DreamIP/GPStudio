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

#ifndef FLOW_COM_H
#define FLOW_COM_H

#include <QQueue>
#include <QMetaType>
#include <QMutex>

#include "gpstudio_com_common.h"
#include "flowpackage.h"

class GPSTUDIO_COM_EXPORT FlowCom
{
public:
    FlowCom(const int idFlow=0);
    FlowCom(const FlowCom &other);

    unsigned char idFlow() const;
    void setIdFlow(const unsigned char idFlow);

    unsigned int numPacket() const;

    // Flow OUT
    bool readyToSend() const;
    QByteArray dataToSend(const int size);

    void send(const FlowPackage &flowData);

    void send(const QImage &image);
    void send(const QByteArray &data);
    //void send(const Mat);
    //void send(const stdVector);

    // Flow IN
    void appendData(const QByteArray &data);

    FlowPackage getData();
    void validate();
    unsigned getSize() const;

private:
    unsigned char _idFlow;
    unsigned int _numPacket;

    QQueue<FlowPackage> _flowDataToSend;     // flowData to send

    FlowPackage _current;              // flowData currently received
    FlowPackage _lastFlowData;              // flowData currently received
    QMutex _mutexDataRead;
};

Q_DECLARE_METATYPE(FlowCom)

#endif // FLOW_COM_H
