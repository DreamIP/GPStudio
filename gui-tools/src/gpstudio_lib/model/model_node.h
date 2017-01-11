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

#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>
#include <QList>

#include "model_block.h"
#include "model_board.h"

class ModelFIBlock;
class ModelCIBlock;
class ModelPIBlock;
class ModelIOCom;
class ModelGPViewer;

class GPSTUDIO_LIB_EXPORT ModelNode
{
public:
    ModelNode(const QString &name=QString());
    ~ModelNode();

    const QString &name() const;
    void setName(const QString &name);

    ModelBoard *board() const;
    void setBoard(ModelBoard *board);

    ModelGPViewer *gpViewer() const;
    void setGpViewer(ModelGPViewer *gpViewer);

    bool isValid() const;
    bool isGeneratedFile() const;

    ModelBlock *getBlock(const QString &blockName) const;
    QList<ModelBlock *> &blocks();
    const QList<ModelBlock *> &blocks() const;
    void addBlock(ModelBlock *block);
    void addBlock(QList<ModelBlock *> blocks);
    void removeBlock(ModelBlock *block);
    void removeBlock(const QString &block_name);
    QStringList iosList();

    ModelFIBlock *getFIBlock() const;
    ModelCIBlock *getCIBlock() const;
    ModelPIBlock *getPIBlock() const;
    ModelIOCom *getIOCom() const;

    ModelParam *getParam(const QString &blockName, const QString &paramName) const;
    ModelProperty *getProperty(const QString &blockName, const QString &propertyName) const;
    ModelProperty *getPropertyPath(const QString &blockName, const QString &path) const;
    ModelClock *getClock(const QString &blockName, const QString &clockName) const;
    ModelFlow *getFlow(const QString &blockName, const QString &flowName) const;

public:
    static ModelNode *readFromFile(const QString &fileName);
    static ModelNode *fromNodeGenerated(const QDomElement &domElement);
    static ModelNode *fromNodeDef(const QDomElement &domElement);

    bool saveToFile(const QString &fileName);
    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    bool _valid;
    bool _generatedFile;

    ModelBoard *_board;
    ModelGPViewer *_gpViewer;

    QList<ModelBlock *> _blocks;
    QMap<QString, ModelBlock*> _blocksMap;
    friend class ModelBlock;
    void updateKeyBlock(ModelBlock *block, const QString &oldKey);
};

#endif // MODEL_NODE_H
