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

#include <QLayout>
#include <QPushButton>

ConnectNodeDialog::ConnectNodeDialog(const CameraInfo &camInfo, QWidget *parent) :
    QDialog(parent),
    _cameraInfo(camInfo)
{
    setupWidgets();
    resize(500, 200);

    connect(_camInfoTreeView, SIGNAL(cameraSelected(CameraInfo)), this, SLOT(selectCam(CameraInfo)));
    connect(&_refreshTimer, SIGNAL(timeout()), _camInfoTreeView, SLOT(refreshCams()));
    _refreshTimer.start(500);
}

ConnectNodeDialog::~ConnectNodeDialog()
{
}

void ConnectNodeDialog::refreshButton_clicked()
{
    _camInfoTreeView->refreshCams();
}

void ConnectNodeDialog::selectCam(CameraInfo cameraInfo)
{
    _cameraInfo.setName(cameraInfo.name());
    _cameraInfo.setAddr(cameraInfo.addr());
    done(QDialog::Accepted);
}

CameraInfo ConnectNodeDialog::cameraInfo() const
{
    return _cameraInfo;
}

void ConnectNodeDialog::buttonBox_accepted()
{
    _cameraInfo.setName(_camInfoTreeView->camInfoSelected().name());
    _cameraInfo.setAddr(_camInfoTreeView->camInfoSelected().addr());
    accept();
}

void ConnectNodeDialog::setupWidgets()
{
    QLayout *layout = new QVBoxLayout();

    QPushButton *refreshButton = new QPushButton("Refresh");
    connect(refreshButton, SIGNAL(clicked(bool)), this, SLOT(refreshButton_clicked()));
    layout->addWidget(refreshButton);

    _camInfoTreeView = new CamInfoTreeView(_cameraInfo);
    connect(_camInfoTreeView, SIGNAL(cameraSelected(CameraInfo)), this, SLOT(selectCam(CameraInfo)));
    layout->addWidget(_camInfoTreeView);

    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(dialogButtonBox);
    connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(buttonBox_accepted()));
    connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(layout);
}
