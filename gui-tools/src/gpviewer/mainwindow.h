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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDockWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "camera/camera.h"
#include "flowviewerwidget/flowviewerwidget.h"

#include "viewer/camexplorerwidget.h"
#include "viewer/viewerexplorerwidget.h"
#include "viewer/flowtocamwidget.h"
#include "scriptwidget.h"

#include "lib_parser/lib.h"

#include "../../thirdparts/hexedit/qhexedit.h"
#include "blockview/blockview.h"

#include "blockeditor/blockeditorwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QStringList args);
    ~MainWindow();

    void openNodeGeneratedFile(const QString fileName);

public slots:
    void about();
    void aboutQt();

protected:
    bool event(QEvent *event);

private slots:
    void openNode();

    void connectCam();
    void disconnectCam();

    void setBiSpace();

    void updateWindowsMenu();
    void showBlockDetails(QString blockName);
    void showCamExplorer();

private:
    Camera *_cam;

    // widgets
    void setupWidgets();
    QMdiArea *_viewersMdiArea;

    // viewer
    void setupViewers();
    QMap<int, FlowViewerWidget *> _viewers;

    // docks
    void createDocks();

    QDockWidget *_piSpaceDock;
    QHexEdit *_piSpaceHex;

    QDockWidget *_blocksViewDock;
    BlockView *_blocksView;

    QDockWidget *_camExplorerDock;
    CamExplorerWidget *_camExplorerWidget;

    QDockWidget *_viewerExplorerDock;
    ViewerExplorerWidget *_viewerExplorerWidget;

    QDockWidget *_flowToCamDock;
    FlowToCamWidget *_flowToCamWidget;

    QDockWidget *_scriptDock;
    ScriptWidget *_scriptWidget;

    // menu and toolbar
    void createToolBarAndMenu();
    QToolBar *_mainToolBar;
    QMenu *_winMenu;
    QAction *_closeAct;
    QAction *_closeAllAct;
    QAction *_tileAct;
    QAction *_cascadeAct;
    QAction *_nextAct;
    QAction *_previousAct;

    BlockEditorWindow *_blockEditor;
};

#endif // MAINWINDOW_H
