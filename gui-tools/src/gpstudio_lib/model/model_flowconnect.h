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

#ifndef MODEL_FLOWCONNECT_H
#define MODEL_FLOWCONNECT_H

#include "gpstudio_lib_common.h"

#include <QList>
#include <QString>
#include <QDomElement>

class ModelBlock;
class ModelFIBlock;
class ModelFlow;

class GPSTUDIO_LIB_EXPORT ModelFlowConnect
{
public:
    ModelFlowConnect(QString fromblock=QString(), QString fromflow=QString(), QString toblock=QString(), QString toflow=QString(), QString order=QString());
    ModelFlowConnect(const ModelFlowConnect &other);
    virtual ~ModelFlowConnect();

    ModelBlock *fromModelBlock() const;
    const QString &fromblock() const;
    void setFromblock(const QString &fromblock);

    ModelFlow *fromModelFlow() const;
    const QString &fromflow() const;
    void setFromflow(const QString &fromflow);

    ModelBlock *toModelBlock() const;
    const QString &toblock() const;
    void setToblock(const QString &toblock);

    ModelFlow *toModelFlow() const;
    const QString &toflow() const;
    void setToflow(const QString &toflow);

    const QString &order() const;
    void setOrder(const QString &order);

    ModelFIBlock *parent() const;
    void setParent(ModelFIBlock *parent);

    bool operator ==(const ModelFlowConnect &other) const;

public:
    static ModelFlowConnect *fromNodeGenerated(const QDomElement &domElement);
    static ModelFlowConnect *fromNodeDef(const QDomElement &domElement);
    static QList<ModelFlowConnect *> listFromNodeGenerated(const QDomElement &domElement);
    static QList<ModelFlowConnect *> listFromNodeDef(const QDomElement &domElement);
    virtual QDomElement toXMLElement(QDomDocument &doc);

protected:
    QString _fromblock;
    QString _fromflow;
    QString _toblock;
    QString _toflow;
    QString _order;

    ModelFIBlock *_parent;
};

#endif // MODEL_FLOWCONNECT_H
