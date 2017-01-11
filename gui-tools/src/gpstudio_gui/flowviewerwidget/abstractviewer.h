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

#ifndef ABSTRACTVIEWER_H
#define ABSTRACTVIEWER_H

#include "gpstudio_gui_common.h"

#include <QWidget>
#include <QToolButton>

#include "camera/property.h"

#include "camera/flowviewerinterface.h"

class GPSTUDIO_GUI_EXPORT AbstractViewer : public QWidget
{
    Q_OBJECT
public:
    AbstractViewer(FlowViewerInterface *flowViewerInterface);
    virtual ~AbstractViewer();

    enum ViewerType {
        LayerViewerType,
        HewViewerType,
        PlotsViewerType
    };

    const Property &properties() const;

public:
    static QList<AbstractViewer::ViewerType> viewer2Type(FlowViewerInterface::FlowDataType dataType);

    static QString nameViewerType(AbstractViewer::ViewerType viewerType);
    static QIcon iconViewerType(AbstractViewer::ViewerType viewerType);
    static AbstractViewer *fromDataTypeName(AbstractViewer::ViewerType viewerType, FlowViewerInterface *flowViewerInterface);

protected:
    virtual void setupWidgets();

protected slots:
    virtual void showFlowConnection(int flowId);

protected:
    Property _properties;

    FlowViewerInterface *_flowViewerInterface;
};

#endif // ABSTRACTVIEWER_H
