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

#include "viewerwindow.h"

#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QDebug>
#include <QImage>
#include <QFileDialog>
#include <QFile>

#include <QDateTime>

#include "camera/property.h"
#include "propertywidgets/propertywidgets.h"
#include "connectnodedialog.h"

#include "cameracom.h"
#include "flowcom.h"
#include "flowpackage.h"
#include "camera/flowmanager.h"

#include <QTreeView>
#include "itemmodel/cameraitemmodel.h"
#include "itemmodel/propertyitemmodel.h"

#include <QMessageBox>

#include "model/model_gpviewer.h"
#include "model/model_viewer.h"
#include "model/model_viewerflow.h"

ViewerWindow::ViewerWindow(QStringList args) :
    QMainWindow(0)
{
    _cam = NULL;

    setWindowIcon(QIcon(":/img/img/gpstudio_viewer.ico"));
    setupWidgets();

    createDocks();
    createToolBarAndMenu();

    connect(_viewersMdiArea->blocksView(), SIGNAL(blockSelected(QString)), _camExplorerWidget, SLOT(selectBlock(QString)));
    connect(_viewersMdiArea->blocksView(), SIGNAL(blockSelected(QString)), this, SLOT(showCamExplorer()));
    connect(_camExplorerWidget, SIGNAL(blockSelected(QString)), _viewersMdiArea->blocksView(), SLOT(selectBlock(QString)));
    connect(_viewersMdiArea->blocksView(), SIGNAL(blockDetailsRequest(QString)), this, SLOT(showBlockDetails(QString)));
    connect(_viewerExplorerWidget, SIGNAL(viewerSelected(QString)), _viewersMdiArea, SLOT(selectViewer(QString)));

    if(args.size()>1)
    {
        if(QFile::exists(args[1]))
            openNodeGeneratedFile(args[1]);
    }

    // show tabs
    showCamExplorer();
    _scriptDock->show();
    _scriptDock->raise();

    _blockEditor = NULL;
}

ViewerWindow::~ViewerWindow()
{
    if(_blockEditor)
        delete _blockEditor;
    delete _viewersMdiArea;
    if(_cam)
        delete _cam;
}

bool ViewerWindow::event(QEvent *event)
{
    if(event->type()==QEvent::Close)
    {
        if(_cam)
        {
            if(_cam->com())
            {
                foreach (Block *block, _cam->blocks())
                {
                    Property *prop = block->assocProperty()->path("enable");
                    if(prop)
                        prop->setValue(false);
                }
                QWaitCondition wc;
                QMutex mutex;
                QMutexLocker locker(&mutex);
                wc.wait(&mutex, 200);
                _cam->com()->stop();
            }
        }
    }
    return QMainWindow::event(event);
}

void ViewerWindow::openNode()
{
    QString file = QFileDialog::getOpenFileName(this, "Open node", "", "*.xml");

    if(!file.isEmpty())
        openNodeGeneratedFile(file);
}

void ViewerWindow::createToolBarAndMenu()
{
    _mainToolBar = new QToolBar(this);
    addToolBar(_mainToolBar);

    // ============= File =============
    QMenu *nodeMenu = menuBar()->addMenu("&Node");

    QAction *openDocAction = new QAction("&Open node",this);
    openDocAction->setIcon(QIcon(":/icons/img/open.png"));
    openDocAction->setShortcut(QKeySequence::Open);
    _mainToolBar->addAction(openDocAction);
    nodeMenu->addAction(openDocAction);
    connect(openDocAction, SIGNAL(triggered()), this, SLOT(openNode()));

    QAction *connectAction = new QAction("&Connect node",this);
    connectAction->setIcon(QIcon(":/icons/img/connect.png"));
    _mainToolBar->addAction(connectAction);
    nodeMenu->addAction(connectAction);
    connect(connectAction, SIGNAL(triggered()), this, SLOT(connectCam()));

    nodeMenu->addSeparator();
    QAction *exit = new QAction("E&xit",this);
    exit->setIcon(QIcon(":/icons/img/exit.png"));
    exit->setShortcut(QKeySequence::Quit);
    nodeMenu->addAction(exit);
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    // ============= View =============
    QMenu *viewMenu = menuBar()->addMenu("&View");

    viewMenu->addSeparator();
    viewMenu->addAction(_scriptDock->toggleViewAction());
    viewMenu->addAction(_camExplorerDock->toggleViewAction());
    viewMenu->addSeparator();
    viewMenu->addAction(_piSpaceDock->toggleViewAction());
    viewMenu->addSeparator();

    QAction *camSwitchMode = new QAction("CameraExplorer &switch mode",this);
    viewMenu->addAction(camSwitchMode);
    connect(camSwitchMode, SIGNAL(triggered()), _camExplorerWidget, SLOT(switchModeView()));

    // ============= Windows =============
    _viewersMdiArea->setMenu(menuBar()->addMenu("&Viewers"));

    // ============= Help =============
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction("About &Qt", this);
    connect(aboutQtAction, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));
    helpMenu->addAction(aboutQtAction);

    _mainToolBar->addSeparator();
}

void ViewerWindow::createDocks()
{
    // settings of mdi area
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

    // cam explorer dock
    _camExplorerDock = new QDockWidget("Cam Explorer", this);
    QWidget *camExplorerContent = new QWidget(_camExplorerDock);
    QLayout *camExplorerLayout = new QVBoxLayout();
    _camExplorerWidget = new CamExplorerWidget();
    camExplorerLayout->addWidget(_camExplorerWidget);
    camExplorerContent->setLayout(camExplorerLayout);
    _camExplorerDock->setWidget(camExplorerContent);
    addDockWidget(Qt::LeftDockWidgetArea, _camExplorerDock);

    // viewer explorer dock
    _viewerExplorerDock = new QDockWidget("Viewers", this);
    QWidget *viewerExplorerContent = new QWidget(_viewerExplorerDock);
    QLayout *viewerExplorerLayout = new QVBoxLayout();
    _viewerExplorerWidget = new ViewerExplorerWidget();
    viewerExplorerLayout->addWidget(_viewerExplorerWidget);
    viewerExplorerContent->setLayout(viewerExplorerLayout);
    _viewerExplorerDock->setWidget(viewerExplorerContent);
    tabifyDockWidget(_camExplorerDock, _viewerExplorerDock);

    // flowToCam dock
    _flowToCamDock = new QDockWidget("Flow Sender", this);
    QWidget *flowToCamContent = new QWidget(_flowToCamDock);
    QLayout *flowToCamLayout = new QVBoxLayout();
    _flowToCamWidget = new FlowToCamWidget();
    flowToCamLayout->addWidget(_flowToCamWidget);
    flowToCamContent->setLayout(flowToCamLayout);
    _flowToCamDock->setWidget(flowToCamContent);
    tabifyDockWidget(_camExplorerDock, _flowToCamDock);

    // script dock
    _scriptDock = new QDockWidget("Scripts", this);
    QWidget *scriptContent = new QWidget(_scriptDock);
    QLayout *scriptLayout = new QVBoxLayout();
    _scriptWidget = new ScriptWidget(scriptContent);
    scriptLayout->addWidget(_scriptWidget);
    scriptContent->setLayout(scriptLayout);
    _scriptDock->setWidget(scriptContent);
    addDockWidget(Qt::BottomDockWidgetArea, _scriptDock);

    // pi space dock
    _piSpaceDock = new QDockWidget("PI space", this);
    QWidget *piSpaceWidgetContent = new QWidget(_piSpaceDock);
    QLayout *piSpaceWidgetLayout = new QVBoxLayout();
    _piSpaceHex = new QHexEdit(piSpaceWidgetContent);
    piSpaceWidgetLayout->addWidget(_piSpaceHex);
    piSpaceWidgetContent->setLayout(piSpaceWidgetLayout);
    _piSpaceDock->setWidget(piSpaceWidgetContent);
    tabifyDockWidget(_scriptDock, _piSpaceDock);
}

void ViewerWindow::openNodeGeneratedFile(const QString fileName)
{
    if(_cam)
        delete _cam;

    _cam = new Camera(fileName);
    _viewersMdiArea->setCamera(_cam);

    connect(_cam, SIGNAL(registerDataChanged()), this, SLOT(setBiSpace()));

    connectCam();

    _camExplorerWidget->setCamera(_cam);
    _viewerExplorerWidget->setCamera(_cam);
    _flowToCamWidget->setCamera(_cam);

    if(_cam->com())
        connect(_cam->com(), SIGNAL(disconnected()), this, SLOT(disconnectCam()));
}

void ViewerWindow::connectCam()
{
    if(_cam)
    {
        ConnectNodeDialog connectNodeDialog(this);
        if(connectNodeDialog.exec()==QDialog::Accepted)
        {
            const CameraInfo &cameraInfo = connectNodeDialog.cameraInfo();

            if(cameraInfo.isValid())
            {
                _cam->connectCam(cameraInfo);
                statusBar()->showMessage("camera connected");
            }
        }
        _cam->registermanager().evalAll();
        _scriptWidget->setRootProperty(&_cam->rootProperty());
    }
}

void ViewerWindow::disconnectCam()
{
    statusBar()->showMessage("camera disconnected");
}

void ViewerWindow::setBiSpace()
{
    if(!_cam)
        return;
    _piSpaceHex->setData(_cam->registerData());
}

void ViewerWindow::showBlockDetails(QString blockName)
{
    if(blockName.isEmpty())
        return;
    if(_blockEditor)
        delete _blockEditor;
    _blockEditor = new BlockEditorWindow (this, _cam->block(blockName)->modelBlock());
    _blockEditor->show();
}

void ViewerWindow::showCamExplorer()
{
    _camExplorerDock->show();
    _camExplorerDock->raise();
}

void ViewerWindow::setupWidgets()
{
    _viewersMdiArea = new ViewersMdiArea();
    setCentralWidget(_viewersMdiArea);

    QMenuBar *menubar = new QMenuBar(this);
    setMenuBar(menubar);

    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
}

void ViewerWindow::about()
{
    QMessageBox::about(this,"GPStudio: GPViewer 1.21", QString("Copyright (C) 2014-2017 Dream IP (<a href=\"http://dream-lab.fr\">dream-lab.fr</a>)<br>\
<br>\
This sofware is part of GPStudio distribution. To check for new version, please visit <a href=\"http://gpstudio.univ-bpclermont.fr/download\">gpstudio.univ-bpclermont.fr/download</a><br>\
<br>\
GPStudio is a free software: you can redistribute it and/or modify\
it under the terms of the GNU General Public License as published by\
the Free Software Foundation, either version 3 of the License, or\
(at your option) any later version.<br>\
<br>\
This program is distributed in the hope that it will be useful,\
but WITHOUT ANY WARRANTY; without even the implied warranty of\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\
GNU General Public License for more details.<br>\
<br>\
You should have received a copy of the GNU General Public License\
along with this program.  If not, see <a href=\"http://www.gnu.org/licenses/\">www.gnu.org/licenses</a><br>\
<br>\
Build date: ") + __DATE__ + QString(" time: ")+__TIME__);
}

void ViewerWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}
