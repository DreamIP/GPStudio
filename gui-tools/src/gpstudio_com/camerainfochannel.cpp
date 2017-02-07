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

#include "camerainfochannel.h"

CameraInfoChannel::CameraInfoChannel()
{
    _channelType = UndefChannel;
    _id = -1;
}

CameraInfoChannel::CameraInfoChannel(CameraInfoChannel::ChannelType channelType, int id)
{
    _channelType = channelType;
    _id = id;
}

CameraInfoChannel::CameraInfoChannel(const QString &channelTypeName, int id)
{
    setChannelType(channelTypeFromString(channelTypeName));
    _id = id;
}

CameraInfoChannel::ChannelType CameraInfoChannel::channelType() const
{
    return _channelType;
}

void CameraInfoChannel::setChannelType(ChannelType channelType)
{
    _channelType = channelType;
}

void CameraInfoChannel::setChannelType(const QString &channelTypeName)
{
    setChannelType(channelTypeFromString(channelTypeName));
}

CameraInfoChannel::ChannelType CameraInfoChannel::channelTypeFromString(const QString &channelTypeName)
{
    if(channelTypeName == "flowin")
        return FlowIn;
    if(channelTypeName == "flowout")
        return FlowOut;
    if(channelTypeName == "paramin")
        return ParamIn;
    if(channelTypeName == "paramout")
        return ParamOut;
    return UndefChannel;
}

int CameraInfoChannel::id() const
{
    return _id;
}

void CameraInfoChannel::setId(int id)
{
    _id = id;
}

bool CameraInfoChannel::isValid() const
{
    return (_id != -1 && _channelType != UndefChannel);
}
