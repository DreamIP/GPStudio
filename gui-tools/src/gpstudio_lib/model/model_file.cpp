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

#include "model_file.h"

#include "model_block.h"

ModelFile::ModelFile(ModelBlock *parent)
    : _parent(parent)
{
    _parent = NULL;
}

ModelFile::~ModelFile()
{
}

const QString &ModelFile::name() const
{
    return _name;
}

void ModelFile::setName(const QString &name)
{
    _name = name;
}

const QString &ModelFile::type() const
{
    return _type;
}

void ModelFile::setType(const QString &type)
{
    _type = type;
}

const QString &ModelFile::group() const
{
    return _group;
}

void ModelFile::setGroup(const QString &group)
{
    _group = group;
}

const QString &ModelFile::path() const
{
    return _path;
}

void ModelFile::setPath(const QString &path)
{
    _path = path;
}

const QString &ModelFile::description() const
{
    return _description;
}

void ModelFile::setDescription(const QString &description)
{
    _description = description;
}

ModelBlock *ModelFile::parent() const
{
    return _parent;
}

void ModelFile::setParent(ModelBlock *parent)
{
    _parent = parent;
}

ModelFile *ModelFile::fromNodeGenerated(const QDomElement &domElement)
{
    ModelFile *file = new ModelFile();

    file->setName(domElement.attribute("name","no_name"));
    file->setType(domElement.attribute("type",""));
    file->setGroup(domElement.attribute("group",""));
    file->setPath(domElement.attribute("path",""));
    file->setDescription(domElement.attribute("desc",""));

    return file;
}

QList<ModelFile *> ModelFile::listFromNodeGenerated(const QDomElement &domElement)
{
    QDomNode n = domElement.firstChild();
    QList<ModelFile *> list;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName()=="file")
                list.append(ModelFile::fromNodeGenerated(e));
        }
        n = n.nextSibling();
    }
    return list;
}
