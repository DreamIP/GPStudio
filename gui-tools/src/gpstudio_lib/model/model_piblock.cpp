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

#include "model_piblock.h"

ModelPIBlock::ModelPIBlock()
{
    _name = "pi";
}

ModelPIBlock::ModelPIBlock(const ModelPIBlock &modelPIBlock)
    : ModelBlock(modelPIBlock)
{
}

ModelPIBlock::~ModelPIBlock()
{
}

ModelBlock::Type ModelPIBlock::type() const
{
    return PI;
}

ModelPIBlock *ModelPIBlock::fromNodeGenerated(const QDomElement &domElement, ModelPIBlock *piBlock)
{
    if(piBlock==NULL)
        piBlock = new ModelPIBlock();

    ModelBlock::fromNodeGenerated(domElement, piBlock);

    return piBlock;
}

ModelPIBlock *ModelPIBlock::fromNodeDef(const QDomElement &domElement, ModelPIBlock *piBlock)
{
    Q_UNUSED(domElement);

    if(piBlock==NULL)
        piBlock = new ModelPIBlock();

    piBlock->setName("pi");

    return piBlock;
}
