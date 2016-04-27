/****************************************************************************
** Copyright (C) 2016 Dream IP
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

#include "flowviewerinterface.h"

LayerViewer::LayerViewer(FlowViewerInterface *flowViewerInterface)
    : AbstractViewer(flowViewerInterface)
{
    setupWidgets();
    //showFlowConnection(0);
    connect((QObject*)_flowViewerInterface, SIGNAL(dataReceived(int)), this, SLOT(showFlowConnection(int)));
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

    int width = flowProp->property("width").toInt();
    int height = flowProp->property("height").toInt();

    QImage *image = flowPackage.toImage(width, height, 8);
    _widget->showImage(*image);
    delete image;

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

    layout->addItem(getToolBar());

    _widget = new LayerWidget();

    layout->addWidget(_widget);

    setLayout(layout);
}

QLayout *LayerViewer::getToolBar()
{
    QVBoxLayout *layoutTools = new QVBoxLayout();
    layoutTools->setContentsMargins(0,5,2,0);
    layoutTools->setSpacing(2);

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

    layoutTools->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    return layoutTools;
}
