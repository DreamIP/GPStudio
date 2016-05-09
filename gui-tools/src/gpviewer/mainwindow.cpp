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

MainWindow::MainWindow(QStringList args) :
    QMainWindow(0),
    ui(new Ui::MainWindow)
{
    _cam = NULL;

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
    QMenu *helpMenu = ui->menuBar->addMenu("&Help");

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

    setupViewers(2);

    connect(_cam, SIGNAL(registerDataChanged()), this, SLOT(setBiSpace()));

    connectCam();

    _camExplorerWidget->setCamera(_cam);
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
            }
        }
    }
}

void MainWindow::setBiSpace()
{
    if(!_cam) return;
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

void MainWindow::showBlockDetails(const Block *block)
{
    if(_blockEditor) delete _blockEditor;
    _blockEditor = new BlockEditorWindow (this, block->modelBlock());
    _blockEditor->show();
}

void MainWindow::setupViewers(int count)
{
    ui->mdiArea->closeAllSubWindows();
    _viewers.clear();

    int i=0;
    foreach (FlowConnection *connection, _cam->flowManager()->flowConnections())
    {
        if(connection->flow()->type()==Flow::Input)
        {
            FlowViewerWidget *viewer = new FlowViewerWidget(new FlowViewerInterface(connection));
            ScriptEngine::getEngine().engine()->globalObject().setProperty(connection->flow()->name(), ScriptEngine::getEngine().engine()->newQObject(viewer));
            viewer->setWindowTitle(QString("Flow %1").arg(connection->flow()->name()));
            _viewers.insert(i, viewer);
            i++;
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
    connect(_blocksView, SIGNAL(blockSelected(const Block*)), _camExplorerWidget, SLOT(selectBlock(const Block*)));
    connect(_camExplorerWidget, SIGNAL(blockSelected(const Block*)), _blocksView, SLOT(selectBlock(const Block*)));
    connect(_blocksView, SIGNAL(blockDetailsRequest(const Block*)), this, SLOT(showBlockDetails(const Block*)));
    windows->setWindowTitle("Blocks view");
    windows->show();

    ui->mdiArea->tileSubWindows();
}
