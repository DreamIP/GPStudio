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

#ifndef CAMERA_COM_H
#define CAMERA_COM_H

#include <QString>
#include <QThread>
#include <QList>

#include "gpstudio_com_common.h"
#include "cameraio.h"
#include "flowcom.h"

class GPSTUDIO_COM_EXPORT CameraCom : public QThread
{
    Q_OBJECT
public:
    CameraCom(const CameraInfo &cameraInfo);
    ~CameraCom();

    bool isConnected() const;
    void stop();

    const CameraInfo info() const;

    static QVector<CameraInfo> avaibleCams(const CameraInfo &info);

    FlowCom *inputFlow(unsigned char idFlow) const;
    const QList<FlowCom*> &inputFlows() const;
    QList<FlowCom*> &inputFlows();

    FlowCom *outputFlow(unsigned char idFlow) const;
    const QList<FlowCom*> &outputFlows() const;
    QList<FlowCom*> &outputFlows();

    CameraIO *cameraIO() const;

    void writeParam(const unsigned int addr, const unsigned int value);
    void writeParam(const unsigned int addr, const unsigned int * data, const unsigned size);

    void fakeDataReceived(int idFlow, const FlowPackage &package);

signals:
    void flowReadyToRead(int flow);
    void disconnected();

protected:
    void run();

private:
    CameraIO *_cameraIO;
    bool _start;

    QList<FlowCom*> _inputFlows;
    QList<FlowCom*> _outputFlows;

    FlowCom *_paramFlow;

    CameraInfo _info;
};

#endif // CAMERA_COM_H
