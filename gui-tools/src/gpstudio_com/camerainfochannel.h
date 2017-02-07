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

#ifndef CAMERAINFOCHANNEL_H
#define CAMERAINFOCHANNEL_H

#include "gpstudio_com_common.h"

#include <QString>

class GPSTUDIO_COM_EXPORT CameraInfoChannel
{
public:
    enum ChannelType {
        UndefChannel,
        FlowIn,
        FlowOut,
        ParamIn,
        ParamOut
    };

    CameraInfoChannel();
    CameraInfoChannel(ChannelType channelType, int id);
    CameraInfoChannel(const QString &channelTypeName, int id);

    ChannelType channelType() const;
    void setChannelType(ChannelType channelType);
    void setChannelType(const QString &channelTypeName);
    static ChannelType channelTypeFromString(const QString &channelTypeName);

    int id() const;
    void setId(int id);

    bool isValid() const;

protected:
    ChannelType _channelType;
    int _id;
};

#endif // CAMERAINFOCHANNEL_H
