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

#ifndef LIBTREEVIEW_H
#define LIBTREEVIEW_H

#include "gpstudio_gui_common.h"

#include <QMouseEvent>
#include <QTreeView>

#include "libitemmodel.h"
#include "nodeeditor/gpnodeproject.h"

class GPSTUDIO_GUI_EXPORT LibTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit LibTreeView(QWidget *parent = 0);

    void attachProject(GPNodeProject *project);

    void setLib(const Lib *lib);

signals:
    void blockAdded(const QString driver, const QPoint pos);

protected slots:
    void doubleClickProcess(QModelIndex index);

protected:
    void startDrag(Qt::DropActions supportedActions);
    void keyPressEvent(QKeyEvent *event);

#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
#endif // QT_NO_CONTEXTMENU

private:
    GPNodeProject *_project;

    LibItemModel *_model;
};

#endif // LIBTREEVIEW_H
