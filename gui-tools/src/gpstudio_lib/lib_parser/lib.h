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

#ifndef LIB_H
#define LIB_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QList>
#include <QMap>

#include "blocklib.h"
#include "ioboardlib.h"
#include "boardlib.h"

class GPSTUDIO_LIB_EXPORT Lib
{
    Q_DISABLE_COPY(Lib)

private:
    Lib();

public:
    ~Lib();

    void reloadProcess();
    void addCustomProcess(const QString filePath);
    void addProcess(BlockLib *process);
    const QList<BlockLib *> &processes() const;
    BlockLib *process(const QString &name);

    void reloadIos();
    void addIo(BlockLib *io);
    const QList<BlockLib *> &ios() const;
    BlockLib *io(const QString &name);

    BlockLib *blockLib(const QString &name);

    void reloadBoards();
    void addBoard(BoardLib *board);
    const QList<BoardLib *> &boards()const;
    BoardLib *board(const QString &name);

    bool addIp(const QString &fileName);

    void reloadLib();

    static inline Lib &getLib() {if(!_instance) _instance = new Lib(); return *_instance;}

protected:
    void closeProcess();
    void closeIos();
    void closeBoards();

protected:
    QList<BlockLib*> _process;
    QList<QString> _customProcess;
    QMap<QString, BlockLib*> _processMap;

    QList<BlockLib*> _ios;
    QMap<QString, BlockLib*> _iosMap;

    QList<BoardLib*> _boards;
    QMap<QString, BoardLib*> _boardsMap;

    QString _path;

    static Lib *_instance;
};

#endif // LIB_H
