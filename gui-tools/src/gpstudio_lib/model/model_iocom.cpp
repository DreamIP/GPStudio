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
    _comDriver = NULL;
}

ModelIOCom::ModelIOCom(const ModelIOCom &modelIOCom)
    : ModelIO(modelIOCom)
{
    _comDriver = new ModelComDriver(*modelIOCom._comDriver);
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

QString ModelIOCom::driverIO() const
{
    if(_comDriver)
        return _comDriver->driverIO();
    return QString();
}

ModelComDriver *ModelIOCom::comDriver() const
{
    return _comDriver;
}

ModelIO *ModelIOCom::fromNodeGenerated(const QDomElement &domElement, ModelIOCom *ioCom)
{
    if(ioCom==NULL)
        ioCom = new ModelIOCom();

    ModelIO::fromNodeGenerated(domElement, ioCom);

    QDomNode n = domElement.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="com_driver")
                ioCom->_comDriver = ModelComDriver::fromNodeGenerated(e);
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
