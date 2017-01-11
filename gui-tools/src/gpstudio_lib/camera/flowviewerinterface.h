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

#ifndef FLOWVIEWERINTERFACE_H
#define FLOWVIEWERINTERFACE_H

#include "gpstudio_lib_common.h"

#include <QObject>
#include <QList>

#include "camera/flowconnection.h"

class GPSTUDIO_LIB_EXPORT FlowViewerInterface : public QObject
{
    Q_OBJECT
public:
    explicit FlowViewerInterface(FlowConnection *flowConnection);
    FlowViewerInterface(const QList<FlowConnection *> &flowConnections=QList<FlowConnection *>());
    ~FlowViewerInterface();

    // flowConnections access and management
    const QList<FlowConnection *> &flowConnections() const;
    void addFlowConnection(FlowConnection *flowConnection);
    void removeFlowConnection(FlowConnection *flowConnection);
    void moveFlowConnection(FlowConnection *flowConnection, int index);

    enum FlowDataType {
        ImageFlowType,
        VectorFlowType,
        UnknowFlowType
    };

    FlowDataType dataType() const;
    QString statusText() const;

signals:
    void dataTypeChanged();
    void dataReceived(int index);

protected slots:
    void changeDataType();
    void processData(FlowPackage data);

protected:
    FlowConnection *objectToFlowConnection(QObject *object);

private:
    QList<FlowConnection *> _flowConnections;
    FlowDataType _dataType;
};

#endif // FLOWVIEWERINTERFACE_H
