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

#ifndef LIBITEM_H
#define LIBITEM_H

#include "gpstudio_gui_common.h"

#include <QMap>
#include <QList>
#include <QString>

class BlockLib;

class GPSTUDIO_GUI_EXPORT LibItem
{
public:
    LibItem();
    LibItem(const BlockLib *blocklib);
    LibItem(const QString &label);
    ~LibItem();

    const BlockLib *blocklib() const;
    bool isCateg() const;
    QString label() const;

    QMap<QString, LibItem *> childrensLabel() const;
    const QList<LibItem*> &childrens() const;
    LibItem *children(int i) const;
    LibItem *children(const QString &label) const;
    LibItem *path(const QString &path) const;
    LibItem *createPath(const QString &path);
    int count() const;

    LibItem *append(const BlockLib *blocklib);
    LibItem *append(const QString &label);

    void clear();

    int row();
    LibItem *parent() const;

protected:
    const BlockLib *_blocklib;
    QString _label;

    int _row;
    LibItem *_parent;

    QList<LibItem*> _childrens;
    QMap<QString, LibItem*> _childrensLabel;
};

#endif // LIBITEM_H
