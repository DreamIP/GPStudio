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

#ifndef LAYERVIEWER_H
#define LAYERVIEWER_H

#include "gpstudio_gui_common.h"

#include "abstractviewer.h"

#include "viewer/viewerwidgets/layerwidget.h"

#include <QToolButton>

class GPSTUDIO_GUI_EXPORT LayerViewer : public AbstractViewer
{
    Q_OBJECT
public:
    LayerViewer(FlowViewerInterface *flowViewerInterface);
    ~LayerViewer();

protected slots:
    void showFlowConnection(int flowId);
    void saveImage();
    void recordImages();

protected:
    virtual void setupWidgets();

    // toolbar members
    QLayout *getToolBar();
    QToolButton *_pauseButton;
    QToolButton *_saveButton;
    QToolButton *_recordButton;
    QToolButton *_settingsButton;

    QToolButton *_zoomFitButton;
    QToolButton *_zoomOutButton;
    QToolButton *_zoomInButton;

    QString _recordPath;

    LayerWidget *_widget;
};

#endif // LAYERVIEWER_H
