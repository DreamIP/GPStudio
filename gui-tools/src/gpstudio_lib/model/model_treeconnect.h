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

#ifndef MODEL_TREECONNECT_H
#define MODEL_TREECONNECT_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QDomElement>

#include "model_treeitem.h"

class GPSTUDIO_LIB_EXPORT ModelTreeConnect
{
public:
    ModelTreeConnect();
    ModelTreeConnect(const ModelTreeConnect &modelTreeConnect);
    ~ModelTreeConnect();

    const QString &toblock() const;
    void setToblock(const QString &toblock);

    const QString &toflow() const;
    void setToflow(const QString &toflow);

    uint size() const;
    void setSize(const uint &size);

    const QString &order() const;
    void setOrder(const QString &order);

    const QString &muxname() const;
    void setMuxname(const QString &muxname);

    QList<ModelTreeItem *> &treeitems();
    const QList<ModelTreeItem *> &treeitems() const;
    void addTreeItem(ModelTreeItem *treeItem);
    void addTreeItems(const QList<ModelTreeItem *> &treeItems);

public:
    static ModelTreeConnect *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelTreeConnect *> listFromNodeGenerated(const QDomElement &domElement);


protected:
    QString _toblock;
    QString _toflow;
    uint _size;
    QString _order;
    QString _muxname;

    QList<ModelTreeItem *> _treeitems;
};

#endif // MODEL_TREECONNECT_H
