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

#include "plotviewer.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFileDialog>

#include "camera/flowviewerinterface.h"

#include "scriptengine/scriptengine.h"

PlotViewer::PlotViewer(FlowViewerInterface *flowViewerInterface)
    : AbstractViewer(flowViewerInterface)
{
    setupWidgets();
    _startDate = 0;
    //showFlowConnection(0);
    connect((QObject*)_flowViewerInterface, SIGNAL(dataReceived(int)), this, SLOT(showFlowConnection(int)));
}

PlotViewer::~PlotViewer()
{
}

void PlotViewer::showFlowConnection(int flowId)
{
    /*if(flowId>=_flowViewerInterface->flowConnections().size())
        return;*/

    if(_pauseButton->isChecked())
        return;

    const FlowPackage flowPackage = _flowViewerInterface->flowConnections()[flowId]->lastData();

    QByteArray data = flowPackage.data();

    QScriptValue function = ScriptEngine::getEngine().engine()->evaluate("(function(x){ return x / 16384.0;})");

    for(int i=0; i<data.size(); i+=2)
    {
        if(i>6) break;
        short dataItem = ((short)((char)data[i])<<8) + data[i+1];
        if ((short)((char)data[i+1])<0) dataItem +=256;
        QScriptValueList args;
        args << dataItem;

        if(_widget->graphCount()<i/2+1)
        {
            _widget->addGraph();
            _widget->graph(i/2)->setName(QString("plot%1").arg(i));
            _widget->graph(i/2)->setPen(QColor::fromHsv(i*10,255,255));
            _widget->graph(i/2)->setAdaptiveSampling(true);
        }

        float value = function.call(QScriptValue(), args).toNumber();
        _widget->graph(i/2)->addData(QCPData(QDateTime::currentMSecsSinceEpoch(), value));
    }

    if(_startDate==0)
    {
        _startDate = QDateTime::currentMSecsSinceEpoch();
    }
    _widget->xAxis->setRange(_startDate, QDateTime::currentMSecsSinceEpoch());

    _widget->replot();
}

void PlotViewer::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save image...", "", "Images (*.jpg)");
    if(!fileName.isEmpty())
    {
        QFileInfo info(fileName);
        if(info.completeSuffix()=="")
            fileName.append(".jpg");
        _widget->saveJpg(fileName);
    }
}

void PlotViewer::setupWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _widget = new QCustomPlot();
    _widget->yAxis->setRange(-2,2);
    _widget->legend->setVisible(true);
    _widget->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    _widget->xAxis->setDateTimeFormat("HH:mm:ss");

    layout->addItem(getToolBar());
    layout->addWidget(_widget);

    setLayout(layout);
}

QLayout *PlotViewer::getToolBar()
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

    _settingsButton = new QToolButton();
    _settingsButton->setToolTip("Records images");
    _settingsButton->setAutoRaise(true);
    _settingsButton->setIcon(QIcon(":/icons/img/settings.png"));
    layoutTools->addWidget(_settingsButton);

    layoutTools->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    return layoutTools;
}
