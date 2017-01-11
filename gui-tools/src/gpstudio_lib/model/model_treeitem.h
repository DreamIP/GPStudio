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

#ifndef MODEL_TREEITEM_H
#define MODEL_TREEITEM_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QList>
#include <QDomElement>

class GPSTUDIO_LIB_EXPORT ModelTreeItem
{
public:
    ModelTreeItem();
    ~ModelTreeItem();

    const QString &fromblock() const;
    void setFromblock(const QString &fromblock);

    const QString &fromflow() const;
    void setFromflow(const QString &fromflow);

    uint size() const;
    void setSize(const uint &size);

    const QString &order() const;
    void setOrder(const QString &order);

    uint muxvalue() const;
    void setMuxvalue(const uint &muxvalue);

public:
    static ModelTreeItem *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelTreeItem *> listFromNodeGenerated(const QDomElement &domElement);

protected:
    QString _fromblock;
    QString _fromflow;
    uint _size;
    QString _order;
    uint _muxvalue;
};

#endif // MODEL_TREEITEM_H
