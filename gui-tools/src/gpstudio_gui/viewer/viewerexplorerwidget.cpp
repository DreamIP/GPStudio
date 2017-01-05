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

#include "viewerexplorerwidget.h"

#include <QLayout>
#include <QGroupBox>
#include <QDebug>

#include "model/model_gpviewer.h"

ViewerExplorerWidget::ViewerExplorerWidget(QWidget *parent) : QWidget(parent)
{
    _project = NULL;

    setupWidgets();
}

void ViewerExplorerWidget::attachProject(GPNodeProject *project)
{
    _project = project;

    if(_project->camera())
    {
        setCamera(_project->camera());
        _viewerTreeView->attachProject(project);
    }

    connect(_viewerTreeView, SIGNAL(viewerDeleted(QString)), this, SLOT(removeViewer(QString)));
    connect(this, SIGNAL(viewerDeleted(ModelViewer*)), _project, SLOT(removeViewer(ModelViewer*)));

    connect(_viewerTreeView, SIGNAL(viewerFlowDeleted(QString,QString)), this, SLOT(removeViewerFlow(QString,QString)));
    connect(this, SIGNAL(viewerFlowDeleted(ModelViewerFlow*)), _project, SLOT(removeViewerFlow(ModelViewerFlow*)));
}

GPNodeProject *ViewerExplorerWidget::project() const
{
    return _project;
}

void ViewerExplorerWidget::setCamera(Camera *camera)
{
    _camera = camera;

    if(_camera->comBlock())
        _flowItemModel->setBlock(_camera->comBlock(), CameraItem::FFlowIn);
    _flowTreeView->expandAll();

    if(_camera->node()->gpViewer())
        _viewerTreeView->setCamera(_camera);
    _viewerTreeView->expandAll();
}

Camera *ViewerExplorerWidget::camera() const
{
    return _camera;
}

void ViewerExplorerWidget::removeViewer(QString viewerName)
{
    ModelViewer *viewer = _camera->node()->gpViewer()->getViewer(viewerName);
    if(viewer)
        emit viewerDeleted(viewer);
}

void ViewerExplorerWidget::removeViewerFlow(QString viewerName, QString viewerFlowName)
{
    ModelViewerFlow *viewerFlow = _camera->node()->gpViewer()->getViewerFlow(viewerName, viewerFlowName);
    if(viewerFlow)
        emit viewerFlowDeleted(viewerFlow);
}

void ViewerExplorerWidget::setupWidgets()
{
    if(layout())
        layout()->deleteLater();

    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    // flow list
    QGroupBox *groupBox = new QGroupBox("Com flows");
    QLayout *layoutBox = new QVBoxLayout();
    layoutBox->setContentsMargins(0,10,0,0);
    _flowItemModel = new CameraItemModel();
    _flowTreeView = new QTreeView();
    _flowTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _flowTreeView->setDragEnabled(true);
    _flowTreeView->setModel(_flowItemModel);
    _flowTreeView->setSortingEnabled(true);
    _flowTreeView->sortByColumn(0, Qt::AscendingOrder);
    layoutBox->addWidget(_flowTreeView);
    groupBox->setLayout(layoutBox);
    layout->addWidget(groupBox);

    // viewer list
    QGroupBox *groupBox2 = new QGroupBox("Viewers");
    QLayout *layoutBox2 = new QVBoxLayout();
    layoutBox2->setContentsMargins(0,10,0,0);
    _viewerTreeView = new ViewerTreeView();
    _viewerTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _viewerTreeView->setAcceptDrops(true);
    _viewerTreeView->viewport()->setAcceptDrops(true);
    _viewerTreeView->setDropIndicatorShown(true);
    _viewerTreeView->setSortingEnabled(true);
    _viewerTreeView->sortByColumn(0, Qt::DescendingOrder);
    layoutBox2->addWidget(_viewerTreeView);
    groupBox2->setLayout(layoutBox2);
    layout->addWidget(groupBox2);

    setLayout(layout);
}
