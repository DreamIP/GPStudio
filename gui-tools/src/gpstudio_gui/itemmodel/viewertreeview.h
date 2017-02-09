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

#ifndef VIEWERTREEVIEW_H
#define VIEWERTREEVIEW_H

#include "gpstudio_gui_common.h"

#include <QTreeView>
#include <QMouseEvent>

#include "cameraitemmodel.h"
#include "nodeeditor/gpnodeproject.h"

#include <QSortFilterProxyModel>

class GPSTUDIO_GUI_EXPORT ViewerTreeView : public QTreeView
{
    Q_OBJECT
public:
    ViewerTreeView();

    void attachProject(GPNodeProject *project);
    GPNodeProject *project() const;

    ModelGPViewer *gpviewer() const;
    void setGpviewer(ModelGPViewer *gpviewer);

signals:
    void viewerSelected(QString viewerName);
    void viewerFlowSelected(QString viewerName, QString viewerFlowName);
    void viewerDeleted(QString viewerName);
    void viewerFlowDeleted(QString viewerName, QString viewerFlowName);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    void keyPressEvent(QKeyEvent *event);

public slots:
    void selectViewer(QString viewerName);

private slots:
    void updateViewer();
    void updateSelection();

#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
#endif // QT_NO_CONTEXTMENU

private:
    CameraItemModel *_model;
    QSortFilterProxyModel *_modelSorted;

    GPNodeProject *_project;
    ModelGPViewer *_gpviewer;
};

#endif // VIEWERTREEVIEW_H
