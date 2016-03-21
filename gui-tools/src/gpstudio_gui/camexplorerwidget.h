/****************************************************************************
** Copyright (C) 2014 Dream IP
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

#ifndef CAMEXPLORERWIDGET_H
#define CAMEXPLORERWIDGET_H

#include "gpstudio_gui_common.h"

#include <QWidget>

#include "propertyitemmodel.h"
#include "cameraitemmodel.h"
#include <QTreeView>

class GPSTUDIO_GUI_EXPORT CamExplorerWidget : public QWidget
{
    Q_OBJECT
public:
    CamExplorerWidget(QWidget *parent=0);

    Camera *camera() const;
    void setCamera(Camera *camera);

protected slots:
    void updateRootProperty(QModelIndex index);

private:
    QTreeView *_camTreeView;
    CameraItemModel *_camItemModel;

    QTreeView *_propertyTreeView;
    PropertyItemModel *_propertyItemModel;

    Camera *_camera;
};

#endif // CAMEXPLORERWIDGET_H
