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

#include "flowviewerwidget.h"

#include <QBoxLayout>
#include <QStatusBar>

#include <QDebug>

#include "layerviewer.h"
#include "hexviewer.h"

FlowViewerWidget::FlowViewerWidget(const QSharedPointer<FlowViewerInterface> &flowViewerInterface) :
    QWidget(NULL),
    _viewer(NULL)
{
    setupWidgets();
    setFlowViewerInterface(flowViewerInterface);
    changeType();
}

FlowViewerWidget::FlowViewerWidget(FlowViewerInterface *flowViewerInterface) :
    QWidget(NULL),
    _viewer(NULL)
{
    setupWidgets();
    setFlowViewerInterface(QSharedPointer<FlowViewerInterface>(flowViewerInterface));
    changeType();
}

FlowViewerWidget::FlowViewerWidget(const FlowViewerWidget &other) :
    QWidget(NULL),
    _viewer(other._viewer)
{
    setupWidgets();
    setFlowViewerInterface(other._flowViewerInterface);
    changeType();
}

FlowViewerWidget::~FlowViewerWidget()
{
}

FlowViewerWidget &FlowViewerWidget::operator=(const FlowViewerWidget &other)
{
    setFlowViewerInterface(other._flowViewerInterface);
    return *this;
}

void FlowViewerWidget::changeType()
{
    FlowViewerInterface::FlowDataType dataType = _flowViewerInterface.data()->dataType();

    _typeComboBox->clear();
    QList<AbstractViewer::ViewerType> viewersTypes = AbstractViewer::viewer2Type(dataType);
    foreach (AbstractViewer::ViewerType type, viewersTypes)
    {
        _typeComboBox->addItem(AbstractViewer::iconViewerType(type), AbstractViewer::nameViewerType(type), (int)type);
    }

    if(!viewersTypes.empty())
        setupViewer(AbstractViewer::fromDataTypeName(viewersTypes.at(0), _flowViewerInterface.data()));
    else
        setupViewer(NULL);
}

void FlowViewerWidget::changeViewer()
{
    AbstractViewer::ViewerType viewerType;
    viewerType = (AbstractViewer::ViewerType)_typeComboBox->itemData(_typeComboBox->currentIndex()).toInt();
    setupViewer(AbstractViewer::fromDataTypeName(viewerType, _flowViewerInterface.data()));
    setWindowIcon(AbstractViewer::iconViewerType(viewerType));
    dataReceive();
}

void FlowViewerWidget::dataReceive()
{
    if(_statusLabel)
    {
        _statusLabel->setText(_flowViewerInterface.data()->statusText());
    }
}

void FlowViewerWidget::setupWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    AbstractViewer *viewer = new AbstractViewer(new FlowViewerInterface);
    layout->addWidget(viewer);

    // status
    _statusLayout = new QHBoxLayout();
    _statusLayout->setContentsMargins(0,0,0,0);

    _statusLabel = new QLabel("");
    _statusLayout->addWidget(_statusLabel);

    _typeComboBox = new QComboBox();
    _statusLayout->addWidget(_typeComboBox);
    connect(_typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeViewer()));

    layout->addItem(_statusLayout);

    setLayout(layout);
}

void FlowViewerWidget::setupViewer(AbstractViewer *viewer)
{
    if(layout())
    {
        QLayoutItem *child;
        while ((child = layout()->takeAt(0)) != 0)
        {
            if(child->widget())
            {
                child->widget()->hide();
                child->widget()->deleteLater();
            }

            // don't delete _statusLayout
            if(child->layout()!=_statusLayout) delete child;
        }
        delete layout();
    }
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    if(viewer==NULL)
        viewer = new AbstractViewer(new FlowViewerInterface);

    layout->addWidget(viewer);
    _viewer = viewer;

    layout->addItem(_statusLayout);

    setLayout(layout);
}

void FlowViewerWidget::setFlowViewerInterface(const QSharedPointer<FlowViewerInterface> &flowViewerInterface)
{
    _flowViewerInterface = flowViewerInterface;
    connect(_flowViewerInterface.data(), SIGNAL(dataTypeChanged()), this, SLOT(changeType()));
    connect(_flowViewerInterface.data(), SIGNAL(dataReceived(int)), this, SLOT(dataReceive()));
}

//QMutexLocker _viewerLock;
