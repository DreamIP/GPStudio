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

#ifndef FLOWCONNECTION_H
#define FLOWCONNECTION_H

#include "gpstudio_lib_common.h"

#include <QObject>
#include <QDateTime>

#include "flow.h"
#include "flowpackage.h"

class GPSTUDIO_LIB_EXPORT FlowConnection : public QObject
{
    Q_OBJECT
public:
    FlowConnection();

    int flowId() const;
    void setFlowId(int flowId);

    Flow *flow() const;
    void setFlow(Flow *flow);

    void recImg(FlowPackage data);
    float fps() const;

    const FlowPackage &lastData() const;

signals:
    void flowReceived(FlowPackage data);

private:
    int _flowId;
    Flow *_flow;

    FlowPackage _lastData;

    QDateTime _prevImgReceive;
    float _fps;
};

#endif // FLOWCONNECTION_H
