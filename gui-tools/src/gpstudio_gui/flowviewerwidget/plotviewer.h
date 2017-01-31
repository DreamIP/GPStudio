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

#ifndef PLOTVIEWER_H
#define PLOTVIEWER_H

#include <QToolBar>

#include "gpstudio_gui_common.h"

#include "abstractviewer.h"

#include "../../thirdparts/qcustomplot/qcustomplot.h"

class GPSTUDIO_GUI_EXPORT PlotViewer : public AbstractViewer
{
    Q_OBJECT
public:
    PlotViewer(FlowViewerInterface *flowViewerInterface);
    ~PlotViewer();

protected slots:
    void showFlowConnection(int flowId);
    void saveImage();

protected:
    virtual void setupWidgets();
    QToolBar *getToolBar();

    QAction *_pauseButton;
    QAction *_saveButton;
    QAction *_settingsButton;

    QCustomPlot *_widget;
    qint64 _startDate;
};

#endif // PLOTVIEWER_H
