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

#include "lib.h"

#include <QDebug>
#include <QDir>
#include <QCoreApplication>

Lib *Lib::_instance = NULL;

Lib::Lib()
{
    QDir dir(QCoreApplication::applicationDirPath()+"/../support");
    _path = dir.absolutePath();

    reloadLib();
}

Lib::~Lib()
{
}

QString Lib::projectPath() const
{
    return _projectPath;
}

void Lib::setProjectPath(const QString &projectPath)
{
    _projectPath = projectPath;
}

void Lib::reloadProcess()
{
    closeProcess();

    QDir dirPath(_path+"/process");

    foreach (QFileInfo pathLib, dirPath.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir dirIP(pathLib.absoluteFilePath());
        foreach (QFileInfo ipInfo, dirIP.entryInfoList(QStringList("*.proc")))
        {
            BlockLib *process = BlockLib::readFromFile(ipInfo.absoluteFilePath());
            if(process)
                addProcess(process);
        }
    }
}

void Lib::addCustomProcess(const QString filePath)
{
    QString processFilePath = filePath;
    if(!QFile::exists(filePath))
    {
        processFilePath = _projectPath + "/" + filePath;
        if(!QFile::exists(processFilePath))
            return;
    }
    _customProcess.append(processFilePath);
    BlockLib *process = BlockLib::readFromFile(processFilePath);
    if(process)
        addProcess(process);
}

void Lib::addProcess(BlockLib *process)
{
    _process.append(process);
    _processMap.insert(process->name(), process);
}

const QList<BlockLib *> &Lib::processes() const
{
    return _process;
}

BlockLib *Lib::process(const QString &name)
{
    QMap<QString, BlockLib* >::const_iterator it = _processMap.find(name);
    if(it != _processMap.end())
        return it.value();
    it = _processMap.find(QString(name).replace(".proc",""));
    if(it != _processMap.end())
        return it.value();
    return NULL;
}

void Lib::reloadIos()
{
    closeIos();

    QDir dirPath(_path+"/io");

    foreach (QFileInfo pathLib, dirPath.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir dirIP(pathLib.absoluteFilePath());
        foreach (QFileInfo ipInfo, dirIP.entryInfoList(QStringList("*.io")))
        {
            BlockLib *io = BlockLib::readFromFile(ipInfo.absoluteFilePath());
            if(io)
                addIo(io);
        }
    }
}

void Lib::addIo(BlockLib *io)
{
    _ios.append(io);
    _iosMap.insert(io->name(), io);
}

const QList<BlockLib *> &Lib::ios() const
{
    return _ios;
}

BlockLib *Lib::io(const QString &name)
{
    QMap<QString, BlockLib* >::const_iterator it = _iosMap.find(name);
    if(it != _iosMap.end())
        return it.value();
    return NULL;
}

BlockLib *Lib::blockLib(const QString &name)
{
    BlockLib *block = NULL;
    block = process(name);
    if(block)
        return block;
    return io(name);
}

void Lib::addBoard(BoardLib *board)
{
    _boards.append(board);
    _boardsMap.insert(board->name(), board);
}

const QList<BoardLib *> &Lib::boards() const
{
    return _boards;
}

BoardLib *Lib::board(const QString &name)
{
    QMap<QString, BoardLib* >::const_iterator it = _boardsMap.find(name);
    if(it != _boardsMap.end())
        return it.value();
    return NULL;
}

bool Lib::addIp(const QString &fileName)
{
    if(fileName.endsWith(".proc"))
    {
        BlockLib *process = BlockLib::readFromFile(fileName);
        if(!process)
            return false;
        if(Lib::process(process->name())!=NULL)
            return false;
        addProcess(process);
        return true;
    }
    if(fileName.endsWith(".io"))
    {
        BlockLib *io = BlockLib::readFromFile(fileName);
        if(!io)
            return false;
        if(Lib::io(io->name())!=NULL)
            return false;
        addIo(io);
        return true;
    }
    return false;
}

void Lib::reloadBoards()
{
    closeBoards();

    QDir dirPath(_path+"/board");

    foreach (QFileInfo pathLib, dirPath.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir dirIP(pathLib.absoluteFilePath());
        foreach (QFileInfo ipInfo, dirIP.entryInfoList(QStringList("*.dev")))
        {
            BoardLib *board = BoardLib::readFromFile(ipInfo.absoluteFilePath());
            if(board)
                addBoard(board);
        }
    }
}

void Lib::reloadLib()
{
    reloadBoards();
    reloadProcess();
    reloadIos();
}

void Lib::closeProcess()
{
    for(int i=0; i<_process.size(); i++)
        delete _process[i];
    _process.clear();
}

void Lib::closeIos()
{
    for(int i=0; i<_ios.size(); i++)
        delete _ios[i];
    _ios.clear();
}

void Lib::closeBoards()
{
    for(int i=0; i<_boards.size(); i++)
        delete _boards[i];
    _boards.clear();
}
