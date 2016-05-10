/****************************************************************************
** Copyright (C) 2016 Dream IP
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

#ifndef IOLIB_H
#define IOLIB_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>
#include <QList>
#include <QIcon>

#include "model/model_io.h"

class GPSTUDIO_LIB_EXPORT IOLib
{
public:
    IOLib();
    ~IOLib();

    const QString &name() const;
    void setName(const QString &name);

    const QString &description() const;
    void setDescription(const QString &description);

    const QString &categ() const;
    void setCateg(const QString &categ);

    const QString &path() const;
    void setPath(const QString &path);

    const QString &configFile() const;
    void setConfigFile(const QString &configFile);

    const QString &draw() const;
    void setDraw(const QString &draw);

    const QIcon &icon() const;
    void setIcon(const QIcon &icon);

    ModelIO *modelIO() const;

public:
    static IOLib *readFromFile(const QString &fileName);
    static IOLib *fromDomElement(const QDomElement &domElement);

protected:
    QString _name;
    QString _categ;
    QString _path;
    QString _configFile;
    QString _description;
    QString _draw;
    QIcon _icon;

    ModelIO *_modelIO;
};

#endif // IOLIB_H
