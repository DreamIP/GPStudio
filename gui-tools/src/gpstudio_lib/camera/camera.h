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

#ifndef CAMERA_H
#define CAMERA_H

#include "gpstudio_lib_common.h"

#include <QObject>
#include <QMap>

#include "block.h"
#include "flow.h"
#include "property.h"

#include "scriptengine/scriptengine.h"

#include "model/model_node.h"
#include "registermanager.h"

class CameraCom;
class FlowManager;
class FlowConnection;
class FlowPackage;

#include "camerainfo.h"

/**
 * @brief The Camera class is part of the run time model and represent the main camera.
 * Camera could be created from a node_generated.xml file. After that you can connect to a real camera with connectCam method.
 */
class GPSTUDIO_LIB_EXPORT Camera : public QObject
{
    Q_OBJECT
public:
    Camera(const QString &fileCameraConfig=QString());
    ~Camera();

    bool loadFromFile(const QString &fileCameraConfig);

    const ModelNode *node() const;
    void setNode(ModelNode *node);

    const Property &rootProperty() const;
    Property &rootProperty();

    void connectCam(const CameraInfo &cameraInfo=CameraInfo());
    CameraInfo cameraInfo() const;
    bool isConnected() const;

    CameraCom *com() const;
    FlowManager *flowManager() const;
    RegisterManager &registers();
    QByteArray registerData() const;

    // blocks access
    void addBlock(Block *block);
    void addBlock(ModelBlock *modelBlock);
    void removeBlock(Block *block);
    void removeBlock(ModelBlock *modelBlock);
    const QList<Block *> &blocks() const;
    Block *block(QString name) const;
    Block *block(int i) const;

    // special blocks access
    Block *comBlock() const;
    Block *fiBlock() const;

    RegisterManager &registermanager();

    void sendPackage(int idFlow, const FlowPackage &package);
    void sendPackage(const QString &flowName, const FlowPackage &package);

signals:
    void registerDataChanged();

public slots:
    uint evalPropertyMap(const QString &propertyMap);
    void setRegister(uint addr, uint value);

protected:
    ModelNode *_modelNode;

    Property _paramsBlocks;
    RegisterManager _registermanager;
    FlowManager *_flowManager;

    CameraCom *_com;

    // blocks conntainers
    QList<Block*> _blocks;
    QMap<QString, Block*> _blocksMap;
    friend class Block;
    void updateKeyBlock(Block *block, const QString &oldKey);

    // special blocks
    Block *_comBlock;
    Block *_fiBlock;
};

#endif // CAMERA_H
