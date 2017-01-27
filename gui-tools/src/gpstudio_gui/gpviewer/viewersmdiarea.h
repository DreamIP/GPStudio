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

#ifndef VIEWERSMDIAREA_H
#define VIEWERSMDIAREA_H

#include "gpstudio_gui_common.h"

#include <QMdiArea>

#include <QMap>
#include <QMdiSubWindow>

class FlowViewerWidget;
class Camera;
class BlockView;

class GPSTUDIO_GUI_EXPORT ViewersMdiArea : public QMdiArea
{
    Q_OBJECT

public:
    ViewersMdiArea();

    void setMenu(QMenu *menu);
    QMenu *menu() const;

    void setCamera(Camera *camera);

    BlockView *blocksView() const;

public slots:
    void toggleBlockView();

protected slots:
    void updateWindowsMenu();

protected:
    BlockView *_blocksView;
    QMdiSubWindow *_blocksWindow;

    void createMenu();
    QMenu *_menu;
    QAction *_viewBlockAct;
    QAction *_closeAct;
    QAction *_closeAllAct;
    QAction *_tileAct;
    QAction *_cascadeAct;
    QAction *_nextAct;
    QAction *_previousAct;

    void setupViewers();
    QMap<int, FlowViewerWidget *> _viewers;

    Camera *_camera;
};

#endif // VIEWERSMDIAREA_H
