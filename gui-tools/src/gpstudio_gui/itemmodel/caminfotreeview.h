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

#ifndef CAMINFOTREEVIEW_H
#define CAMINFOTREEVIEW_H

#include "gpstudio_gui_common.h"

#include <QTreeView>
#include "caminfoitemmodel.h"

class GPSTUDIO_GUI_EXPORT CamInfoTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CamInfoTreeView(QWidget *parent = 0);

    CameraInfo camInfoSelected() const;

signals:
    void cameraSelected(CameraInfo cameraInfo);

public slots:
    void refreshCams();

private slots:
    void cameraSelect(QModelIndex index);

private:
    CamInfoItemModel *_model;
};

#endif // CAMINFOTREEVIEW_H
