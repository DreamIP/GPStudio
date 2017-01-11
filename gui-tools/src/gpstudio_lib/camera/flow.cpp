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

#include "flow.h"

#include "model/model_flow.h"

#include "property.h"

Flow::Flow()
{

}

QString Flow::name() const
{
    return _name;
}

void Flow::setName(const QString &name)
{
    _name = name;
}

Flow *Flow::fromModelFlow(const ModelFlow *modelFlow)
{
    Flow *flow = new Flow();
    flow->_modelFlow = modelFlow;
    flow->setName(modelFlow->name());
    if(modelFlow->type()=="in")
        flow->setType(Input);
    else
        flow->setType(Output);

    Property *flowprop = Property::fromModelFlow(modelFlow);
    flow->_assocProperty = flowprop;

    return flow;
}

const ModelFlow *Flow::modelFlow() const
{
    return _modelFlow;
}

Flow::Type Flow::type() const
{
    return _type;
}

void Flow::setType(const Flow::Type &type)
{
    _type = type;
}

Property *Flow::assocProperty() const
{
    return _assocProperty;
}
