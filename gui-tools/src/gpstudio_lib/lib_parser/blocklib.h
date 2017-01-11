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

#ifndef PROCESSLIB_H
#define PROCESSLIB_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>
#include <QList>
#include <QIcon>

#include "model/model_process.h"
#include "model/model_io.h"
#include "model/model_iocom.h"

class GPSTUDIO_LIB_EXPORT BlockLib
{
public:
    BlockLib();
    ~BlockLib();

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

    const QIcon &icon() const;
    void setIcon(const QIcon &icon);

    ModelBlock *modelBlock() const;
    ModelProcess *modelProcess() const;
    ModelIO *modelIO() const;
    ModelIOCom *modelIOCom() const;

    enum Type
    {
        Block,
        Process,
        IO,
        IOCom
    };
    Type type() const;

public:
    static BlockLib *readFromFile(const QString &fileName);
    static BlockLib *fromDomElement(const QDomElement &domElement);

protected:
    QString _name;
    QString _categ;
    QString _path;
    QString _configFile;
    QString _description;
    QIcon _icon;
    Type _type;

    ModelBlock *_modelBlock;
};

#endif // PROCESSLIB_H
