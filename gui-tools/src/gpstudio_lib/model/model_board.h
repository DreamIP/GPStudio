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

#ifndef MODEL_BOARD_H
#define MODEL_BOARD_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>
#include <QList>

class ModelNode;
class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelBoard
{
public:
    ModelBoard();
    ~ModelBoard();

    const QString &name() const;
    void setName(const QString &name);

    ModelNode *parent() const;
    void setParent(ModelNode *parent);

public:
    static ModelBoard *fromNodeGenerated(const QDomElement &domElement);
    static ModelBoard *fromNodeDef(const QDomElement &domElement);
    static QList<ModelBlock *> listIosFromNodeDef(const QDomElement &domElement);
    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;

    ModelNode *_parent;
};

#endif // MODEL_BOARD_H
