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

#include "model_ciblock.h"

ModelCIBlock::ModelCIBlock()
{
    _name = "ci";
}

ModelCIBlock::ModelCIBlock(const ModelCIBlock &modelCIBlock)
    : ModelBlock(modelCIBlock)
{
}

ModelCIBlock::~ModelCIBlock()
{
}

ModelBlock::Type ModelCIBlock::type() const
{
    return CI;
}

ModelCIBlock *ModelCIBlock::fromNodeGenerated(const QDomElement &domElement, ModelCIBlock *ciBlock)
{
    if(ciBlock==NULL)
        ciBlock = new ModelCIBlock();

    ModelBlock::fromNodeGenerated(domElement, ciBlock);

    return ciBlock;
}

ModelCIBlock *ModelCIBlock::fromNodeDef(const QDomElement &domElement, ModelCIBlock *ciBlock)
{
    Q_UNUSED(domElement);

    if(ciBlock==NULL)
        ciBlock = new ModelCIBlock();

    ciBlock->setName("ci");

    return ciBlock;
}
