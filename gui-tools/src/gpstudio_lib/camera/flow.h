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

#ifndef FLOW_H
#define FLOW_H

#include "gpstudio_lib_common.h"

#include "property.h"

#include <QString>

class ModelFlow;

/**
 * @brief The Flow class is part of the run time model and represent a flow interface in a block.
 */
class GPSTUDIO_LIB_EXPORT Flow
{
public:
    Flow();

    QString name() const;

    enum Type {Input, Output};
    Type type() const;

    Property *assocProperty() const;

    const ModelFlow *modelFlow() const;

protected:
    void setName(const QString &name);
    void setType(const Type &type);

public:
    static Flow *fromModelFlow(const ModelFlow *modelFlow);

protected:
    QString _name;
    Type _type;

    Property *_assocProperty;

    const ModelFlow *_modelFlow;
};

#endif // FLOW_H
