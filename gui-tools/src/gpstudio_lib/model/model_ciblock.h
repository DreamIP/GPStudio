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

#ifndef MODEL_CIBLOCK_H
#define MODEL_CIBLOCK_H

#include "gpstudio_lib_common.h"

#include <QString>

#include "model_block.h"

class GPSTUDIO_LIB_EXPORT ModelCIBlock : public ModelBlock
{
public:
    ModelCIBlock();
    ModelCIBlock(const ModelCIBlock &modelCIBlock);
    virtual ~ModelCIBlock();

    Type type() const;

public:
    static ModelCIBlock *fromNodeGenerated(const QDomElement &domElement, ModelCIBlock *ciBlock=NULL);
    static ModelCIBlock *fromNodeDef(const QDomElement &domElement, ModelCIBlock *ciBlock=NULL);
};

#endif // MODEL_CIBLOCK_H
