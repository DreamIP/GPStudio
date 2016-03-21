/****************************************************************************
** Copyright (C) 2014 Dream IP
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

#ifndef MODEL_FIBLOCK_H
#define MODEL_FIBLOCK_H

#include "gpstudio_lib_common.h"

#include <QString>

#include "model_block.h"
#include "model_flowconnect.h"
#include "model_treeconnect.h"

class GPSTUDIO_LIB_EXPORT ModelFIBlock : public ModelBlock
{
public:
    ModelFIBlock();
    virtual ~ModelFIBlock();

    QString type() const;

    QList<ModelFlowConnect *> flowConnects();
    const QList<ModelFlowConnect *> flowConnects() const;
    void addFlowConnect(ModelFlowConnect *flowConnect);
    void addFlowConnects(const QList<ModelFlowConnect *> &flowConnects);

    QList<ModelTreeConnect *> treeConnects();
    const QList<ModelTreeConnect *> treeConnects() const;
    void addTreeConnect(ModelTreeConnect *treeConnect);
    void addTreeConnects(const QList<ModelTreeConnect *> &treeConnects);

public:
    static ModelFIBlock *fromNodeGenerated(const QDomElement &domElement, ModelFIBlock *fiBlock=NULL);

protected:
    QList<ModelFlowConnect *> _flowConnects;
    QList<ModelTreeConnect *> _treeConnects;
};

#endif // MODEL_FIBLOCK_H
