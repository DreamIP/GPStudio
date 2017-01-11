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

#include "hexviewer.h"

#include <QDebug>
#include <QVBoxLayout>

#include "camera/flowviewerinterface.h"

HexViewer::HexViewer(FlowViewerInterface *flowViewerInterface)
    : AbstractViewer(flowViewerInterface)
{
    setupWidgets();
    //showFlowConnection(0);
    connect((QObject*)_flowViewerInterface, SIGNAL(dataReceived(int)), this, SLOT(showFlowConnection(int)));
}

HexViewer::~HexViewer()
{
}

void HexViewer::showFlowConnection(int flowId)
{
    if(flowId>=_flowViewerInterface->flowConnections().size())
        return;

    const FlowPackage flowPackage = _flowViewerInterface->flowConnections()[flowId]->lastData();

    _widget->setData(flowPackage.data());
}

void HexViewer::setupWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _widget = new QHexEdit();
    _widget->setReadOnly(true);

    layout->addWidget(_widget);

    setLayout(layout);
}
