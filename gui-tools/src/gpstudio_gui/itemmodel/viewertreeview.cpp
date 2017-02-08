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

#include "viewertreeview.h"

#include <QDebug>
#include <QMimeData>

#include "camera/camera.h"
#include "model/model_viewerflow.h"
#include "model/model_gpviewer.h"

ViewerTreeView::ViewerTreeView()
{
    _model = new CameraItemModel(this);
    _modelSorted = new QSortFilterProxyModel(this);
    _modelSorted->setSourceModel(_model);
    setModel(_modelSorted);

    connect(this->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateSelection()));
}

void ViewerTreeView::attachProject(GPNodeProject *project)
{
    _project = project;
    if(_project->node())
        setGpviewer(_project->node()->gpViewer());

    connect(_project, SIGNAL(nodeChanged(ModelNode*)), this, SLOT(updateViewer()));

    connect(_project, SIGNAL(viewerUpdated(ModelViewer*)), _model, SLOT(updateViewer(ModelViewer*)));
    connect(_project, SIGNAL(viewerAdded(ModelViewer*)), _model, SLOT(addViewer(ModelViewer*)));
    connect(_project, SIGNAL(viewerFlowAdded(ModelViewerFlow*)), _model, SLOT(addViewerFlow(ModelViewerFlow*)));
    connect(_project, SIGNAL(viewerRemoved(QString)), _model, SLOT(removeViewer(QString)));
    connect(_project, SIGNAL(viewerFlowRemoved(QString,QString)), _model, SLOT(removeViewerFlow(QString,QString)));

    connect(_model, SIGNAL(viewerAdded(ModelViewer*)), _project, SLOT(addViewer(ModelViewer*)));
    connect(_model, SIGNAL(viewerRenamed(QString,QString)), _project, SLOT(renameViewer(QString,QString)));
    connect(_model, SIGNAL(viewerFlowAdded(QString,ModelViewerFlow*)), _project, SLOT(addViewerFlow(QString,ModelViewerFlow*)));
}

GPNodeProject *ViewerTreeView::project() const
{
    return _project;
}

ModelGPViewer *ViewerTreeView::gpviewer() const
{
    return _gpviewer;
}

void ViewerTreeView::setGpviewer(ModelGPViewer *gpviewer)
{
    _model->setViewer(gpviewer);
    expandAll();
}

void ViewerTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeView::dragEnterEvent(event);

    if(event->mimeData()->hasFormat("flow/flowid"))
        event->accept();
}

void ViewerTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeView::dragMoveEvent(event);

    if(event->mimeData()->hasFormat("flow/flowid"))
        event->accept();
}

void ViewerTreeView::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Delete || event->key()==Qt::Key_Backspace)
    {
        if(!currentIndex().isValid())
            return;
        const ModelViewer *viewer = _model->viewer(_modelSorted->mapToSource(currentIndex()));
        if(viewer)
        {
            emit viewerDeleted(viewer->name());
            return;
        }
        const ModelViewerFlow *viewerFlow = _model->viewerFlow(_modelSorted->mapToSource(currentIndex()));
        if(viewerFlow)
        {
            emit viewerFlowDeleted(viewerFlow->viewer()->name(), viewerFlow->flowName());
            return;
        }
    }
    QTreeView::keyPressEvent(event);
}

void ViewerTreeView::selectViewer(QString viewerName)
{
    blockSignals(true);
    selectionModel()->clearSelection();
    if(!viewerName.isEmpty())
    {
        QModelIndexList items = model()->match(model()->index(0, 0), Qt::DisplayRole, QVariant(viewerName), -1, Qt::MatchRecursive);
        if(items.count()>0)
        {
            selectionModel()->select(items.at(0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }
    blockSignals(false);
}

void ViewerTreeView::updateViewer()
{
    if(_project->node())
        setGpviewer(_project->node()->gpViewer());
    else
        setGpviewer(NULL);
}

void ViewerTreeView::updateSelection()
{
    const ModelViewer *viewer = _model->viewer(_modelSorted->mapToSource(currentIndex()));
    if(viewer)
    {
        emit viewerSelected(viewer->name());
        return;
    }
    const ModelViewerFlow *viewerFlow = _model->viewerFlow(_modelSorted->mapToSource(currentIndex()));
    if(viewerFlow)
    {
        emit viewerSelected(viewerFlow->viewer()->name());
    }
}

#ifndef QT_NO_CONTEXTMENU
void ViewerTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)
    /*QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
        return;
    const BlockLib *proc = _model->blockLib(index);
    if(!proc)
        return;*/

    /*QMenu menu;
    QAction *infosIPAction = menu.addAction("View implementation files");
    QAction *docIPAction = menu.addAction("View pdf documentation");
    if(docFile.isEmpty())
        docIPAction->setEnabled(false);
    QAction *trigered = menu.exec(event->globalPos());
    if(trigered == docIPAction)
        PdfViewer::showDocument(docFile.first());*/
}
#endif // QT_NO_CONTEXTMENU
