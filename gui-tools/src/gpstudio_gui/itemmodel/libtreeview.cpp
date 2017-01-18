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

#include "libtreeview.h"

#include <QDebug>
#include <QDrag>
#include <QMenu>
#include <QMimeData>

#include "viewer/viewerwidgets/pdfviewer.h"
#include "blockeditor/blockeditorwindow.h"

LibTreeView::LibTreeView(QWidget *parent) :
    QTreeView(parent)
{
    _model = new LibItemModel(this);
    _project = NULL;
    setModel(_model);
    setSortingEnabled(true);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickProcess(QModelIndex)));
}

void LibTreeView::attachProject(GPNodeProject *project)
{
    if(_project)
        disconnect(_project);

    _project = project;

    connect(this, SIGNAL(blockAdded(QString,QPoint)),
            _project, SLOT(addBlock(QString,QPoint)));
}

void LibTreeView::setLib(const Lib *lib)
{
    _model->setLib(lib);

    sortByColumn(0);
    expandAll();
    resizeColumnToContents(0);
    setColumnWidth(1, 100);
}

void LibTreeView::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    const BlockLib *proc = _model->blockLib(currentIndex());
    if(!proc)
        return;

    QByteArray encodedData;
    encodedData.append(proc->name());
    mimeData->setData("ip/process", encodedData);
    drag->setMimeData(mimeData);
    drag->setPixmap(proc->icon().pixmap(32,32));

    drag->exec();
}

void LibTreeView::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_F1)
    {
        const BlockLib *proc = _model->blockLib(currentIndex());
        if(!proc)
            return;
        QStringList docFile = proc->modelBlock()->getPdfDoc();
        if(docFile.isEmpty())
            return;

        PdfViewer::showDocument(docFile.first());
    }
    QTreeView::keyPressEvent(event);
}

#ifndef QT_NO_CONTEXTMENU
void LibTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
        return;
    const BlockLib *proc = _model->blockLib(index);
    if(!proc)
        return;

    QStringList docFile = proc->modelBlock()->getPdfDoc();

    QMenu menu;
    QAction *addIpAction = menu.addAction("Add IP to project");
    QAction *infosIPAction = menu.addAction("View implementation files");
    QAction *docIPAction = menu.addAction("View documentation");
    docIPAction->setShortcut(QKeySequence::HelpContents);
    QFont boldFont = addIpAction->font();
    boldFont.setBold(true);
    addIpAction->setFont(boldFont);
    if(docFile.isEmpty())
        docIPAction->setEnabled(false);
    QAction *trigered = menu.exec(event->globalPos());
    if(trigered == docIPAction)
        PdfViewer::showDocument(docFile.first());
    if(trigered == infosIPAction)
        BlockEditorWindow::showImplementationsFiles(proc->name());
    if(trigered == addIpAction)
        emit blockAdded(proc->name(), QPoint());
}
#endif // QT_NO_CONTEXTMENU

void LibTreeView::doubleClickProcess(QModelIndex index)
{
    if(!index.isValid())
        return;

    const BlockLib *proc = _model->blockLib(currentIndex());
    if(!proc)
        return;

    if(proc)
        emit blockAdded(proc->name(), QPoint());
}
