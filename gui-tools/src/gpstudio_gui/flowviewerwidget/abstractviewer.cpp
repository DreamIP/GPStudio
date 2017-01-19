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

#include "abstractviewer.h"

#include "layerviewer.h"
#include "hexviewer.h"
#include "plotviewer.h"

#include <QDebug>

AbstractViewer::AbstractViewer(FlowViewerInterface *flowViewerInterface)
    : _flowViewerInterface(flowViewerInterface)
{
}

AbstractViewer::~AbstractViewer()
{
}

void AbstractViewer::setupWidgets()
{
}

void AbstractViewer::showFlowConnection(int flowId)
{
    Q_UNUSED(flowId)
}

const Property &AbstractViewer::properties() const
{
    return _properties;
}

AbstractViewer *AbstractViewer::fromDataTypeName(ViewerType viewerType, FlowViewerInterface *flowViewerInterface)
{
    switch (viewerType)
    {
    case AbstractViewer::LayerViewerType:
        return new LayerViewer(flowViewerInterface);
    case AbstractViewer::HewViewerType:
        return new HexViewer(flowViewerInterface);
    case AbstractViewer::PlotsViewerType:
        return new PlotViewer(flowViewerInterface);
    default:
        return NULL;
    }
}

QList<AbstractViewer::ViewerType> AbstractViewer::viewer2Type(FlowViewerInterface::FlowDataType dataType)
{
    QList<AbstractViewer::ViewerType> list;

    switch (dataType)
    {
    case FlowViewerInterface::ImageFlowType:
    case FlowViewerInterface::FeaturesType:
        list << LayerViewerType;
        list << HewViewerType;
        break;
    case FlowViewerInterface::VectorFlowType:
        list << PlotsViewerType;
        list << HewViewerType;
        break;
    case FlowViewerInterface::UnknowFlowType:
    default:
        break;
    }

    return list;
}

QString AbstractViewer::nameViewerType(AbstractViewer::ViewerType viewerType)
{
    switch (viewerType)
    {
    case AbstractViewer::LayerViewerType:
        return QString("Layer Viewer");
    case AbstractViewer::HewViewerType:
        return QString("Hex Viewer");
    case AbstractViewer::PlotsViewerType:
        return QString("Plots Viewer");
    default:
        return QString("Unknow");
    }
}

QIcon AbstractViewer::iconViewerType(AbstractViewer::ViewerType viewerType)
{
    switch (viewerType)
    {
    case AbstractViewer::LayerViewerType:
        return QIcon(":/icons/img/layers.png");
    case AbstractViewer::HewViewerType:
        return QIcon(":/icons/img/hex.png");
    case AbstractViewer::PlotsViewerType:
        return QIcon(":/icons/img/charts.png");
    default:
        return QIcon();
    }
}
