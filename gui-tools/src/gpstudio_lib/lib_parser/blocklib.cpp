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

#include "blocklib.h"

#include <QFile>
#include <QFileInfo>
#include <QSvgRenderer>
#include <QDebug>
#include <QPainter>

BlockLib::BlockLib()
{
    _modelBlock = NULL;
}

BlockLib::~BlockLib()
{
    delete _modelBlock;
}

const QString &BlockLib::name() const
{
    return _name;
}

void BlockLib::setName(const QString &name)
{
    _name = name;
}

const QString &BlockLib::description() const
{
    return _description;
}

void BlockLib::setDescription(const QString &description)
{
    _description = description;
}

const QString &BlockLib::categ() const
{
    return _categ;
}

void BlockLib::setCateg(const QString &categ)
{
    _categ = categ;
}

const QString &BlockLib::path() const
{
    return _path;
}

void BlockLib::setPath(const QString &path)
{
    _path = path;
    if(_modelBlock)
        _modelBlock->setPath(path);
}

const QString &BlockLib::configFile() const
{
    return _configFile;
}

void BlockLib::setConfigFile(const QString &configFile)
{
    _configFile = configFile;
}

const QIcon &BlockLib::icon() const
{
    return _icon;
}

void BlockLib::setIcon(const QIcon &icon)
{
    _icon = icon;
}

BlockLib *BlockLib::readFromFile(const QString &fileName)
{
    QDomDocument doc;
    QFile file(fileName);
    BlockLib *blockLib = NULL;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug()<<"Cannot open"<<file.fileName();
    else
    {
        if(!doc.setContent(&file))
            qDebug()<<"Cannot open"<<file.fileName();
        else
        {
            blockLib = BlockLib::fromDomElement(doc.documentElement());
            if(blockLib)
            {
                blockLib->setPath(QFileInfo(file).absolutePath());
            }
        }
        file.close();
    }
    return blockLib;
}

BlockLib *BlockLib::fromDomElement(const QDomElement &domElement)
{
    BlockLib *blockLib = new BlockLib();
    if(domElement.tagName()=="process")
    {
        blockLib->_type = Process;
        blockLib->setName(domElement.attribute("name","no_name"));
    }
    else
    {
        if(domElement.attribute("categ","")=="communication")
            blockLib->_type = IOCom;
        else
            blockLib->_type = IO;
        blockLib->setName(domElement.attribute("driver","no_name"));
    }

    blockLib->setCateg(domElement.attribute("categ",""));
    blockLib->setDescription(domElement.attribute("desc",""));

    // get svg part of file and save as string
    QString svg;
    const QDomNodeList &nodesSvg = domElement.elementsByTagName("svg");
    if(nodesSvg.size()>0)
    {
        QTextStream streamSvg(&svg);
        streamSvg << nodesSvg.at(0);
    }

    // render an 32px icon from svg
    QSvgRenderer render;
    QPixmap pixIcon(32, 32);
    QPainter painter(&pixIcon);
    painter.setBrush(Qt::white);
    painter.drawRect(pixIcon.rect());
    render.load(svg.toUtf8());
    render.render(&painter, pixIcon.rect());
    painter.end();
    blockLib->_icon.addPixmap(pixIcon);

    // get process model for creating instance of ModelProcess
    if(domElement.tagName()=="process")
        blockLib->_modelBlock = ModelProcess::fromNodeGenerated(domElement);
    else if(domElement.tagName()=="iocom" || domElement.attribute("categ") == "communication")
        blockLib->_modelBlock = ModelIOCom::fromNodeGenerated(domElement);
    else if(domElement.tagName()=="io")
        blockLib->_modelBlock = ModelIO::fromNodeGenerated(domElement);

    blockLib->_modelBlock->setInLib(true);
    blockLib->_modelBlock->setDriver(blockLib->_name); // TODO fixe me

    return blockLib;
}

ModelBlock *BlockLib::modelBlock() const
{
    return _modelBlock;
}

ModelProcess *BlockLib::modelProcess() const
{
    return static_cast<ModelProcess*>(_modelBlock);
}

ModelIO *BlockLib::modelIO() const
{
    return static_cast<ModelIO*>(_modelBlock);
}

ModelIOCom *BlockLib::modelIOCom() const
{
    return static_cast<ModelIOCom*>(_modelBlock);
}

BlockLib::Type BlockLib::type() const
{
    return _type;
}
