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

#include "caminfotreeview.h"

CamInfoTreeView::CamInfoTreeView(QWidget *parent) :
    QTreeView(parent)
{
    _model = new CamInfoItemModel();
    setModel(_model);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(cameraSelect(QModelIndex)));

    refreshCams();
}

void CamInfoTreeView::refreshCams()
{
    _model->refreshCams();
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
}

CameraInfo CamInfoTreeView::camInfoSelected() const
{
    if(!currentIndex().isValid()) return CameraInfo();
    return _model->usbList()[currentIndex().row()];
}

void CamInfoTreeView::cameraSelect(QModelIndex index)
{
    if(!index.isValid()) return;

    emit cameraSelected(_model->usbList()[index.row()]);
}
