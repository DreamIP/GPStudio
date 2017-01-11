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

#ifndef FLOWMANAGER_H
#define FLOWMANAGER_H

#include "gpstudio_lib_common.h"
#include "property.h"

#include <QMap>
#include <QList>

class Block;
class FlowConnection;

class GPSTUDIO_LIB_EXPORT FlowManager : public QObject
{
    Q_OBJECT
public:
    FlowManager(Camera *camera);

    Camera *camera() const;
    void setCamera(Camera *camera);

    const QMap<int, FlowConnection* > flowConnectionsMap() const;
    const QList<FlowConnection *> flowConnections() const;
    FlowConnection *flowConnection(int id) const;
    FlowConnection *flowConnection(const QString &name) const;

private:
    void addFlowConnection(FlowConnection *flowConnection);

protected slots:
    void processFlow(int idFlow);

private:
    QMap<int, FlowConnection* > _flowConnectionsMapId;
    QMap<QString, FlowConnection* > _flowConnectionsMapName;
    QList<FlowConnection* > _flowConnections;

    Block *_blockCom;
    Block *_fi;
    Camera *_camera;
};

#endif // FLOWMANAGER_H
