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

#include "viewersmdiarea.h"

#include <QAction>
#include <QMenu>
#include <QDebug>

#include "camera/camera.h"
#include "camera/flowmanager.h"
#include "camera/flowconnection.h"
#include "camera/flowviewerinterface.h"
#include "flowviewerwidget/flowviewerwidget.h"

#include "model/model_gpviewer.h"
#include "model/model_viewer.h"
#include "model/model_viewerflow.h"

#include "blockview/blockview.h"

ViewersMdiArea::ViewersMdiArea()
{
    _blocksView = new BlockView(this);

    createMenu();
}

QMenu *ViewersMdiArea::menu() const
{
    return _menu;
}

void ViewersMdiArea::setCamera(Camera *camera)
{
    _camera = camera;
    setupViewers();
}

void ViewersMdiArea::toggleBlockView()
{
    _blocksWindow->show();
    _blocksView->show();
}

void ViewersMdiArea::selectViewer(QString name)
{
    QList<QMdiSubWindow *> windows = subWindowList();

    for (int i = 0; i < windows.size(); ++i)
    {
        QMdiSubWindow *child = windows.at(i);
        if(child->windowTitle() == name)
            setActiveSubWindow(child);
    }

    if(_camera)
    {
        ModelViewer *viewer = _camera->node()->gpViewer()->getViewer(name);
        if(!viewer)
            return;
        _blocksView->clearSelection();
        QStringList list = viewer->viewerFlowsName();
        for(int i=0; i<list.size(); i++)
        {
            _blocksView->selectFlowCom(list[i]);
        }
    }
}

void ViewersMdiArea::setMenu(QMenu *menu)
{
    blockSignals(true);
    _menu = menu;
    updateWindowsMenu();
    blockSignals(false);
}

void ViewersMdiArea::updateWindowsMenu()
{
    _menu->clear();
    _menu->addAction(_viewBlockAct);
    _menu->addSeparator();
    _menu->addAction(_closeAct);
    _menu->addAction(_closeAllAct);
    _menu->addSeparator();
    _menu->addAction(_tileAct);
    _menu->addAction(_cascadeAct);
    _menu->addSeparator();
    _menu->addAction(_nextAct);
    _menu->addAction(_previousAct);

    QList<QMdiSubWindow *> windows = subWindowList();

    for (int i = 0; i < windows.size(); ++i)
    {
        QMdiSubWindow *child = windows.at(i);

        QString text;
        if (i < 9)
            text = tr("&%1 %2").arg(i + 1).arg(child->windowTitle());
        else
            text = tr("%1 %2").arg(i + 1).arg(child->windowTitle());
        QAction *action  = _menu->addAction(text);
        action->setData(child->windowTitle());
        action->setCheckable(true);
        if(child == activeSubWindow())
        {
            action->setChecked(true);
            if(child->windowTitle() != "Blocks view")
            {
                emit viewerSelected(child->windowTitle());
                selectViewer(child->windowTitle());
            }
        }
        connect(action, SIGNAL(triggered(bool)), this, SLOT(selectViewerAction()));
    }
}

void ViewersMdiArea::selectViewerAction()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        selectViewer(action->data().toString());
}

BlockView *ViewersMdiArea::blocksView() const
{
    return _blocksView;
}

void ViewersMdiArea::createMenu()
{
    _viewBlockAct = new QAction(tr("Toggle block view show"), this);
    _viewBlockAct->setStatusTip(tr("Toggle block view show"));
    connect(_viewBlockAct, SIGNAL(triggered()), this, SLOT(toggleBlockView()));

    _closeAct = new QAction(tr("Cl&ose"), this);
    _closeAct->setIcon(QIcon(":/icons/img/window-suppressed.png"));
    _closeAct->setShortcuts(QKeySequence::Close);
    _closeAct->setStatusTip(tr("Close the active window"));
    connect(_closeAct, SIGNAL(triggered()), this, SLOT(closeActiveSubWindow()));

    _closeAllAct = new QAction(tr("Close &All"), this);
    _closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(_closeAllAct, SIGNAL(triggered()), this, SLOT(closeAllSubWindows()));

    _tileAct = new QAction(tr("&Tile"), this);
    _tileAct->setIcon(QIcon(":/icons/img/windows-cascade.png"));
    _tileAct->setStatusTip(tr("Tile the windows"));
    connect(_tileAct, SIGNAL(triggered()), this, SLOT(tileSubWindows()));

    _cascadeAct = new QAction(tr("&Cascade"), this);
    _cascadeAct->setIcon(QIcon(":/icons/img/windows-tile.png"));
    _cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(_cascadeAct, SIGNAL(triggered()), this, SLOT(cascadeSubWindows()));

    _nextAct = new QAction(tr("Ne&xt"), this);
    _nextAct->setShortcuts(QKeySequence::NextChild);
    _nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(_nextAct, SIGNAL(triggered()), this, SLOT(activateNextSubWindow()));

    _previousAct = new QAction(tr("Pre&vious"), this);
    _previousAct->setShortcuts(QKeySequence::PreviousChild);
    _previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(_previousAct, SIGNAL(triggered()), this, SLOT(activatePreviousSubWindow()));

    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateWindowsMenu()));
}

void ViewersMdiArea::setupViewers()
{
    blockSignals(true);
    closeAllSubWindows();
    _viewers.clear();

    int i=0;
    if(_camera->node()->gpViewer()->viewers().isEmpty())
    {
        foreach (FlowConnection *connection, _camera->flowManager()->flowConnections())
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
        foreach(ModelViewer *viewer, _camera->node()->gpViewer()->viewers())
        {
            QList<FlowConnection *> flowConnections;
            foreach(ModelViewerFlow *viewerflow, viewer->viewerFlows())
            {
                FlowConnection *connection = _camera->flowManager()->flowConnection(viewerflow->flowName());
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
        QMdiSubWindow * windows = addSubWindow(_viewers[i]);
        windows->show();
    }

    // adding block view
    if(_camera)
        _blocksView->loadFromCam(_camera);
    _blocksWindow = new QMdiSubWindow();
    _blocksWindow->setWidget(_blocksView);
    //blocksWindow->setAttribute(Qt::WA_DeleteOnClose); // disabled, if not block is deleted on each close subwindow
    addSubWindow(_blocksWindow);
    _blocksWindow->setWindowTitle("Blocks view");
    _blocksWindow->show();

    tileSubWindows();
    blockSignals(false);
}
