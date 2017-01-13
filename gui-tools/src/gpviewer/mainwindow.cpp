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

#include "mainwindow.h"
#include "ui_mainwindow.h"

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

MainWindow::MainWindow(QStringList args) :
    QMainWindow(0),
    ui(new Ui::MainWindow)
{
    _cam = NULL;

    setWindowIcon(QIcon(":/img/img/gpstudio_viewer.ico"));
    ui->setupUi(this);

    createDocks();
    createToolBarAndMenu();

    if(args.size()>1)
    {
        if(QFile::exists(args[1])) openNodeGeneratedFile(args[1]);
    }

    // show tabs
    _camExplorerDock->show();
    _camExplorerDock->raise();
    _scriptDock->show();
    _scriptDock->raise();

    _blockEditor = NULL;
}

MainWindow::~MainWindow()
{
    if(_blockEditor) delete _blockEditor;
    delete ui;
    if(_cam) delete _cam;
}

bool MainWindow::event(QEvent *event)
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

void MainWindow::openNode()
{
    QString file = QFileDialog::getOpenFileName(this, "Open node", "", "*.xml");

    if(!file.isEmpty()) openNodeGeneratedFile(file);
}

void MainWindow::createToolBarAndMenu()
{
    // ============= File =============
    QMenu *nodeMenu = ui->menuBar->addMenu("&Node");

    QAction *openDocAction = new QAction("&Open node",this);
    openDocAction->setIcon(QIcon(":/icons/img/open.png"));
    openDocAction->setShortcut(QKeySequence::Open);
    ui->mainToolBar->addAction(openDocAction);
    nodeMenu->addAction(openDocAction);
    connect(openDocAction, SIGNAL(triggered()), this, SLOT(openNode()));

    QAction *connectAction = new QAction("&Connect node",this);
    connectAction->setIcon(QIcon(":/icons/img/connect.png"));
    ui->mainToolBar->addAction(connectAction);
    nodeMenu->addAction(connectAction);
    connect(connectAction, SIGNAL(triggered()), this, SLOT(connectCam()));

    nodeMenu->addSeparator();
    QAction *exit = new QAction("E&xit",this);
    exit->setIcon(QIcon(":/icons/img/exit.png"));
    exit->setShortcut(QKeySequence::Quit);
    nodeMenu->addAction(exit);
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    // ============= View =============
    QMenu *viewMenu = ui->menuBar->addMenu("&View");

    viewMenu->addSeparator();
    viewMenu->addAction(_scriptDock->toggleViewAction());
    viewMenu->addAction(_camExplorerDock->toggleViewAction());
    viewMenu->addSeparator();
    viewMenu->addAction(_piSpaceDock->toggleViewAction());

    // ============= Windows =============
    _winMenu = ui->menuBar->addMenu("&Windows");
    _closeAct = new QAction(tr("Cl&ose"), this);
    _closeAct->setStatusTip(tr("Close the active window"));
    connect(_closeAct, SIGNAL(triggered()), ui->mdiArea, SLOT(closeActiveSubWindow()));

    _closeAllAct = new QAction(tr("Close &All"), this);
    _closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(_closeAllAct, SIGNAL(triggered()), ui->mdiArea, SLOT(closeAllSubWindows()));

    _tileAct = new QAction(tr("&Tile"), this);
    _tileAct->setStatusTip(tr("Tile the windows"));
    connect(_tileAct, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));

    _cascadeAct = new QAction(tr("&Cascade"), this);
    _cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(_cascadeAct, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));

    _nextAct = new QAction(tr("Ne&xt"), this);
    _nextAct->setShortcuts(QKeySequence::NextChild);
    _nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(_nextAct, SIGNAL(triggered()), ui->mdiArea, SLOT(activateNextSubWindow()));

    _previousAct = new QAction(tr("Pre&vious"), this);
    _previousAct->setShortcuts(QKeySequence::PreviousChild);
    _previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(_previousAct, SIGNAL(triggered()), ui->mdiArea, SLOT(activatePreviousSubWindow()));

    updateWindowsMenu();
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateWindowsMenu()));

    // ============= Help =============
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction("About &Qt", this);
    connect(aboutQtAction, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));
    helpMenu->addAction(aboutQtAction);

    ui->mainToolBar->addSeparator();
}

void MainWindow::createDocks()
{
    // settings of mdi area
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

    // cam explorer dock
    _camExplorerDock = new QDockWidget("CamExplorer", this);
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

void MainWindow::openNodeGeneratedFile(const QString fileName)
{
    if(_cam) delete _cam;

    _cam = new Camera(fileName);

    setupViewers();

    connect(_cam, SIGNAL(registerDataChanged()), this, SLOT(setBiSpace()));

    connectCam();

    _camExplorerWidget->setCamera(_cam);
    _viewerExplorerWidget->setCamera(_cam);

    if(_cam->com())
        connect(_cam->com(), SIGNAL(disconnected()), this, SLOT(disconnectCam()));
}

void MainWindow::connectCam()
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
                ui->statusBar->showMessage("camera connected");
            }
        }
        _cam->registermanager().evalAll();
        _scriptWidget->setRootProperty(&_cam->rootProperty());
    }
}

void MainWindow::disconnectCam()
{
    ui->statusBar->showMessage("camera disconnected");
}

void MainWindow::setBiSpace()
{
    if(!_cam)
        return;
    _piSpaceHex->setData(_cam->registerData());
}

void MainWindow::updateWindowsMenu()
{
    _winMenu->clear();
    _winMenu->addAction(_closeAct);
    _winMenu->addAction(_closeAllAct);
    _winMenu->addSeparator();
    _winMenu->addAction(_tileAct);
    _winMenu->addAction(_cascadeAct);
    _winMenu->addSeparator();
    _winMenu->addAction(_nextAct);
    _winMenu->addAction(_previousAct);

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i)
    {
        QMdiSubWindow *child = windows.at(i);

        QString text;
        if (i < 9) text = tr("&%1 %2").arg(i + 1).arg(child->windowTitle());
        else text = tr("%1 %2").arg(i + 1).arg(child->windowTitle());
        QAction *action  = _winMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == ui->mdiArea->activeSubWindow());
    }
}

void MainWindow::showBlockDetails(QString blockName)
{
    if(blockName.isEmpty())
        return;
    if(_blockEditor)
        delete _blockEditor;
    _blockEditor = new BlockEditorWindow (this, _cam->block(blockName)->modelBlock());
    _blockEditor->show();
}

void MainWindow::setupViewers()
{
    ui->mdiArea->closeAllSubWindows();
    _viewers.clear();

    int i=0;
    if(_cam->node()->gpViewer()->viewers().isEmpty())
    {
        foreach (FlowConnection *connection, _cam->flowManager()->flowConnections())
        {
            if(connection->flow()->type()==Flow::Input)
            {
                FlowViewerWidget *viewer = new FlowViewerWidget(new FlowViewerInterface(connection));
                //ScriptEngine::getEngine().engine()->globalObject().setProperty(connection->flow()->name(), ScriptEngine::getEngine().engine()->newQObject(viewer));
                viewer->setWindowTitle(QString("Flow %1").arg(connection->flow()->name()));
                _viewers.insert(i, viewer);
                i++;
            }
        }
    }
    else
    {
        foreach(ModelViewer *viewer, _cam->node()->gpViewer()->viewers())
        {
            QList<FlowConnection *> flowConnections;
            foreach(ModelViewerFlow *viewerflow, viewer->viewerFlows())
            {
                FlowConnection *connection = _cam->flowManager()->flowConnection(viewerflow->flowName());
                if(connection)
                    flowConnections.append(connection);
            }
            FlowViewerInterface *viewerInterface = new FlowViewerInterface(flowConnections);
            FlowViewerWidget *viewerWidget = new FlowViewerWidget(viewerInterface);
            //ScriptEngine::getEngine().engine()->globalObject().setProperty(connection->flow()->name(), ScriptEngine::getEngine().engine()->newQObject(viewerWidget));
            viewerWidget->setWindowTitle(viewer->name());
            _viewers.insert(i++, viewerWidget);
        }
    }

    // adding flow view (reverse order to have alphabetic order)
    for(i=_viewers.count()-1; i>=0; i--)
    {
        QMdiSubWindow * windows = ui->mdiArea->addSubWindow(_viewers[i]);
        windows->show();
    }

    // adding block view
    _blocksView = new BlockView(this);
    if(_cam)
        _blocksView->loadFromCam(_cam);
    QMdiSubWindow * windows = ui->mdiArea->addSubWindow(_blocksView);
    connect(_blocksView, SIGNAL(blockSelected(QString)), _camExplorerWidget, SLOT(selectBlock(QString)));
    connect(_blocksView, SIGNAL(blockSelected(QString)), _camExplorerWidget, SLOT(raise()));
    connect(_camExplorerWidget, SIGNAL(blockSelected(QString)), _blocksView, SLOT(selectBlock(QString)));
    connect(_blocksView, SIGNAL(blockDetailsRequest(QString)), this, SLOT(showBlockDetails(QString)));
    windows->setWindowTitle("Blocks view");
    windows->show();

    ui->mdiArea->tileSubWindows();
}

void MainWindow::about()
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

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}
