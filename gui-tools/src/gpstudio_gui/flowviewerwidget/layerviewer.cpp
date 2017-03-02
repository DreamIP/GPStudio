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
        _widget->scene()->render(&painter, _widget->rectSize());
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
    _widget->scene()->render(&painter, _widget->rectSize());

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
    {
        QString newPath = QFileDialog::getExistingDirectory(this, "Directory to save images...", "");
        if(!newPath.isEmpty())
            _recordPath = newPath;
        else
            _recordButton->setChecked(false);
    }
}

void LayerViewer::setupWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    _widget = new LayerWidget();
    layout->addWidget(getToolBar());
    layout->addWidget(_widget);

    setLayout(layout);
}

QToolBar *LayerViewer::getToolBar()
{
    QToolBar *toolbar = new QToolBar(this);
    toolbar->setOrientation(Qt::Vertical);
    toolbar->setIconSize(QSize(18,18));

    // viewer actions (pause, grab image, video...)
    _pauseButton = new QAction("Pause", this);
    _pauseButton->setCheckable(true);
    _pauseButton->setIcon(QIcon(":/icons/img/pause.png"));
    toolbar->addAction(_pauseButton);

    _saveButton = new QAction("Save image", this);
    _saveButton->setToolTip("Save image");
    _saveButton->setIcon(QIcon(":/icons/img/save.png"));
    connect(_saveButton, SIGNAL(triggered(bool)), this, SLOT(saveImage()));
    toolbar->addAction(_saveButton);

    _recordButton = new QAction("Records images", this);
    _recordButton->setToolTip("Records images");
    _recordButton->setCheckable(true);
    _recordButton->setIcon(QIcon(":/icons/img/record.png"));
    connect(_recordButton, SIGNAL(triggered(bool)), this, SLOT(recordImages()));
    toolbar->addAction(_recordButton);

    _settingsButton = new QAction("Settings", this);
    _settingsButton->setToolTip("Open viewer settings");
    _settingsButton->setIcon(QIcon(":/icons/img/settings.png"));
    toolbar->addAction(_settingsButton);

    toolbar->addSeparator();

    // viewer zoom option
    _zoomFitButton = new QAction("Zoom fit", this);
    _zoomFitButton->setToolTip("Zoom fit best");
    _zoomFitButton->setIcon(QIcon(":/icons/img/zoom-fit.png"));
    connect(_zoomFitButton, SIGNAL(triggered(bool)), _widget, SLOT(zoomFit()));
    toolbar->addAction(_zoomFitButton);

    _zoomOutButton = new QAction("Zoom -", this);
    _zoomOutButton->setToolTip("Zoom out");
    _zoomOutButton->setIcon(QIcon(":/icons/img/zoom-out.png"));
    connect(_zoomOutButton, SIGNAL(triggered(bool)), _widget, SLOT(zoomOut()));
    toolbar->addAction(_zoomOutButton);

    _zoomInButton = new QAction("Zoom +", this);
    _zoomInButton->setToolTip("Zoom in");
    _zoomInButton->setIcon(QIcon(":/icons/img/zoom-in.png"));
    connect(_zoomInButton, SIGNAL(triggered(bool)), _widget, SLOT(zoomIn()));
    toolbar->addAction(_zoomInButton);

    return toolbar;
}
