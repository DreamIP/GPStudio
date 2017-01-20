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

#ifndef MODEL_COMDRIVER_H
#define MODEL_COMDRIVER_H

#include "gpstudio_lib_common.h"

#include "model_comconnect.h"
#include "model_comparam.h"

class GPSTUDIO_LIB_EXPORT ModelComDriver
{
public:
    ModelComDriver();
    ModelComDriver(const ModelComDriver &modelComDriver);
    ~ModelComDriver();

    const QString &driverIO() const;
    void setDriverIO(const QString &driverIO);

    void addComConnect(ModelComConnect *comConnect);
    void addComConnects(const QList<ModelComConnect *> &comConnects);
    QList<ModelComConnect *> &comConnects();
    const QList<ModelComConnect *> &comConnects() const;

    void addComParam(ModelComParam *comParam);
    void addComParams(const QList<ModelComParam *> &comParams);
    QList<ModelComParam *> &comParams();
    const QList<ModelComParam *> &comParams() const;

public:
    static ModelComDriver *fromNodeGenerated(const QDomElement &domElement);

protected:
    QString _driverIO;

    QList<ModelComConnect *> _comConnects;
    QList<ModelComParam *> _comParams;
};

#endif // MODEL_COMDRIVER_H
