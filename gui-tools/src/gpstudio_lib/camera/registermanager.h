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

#ifndef REGISTERSMAP_H
#define REGISTERSMAP_H

#include "gpstudio_lib_common.h"

#include <QMap>

#include "register.h"
#include "model/model_node.h"

class Register;
class Camera;

class GPSTUDIO_LIB_EXPORT RegisterManager
{
public:
    RegisterManager(Camera *camera);
    ~RegisterManager();

    void setNode(ModelNode *node);

    Register *operator[](const uint addr);
    const QMap<uint, Register *> &registersMap() const;

    QByteArray registerData() const;

    void evalAll();

    void start();

    void setRegister(uint addr, uint value);

private:
    void addRegister(Register *cameraRegister);
    QMap<uint, Register *> _registersMap;

    QMap<uint, uint> _regToSend;
    QByteArray _registerData;

    Camera *_camera;
};

#endif // REGISTERSMAP_H
