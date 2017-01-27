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

#ifndef VIEWEREXPLORERWIDGET_H
#define VIEWEREXPLORERWIDGET_H

#include "gpstudio_gui_common.h"

#include <QWidget>
#include <QTreeView>

#include "itemmodel/cameraitemmodel.h"
#include "itemmodel/viewertreeview.h"

#include <nodeeditor/gpnodeproject.h>

class GPSTUDIO_GUI_EXPORT ViewerExplorerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewerExplorerWidget(QWidget *parent = 0);

    void attachProject(GPNodeProject *project);
    GPNodeProject *project() const;

    void setCamera(Camera *camera);
    Camera *camera() const;

signals:
    void viewerSelected(QString viewerName);
    void viewerDeleted(ModelViewer*);
    void viewerFlowDeleted(ModelViewerFlow*);

private slots:
    void selectViewer(QString viewerName);
    void removeViewer(QString viewerName);
    void removeViewerFlow(QString viewerName, QString viewerFlowName);

    void updateCom();

private:
    void setupWidgets();

    QTreeView *_flowTreeView;
    CameraItemModel *_flowItemModel;

    ViewerTreeView *_viewerTreeView;

    GPNodeProject *_project;
    Camera *_camera;
};

#endif // VIEWEREXPLORERWIDGET_H
