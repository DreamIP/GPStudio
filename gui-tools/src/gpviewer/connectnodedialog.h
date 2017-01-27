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

#ifndef CONNECTNODEDIALOG_H
#define CONNECTNODEDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QDialogButtonBox>
#include "itemmodel/caminfotreeview.h"

#include "camerainfo.h"

class ConnectNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectNodeDialog(QWidget *parent = 0);
    ~ConnectNodeDialog();

    CameraInfo cameraInfo() const;

private slots:
    void refreshButton_clicked();
    void selectCam(CameraInfo cameraInfo);

    void buttonBox_accepted();

protected:
    void setupWidgets();

    CameraInfo _cameraInfo;
    CamInfoTreeView *_camInfoTreeView;
    QTimer _refreshTimer;
};

#endif // CONNECTNODEDIALOG_H
