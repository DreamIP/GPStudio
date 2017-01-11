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

#ifndef MODEL_PROCESS_H
#define MODEL_PROCESS_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QStringList>

#include "model_block.h"

class GPSTUDIO_LIB_EXPORT ModelProcess : public ModelBlock
{
public:
    ModelProcess();
    ModelProcess(const ModelProcess &modelProcess);
    virtual ~ModelProcess();

    Type type() const;

public:
    static ModelProcess *fromNodeGenerated(const QDomElement &domElement, ModelProcess *process=NULL);
    static ModelProcess *fromNodeDef(const QDomElement &domElement, ModelProcess *process=NULL);
};

#endif // MODEL_PROCESS_H
