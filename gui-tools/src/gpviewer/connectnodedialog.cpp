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

#include "connectnodedialog.h"
#include "ui_connectnodedialog.h"

ConnectNodeDialog::ConnectNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectNodeDialog)
{
    ui->setupUi(this);
    _cameraInfo = CameraInfo();
    connect(ui->camTreeView, SIGNAL(cameraSelected(CameraInfo)), this, SLOT(selectCam(CameraInfo)));
    connect(&_refreshTimer, SIGNAL(timeout()), ui->camTreeView, SLOT(refreshCams()));
    _refreshTimer.start(500);
}

ConnectNodeDialog::~ConnectNodeDialog()
{
    delete ui;
}

void ConnectNodeDialog::on_refreshButton_clicked()
{
    ui->camTreeView->refreshCams();
}

void ConnectNodeDialog::selectCam(CameraInfo cameraInfo)
{
    _cameraInfo = cameraInfo;
    done(QDialog::Accepted);
}

CameraInfo ConnectNodeDialog::cameraInfo() const
{
    return _cameraInfo;
}

void ConnectNodeDialog::on_buttonBox_accepted()
{
    _cameraInfo = ui->camTreeView->camInfoSelected();
}
