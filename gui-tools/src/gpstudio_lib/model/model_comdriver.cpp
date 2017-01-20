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

#include "model_comdriver.h"

#include "lib_parser/lib.h"

#include "model_comconnect.h"

ModelComDriver::ModelComDriver()
{
}

ModelComDriver::ModelComDriver(const ModelComDriver &modelComDriver)
{
    _driverIO = modelComDriver._driverIO;

    for(int i=0; i<modelComDriver._comConnects.size(); i++)
        addComConnect(modelComDriver._comConnects[i]);
}

ModelComDriver::~ModelComDriver()
{
    for(int i=0; i<_comConnects.size(); i++)
        delete _comConnects[i];
}

const QString &ModelComDriver::driverIO() const
{
    return _driverIO;
}

void ModelComDriver::setDriverIO(const QString &driverIO)
{
    _driverIO = driverIO;
}

void ModelComDriver::addComConnect(ModelComConnect *comConnect)
{
    _comConnects.append(comConnect);
}

void ModelComDriver::addComConnects(const QList<ModelComConnect *> &comConnects)
{
    foreach (ModelComConnect *comConnect, comConnects)
    {
        addComConnect(comConnect);
    }
}

QList<ModelComConnect *> &ModelComDriver::comConnects()
{
    return _comConnects;
}

const QList<ModelComConnect *> &ModelComDriver::comConnects() const
{
    return _comConnects;
}

void ModelComDriver::addComParam(ModelComParam *comParam)
{
    _comParams.append(comParam);
}

void ModelComDriver::addComParams(const QList<ModelComParam *> &comParams)
{
    foreach (ModelComParam *comParam, comParams)
    {
        addComParam(comParam);
    }
}

QList<ModelComParam *> &ModelComDriver::comParams()
{
    return _comParams;
}

const QList<ModelComParam *> &ModelComDriver::comParams() const
{
    return _comParams;
}

ModelComDriver *ModelComDriver::fromNodeGenerated(const QDomElement &domElement)
{
    ModelComDriver *modelComDriver = new ModelComDriver();

    modelComDriver->setDriverIO(domElement.attribute("driverio",""));

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="com_connects")
                modelComDriver->addComConnects(ModelComConnect::listFromNodeGenerated(e));
            if(e.tagName()=="com_params")
                modelComDriver->addComParams(ModelComParam::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return modelComDriver;
}
