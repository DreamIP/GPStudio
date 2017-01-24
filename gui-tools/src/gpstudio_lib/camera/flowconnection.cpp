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

#include "flowconnection.h"

FlowConnection::FlowConnection()
{
    _fps=0;
}

int FlowConnection::idFlow() const
{
    return _flowId;
}

void FlowConnection::setIdFlow(int flowId)
{
    _flowId = flowId;
}

Flow *FlowConnection::flow() const
{
    return _flow;
}

void FlowConnection::setFlow(Flow *flow)
{
    _flow = flow;
}

void FlowConnection::recImg(FlowPackage data)
{
    // fps calculation
    if(_prevImgReceive.isValid())
    {
        _fps = 1.0 / (QDateTime::currentDateTime().toMSecsSinceEpoch() - _prevImgReceive.toMSecsSinceEpoch()) * 1000.0;
    }
    _prevImgReceive = QDateTime::currentDateTime();

    // rec notification
    _lastData = data;
    emit flowReceived(data);
}

float FlowConnection::fps() const
{
    return _fps;
}

const FlowPackage &FlowConnection::lastData() const
{
    return _lastData;
}
