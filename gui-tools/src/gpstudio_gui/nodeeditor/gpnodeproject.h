/****************************************************************************
** Copyright (C) 2016 Dream IP
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

#ifndef GPNODEPROJECT_H
#define GPNODEPROJECT_H

#include "gpstudio_gui_common.h"

#include "model/model_node.h"

class GPSTUDIO_GUI_EXPORT GPNodeProject : public QObject
{
    Q_OBJECT
public:
    GPNodeProject(QObject *parent=NULL);
    ~GPNodeProject();

    QString name() const;
    QString path() const;
    bool isModified() const;
    ModelNode *node() const;

public slots:
    void newProject();
    bool openProject(const QString &nodeFileName=QString());
    bool saveProject();
    bool saveProjectAs(const QString &nodeFileName=QString());
    void closeProject();

signals:
    void nodeChanged();
    void nodePathChanged(QString path);
    void nodeModified(bool modified);

private:
    void setPath(const QString &path);
    QString _path;

    void setNode(ModelNode *node);
    ModelNode *_node;

    void setModified(bool modified);
    bool _modified;
};

#endif // GPNODEPROJECT_H
