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

#include "model_iocom.h"

#include "lib_parser/lib.h"

#include "model_comconnect.h"

ModelIOCom::ModelIOCom()
{
}

ModelIOCom::ModelIOCom(const ModelIOCom &modelIOCom)
    : ModelIO(modelIOCom)
{
    _driverIO = modelIOCom._driverIO;

    for(int i=0; i<modelIOCom._comConnects.size(); i++)
        addComConnect(modelIOCom._comConnects[i]);
}

ModelIOCom::~ModelIOCom()
{
    for(int i=0; i<_comConnects.size(); i++)
        delete _comConnects[i];
}

ModelBlock::Type ModelIOCom::type() const
{
    return IOCom;
}

const QString &ModelIOCom::driverIO() const
{
    return _driverIO;
}

void ModelIOCom::setDriverIO(const QString &driverIO)
{
    _driverIO = driverIO;
}

void ModelIOCom::addComConnect(ModelComConnect *comConnect)
{
    _comConnects.append(comConnect);
}

void ModelIOCom::addComConnects(const QList<ModelComConnect *> &comConnects)
{
    foreach (ModelComConnect *comConnect, comConnects)
    {
        addComConnect(comConnect);
    }
}

QList<ModelComConnect *> &ModelIOCom::comConnects()
{
    return _comConnects;
}

const QList<ModelComConnect *> &ModelIOCom::comConnects() const
{
    return _comConnects;
}

ModelIO *ModelIOCom::fromNodeGenerated(const QDomElement &domElement, ModelIOCom *ioCom)
{
    if(ioCom==NULL)
        ioCom = new ModelIOCom();

    ioCom->setDriverIO(domElement.attribute("driverio",""));

    ModelIO::fromNodeGenerated(domElement, ioCom);

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="com_connects")
                ioCom->addComConnects(ModelComConnect::listFromNodeGenerated(e));
        }
        n = n.nextSibling();
    }

    return ioCom;
}

ModelIO *ModelIOCom::fromNodeDef(const QDomElement &domElement, ModelIO *io)
{
    QString driver = domElement.attribute("driver","");

    BlockLib *ioLib = Lib::getLib().io(driver);
    if(ioLib)
        io = new ModelIOCom(*ioLib->modelIOCom());

    if(io==NULL)
        io = new ModelIOCom();

    ModelBlock::fromNodeDef(domElement, io);

    return io;
}

