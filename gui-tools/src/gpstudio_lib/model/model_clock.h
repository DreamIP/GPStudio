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

#ifndef MODEL_CLOCK_H
#define MODEL_CLOCK_H

#include "gpstudio_lib_common.h"

#include <QString>
#include <QDomElement>

class ModelBlock;

class GPSTUDIO_LIB_EXPORT ModelClock
{
public:
    ModelClock(ModelBlock *parent=NULL);
    ~ModelClock();

    const QString &name() const;
    void setName(const QString &name);

    const QString &group() const;
    void setGroup(const QString &group);

    qint16 shift() const;
    void setShift(const qint16 &shift);

    qint32 min() const;
    void setMin(const qint32 &min);

    qint32 max() const;
    void setMax(const qint32 &max);

    qint32 typical() const;
    void setTypical(const qint32 &typical);

    enum Direction {In, Out};
    Direction direction() const;
    void setDirection(const Direction &direction);

    const QString &description() const;
    void setDescription(const QString &description);

    ModelBlock *parent() const;
    void setParent(ModelBlock *parent);

public:
    static ModelClock *fromNodeGenerated(const QDomElement &domElement);
    static QList<ModelClock *> listFromNodeGenerated(const QDomElement &domElement);
    QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _name;
    QString _group;
    qint16 _shift;
    qint32 _min;
    qint32 _max;
    qint32 _typical;
    Direction _direction;
    QString _description;

    ModelBlock *_parent;
};

#endif // MODEL_CLOCK_H
