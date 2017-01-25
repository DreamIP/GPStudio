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

#ifndef MODEL_IOCOM_H
#define MODEL_IOCOM_H

#include "gpstudio_lib_common.h"

#include "model_io.h"

#include "model_comdriver.h"

class GPSTUDIO_LIB_EXPORT ModelIOCom : public ModelIO
{
public:
    ModelIOCom();
    ModelIOCom(const ModelIOCom &modelIOCom);
    virtual ~ModelIOCom();

    Type type() const;

    const QString &driverIO() const;
    ModelComDriver *comDriver() const;

public:
    static ModelIO *fromNodeGenerated(const QDomElement &domElement, ModelIOCom *ioCom=NULL);
    static ModelIO *fromNodeDef(const QDomElement &domElement, ModelIO *io=NULL);

protected:
    ModelComDriver *_comDriver;
    void setComDriver(ModelComDriver *comDriver);
};

#endif // MODEL_IOCOM_H
