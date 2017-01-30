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

#include "layerviewer.h"

#include <QDebug>
#include <QVBoxLayout>

#include <QFileDialog>
#include <QFileInfo>
#include <QDateTime>

#include "camera/flowviewerinterface.h"

LayerViewer::LayerViewer(FlowViewerInterface *flowViewerInterface)
    : AbstractViewer(flowViewerInterface)
{
    setupWidgets();
    //showFlowConnection(0);
    connect((QObject*)_flowViewerInterface, SIGNAL(dataReceived(int)), this, SLOT(showFlowConnection(int)));

    foreach (FlowConnection *flowConnection, flowViewerInterface->flowConnections())
    {
        Property *flowProp = flowConnection->flow()->assocProperty();
        if(flowProp)
        {
            QSize size(flowProp->property("width").toInt(), flowProp->property("height").toInt());
            if(size.isValid())
            {
                _widget->setRectSize(size);
                break;
            }
        }
    }
}

LayerViewer::~LayerViewer()
{
}

void LayerViewer::showFlowConnection(int flowId)
{
    if(flowId>=_flowViewerInterface->flowConnections().size())
        return;

    if(_pauseButton->isChecked())
        return;

    const FlowPackage flowPackage = _flowViewerInterface->flowConnections()[flowId]->lastData();
    Property *flowProp = _flowViewerInterface->flowConnections()[flowId]->flow()->assocProperty();

    QString datatype = flowProp->property("datatype").toString();
    if(datatype == "image")
    {
        int width = flowProp->property("width").toInt();
        int height = flowProp->property("height").toInt();

        // check for masked mode
        if(_flowViewerInterface->flowConnections()[flowId]->flow()->assocProperty()->property("colormode").toString()=="bin"
                && _flowViewerInterface->flowConnections().count()>1)
        {
            QImage image = flowPackage.toImage(width, height, 8);
            _widget->setMask(image);
            return;
        }
        if(width!=0 && height!=0)
        {
            QImage image = flowPackage.toImage(width, height, 8);
            _widget->showImage(image);
            _widget->setRectSize(image.size());
        }
    }
    if(datatype == "features")
    {
        const QList<Feature> &features = Feature::fromData(flowPackage, flowProp);
        _widget->setFeatures(flowId, features);
    }

    if(!_recordPath.isEmpty() && _recordButton->isChecked())
    {
        QImage image(_widget->scene()->sceneRect().size().toSize(), QImage::Format_RGB32);
        QPainter painter(&image);
        _widget->scene()->render(&painter);
        image.save(QString("%1/%2_%3.jpg")
                   .arg(_recordPath)
                   .arg(_flowViewerInterface->flowConnections()[flowId]->flow()->name())
                   .arg(QDateTime::currentDateTime().toString("yy-MM-dd_hh.mm.ss.zzz")));
    }
}

void LayerViewer::saveImage()
{
    QImage image(_widget->scene()->sceneRect().size().toSize(), QImage::Format_RGB32);
    QPainter painter(&image);
    _widget->scene()->render(&painter);

    QString fileName = QFileDialog::getSaveFileName(this, "Save image...", "", "Images (*.png *.bmp *.jpg)");
    if(!fileName.isEmpty())
    {
        QFileInfo info(fileName);
        if(info.completeSuffix()=="")
            fileName.append(".jpg");
        image.save(fileName);
    }
}

void LayerViewer::recordImages()
{
    if(_recordButton->isChecked())
        _recordPath = QFileDialog::getExistingDirectory(this, "Directory to save images...", "");
}

void LayerViewer::setupWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    _widget = new LayerWidget();

    layout->addItem(getToolBar());

    layout->addWidget(_widget);

    setLayout(layout);
}

QLayout *LayerViewer::getToolBar()
{
    QVBoxLayout *layoutTools = new QVBoxLayout();
    layoutTools->setContentsMargins(0,5,2,0);
    layoutTools->setSpacing(2);

    // viewer actions (pause, grab image, video...)
    _pauseButton = new QToolButton();
    _pauseButton->setToolTip("Pause viewer");
    _pauseButton->setAutoRaise(true);
    _pauseButton->setCheckable(true);
    _pauseButton->setIcon(QIcon(":/icons/img/pause.png"));
    layoutTools->addWidget(_pauseButton);

    _saveButton = new QToolButton();
    _saveButton->setToolTip("Save image");
    _saveButton->setAutoRaise(true);
    _saveButton->setIcon(QIcon(":/icons/img/save.png"));
    connect(_saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
    layoutTools->addWidget(_saveButton);

    _recordButton = new QToolButton();
    _recordButton->setToolTip("Records images");
    _recordButton->setAutoRaise(true);
    _recordButton->setCheckable(true);
    _recordButton->setIcon(QIcon(":/icons/img/record.png"));
    connect(_recordButton, SIGNAL(clicked(bool)), this, SLOT(recordImages()));
    layoutTools->addWidget(_recordButton);

    _settingsButton = new QToolButton();
    _settingsButton->setToolTip("Records images");
    _settingsButton->setAutoRaise(true);
    _settingsButton->setIcon(QIcon(":/icons/img/settings.png"));
    layoutTools->addWidget(_settingsButton);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layoutTools->addWidget(line);

    // viewer zoom option
    _zoomFitButton = new QToolButton();
    _zoomFitButton->setToolTip("Zoom fit best");
    _zoomFitButton->setAutoRaise(true);
    _zoomFitButton->setIcon(QIcon(":/icons/img/zoom-fit.png"));
    connect(_zoomFitButton, SIGNAL(clicked(bool)), _widget, SLOT(zoomFit()));
    layoutTools->addWidget(_zoomFitButton);

    _zoomOutButton = new QToolButton();
    _zoomOutButton->setToolTip("Zoom out");
    _zoomOutButton->setAutoRaise(true);
    _zoomOutButton->setIcon(QIcon(":/icons/img/zoom-out.png"));
    connect(_zoomOutButton, SIGNAL(clicked(bool)), _widget, SLOT(zoomOut()));
    layoutTools->addWidget(_zoomOutButton);

    _zoomInButton = new QToolButton();
    _zoomInButton->setToolTip("Zoom in");
    _zoomInButton->setAutoRaise(true);
    _zoomInButton->setIcon(QIcon(":/icons/img/zoom-in.png"));
    connect(_zoomInButton, SIGNAL(clicked(bool)), _widget, SLOT(zoomIn()));
    layoutTools->addWidget(_zoomInButton);

    layoutTools->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    return layoutTools;
}
