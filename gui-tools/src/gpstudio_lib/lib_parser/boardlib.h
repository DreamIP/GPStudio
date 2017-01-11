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

#ifndef BOARDLIB_H
#define BOARDLIB_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>
#include <QList>
#include <QMap>

#include "ioboardlib.h"
#include "ioboardlibgroup.h"

class ModelBoard;

class GPSTUDIO_LIB_EXPORT BoardLib
{
public:
    BoardLib();
    ~BoardLib();

    const QString &name() const;
    void setName(const QString &name);

    QList<IOBoardLib *> &ios();
    const QList<IOBoardLib *> &ios() const;
    void addIO(IOBoardLib *io);
    void addIOs(const QList<IOBoardLib *> &ios);
    IOBoardLib *io(const QString &name) const;
    const QMap<QString, IOBoardLibGroup> &iosGroups() const;

    ModelBoard *modelBoard() const;

public:
    static BoardLib *readFromFile(const QString &fileName);
    static BoardLib *fromDomElement(const QDomElement &domElement);

protected:
    QString _name;

    ModelBoard *_modelBoard;

    QList<IOBoardLib *> _ios;
    QMap<QString, IOBoardLib *> _iosMap;
    QMap<QString, IOBoardLibGroup> _iosGroups;
};

#endif // BOARDLIB_H
