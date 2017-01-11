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

#ifndef CAMERAITEMMODEL_H
#define CAMERAITEMMODEL_H

#include "gpstudio_gui_common.h"

#include "cameraitem.h"

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

class Camera;
class ModelNode;
class ModelGPViewer;

class GPSTUDIO_GUI_EXPORT CameraItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CameraItemModel(QObject *parent = 0);
    CameraItemModel(Camera *camera, QObject *parent = 0);

    enum Column {
        Name,
        Value,
        ColumnCount
    };

    void setCamera(const Camera *camera, uint filter=CameraItem::FAll);
    void setBlock(const Block *block, uint filter=CameraItem::FAll);
    void setNode(const ModelNode *node, uint filter=CameraItem::FAll);
    void setViewer(const ModelGPViewer *gpViewer, uint filter=CameraItem::FAll);
    void clearAll();

    const ModelGPViewer *gpViewer(const QModelIndex &index) const;
    const ModelViewer *viewer(const QModelIndex &index) const;
    const ModelViewerFlow *viewerFlow(const QModelIndex &index) const;

    QModelIndex indexViewer(const QString &viewerName) const;
    QModelIndex indexViewerFlow(const QString &viewerName, const QString &viewerFlowName) const;

    // QAbstractItemModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // drag and drop funtionnality
    Qt::DropActions supportedDropActions() const;
    Qt::DropActions supportedDragActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool canDropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent) const;
    bool dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    // add remove
    //bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

private:
    CameraItem *_rootItem;
    void setRootItem(CameraItem *rootItem);

signals:
    void nodeRenamed(const QString &blockName, const QString &newName);

    void blockRenamed(const QString &blockName, const QString &newName);

    void viewerAdded(ModelViewer *viewer);
    void viewerRenamed(const QString &viewerName, const QString &newName);
    void viewerFlowRenamed(const QString &viewerName, const QString &viewerFlowName, const QString &newName);

    void viewerFlowAdded(const QString &viewerName, ModelViewerFlow *viewerFlow);

public slots:
    void updateViewer(ModelViewer *viewer);
    void addViewer(ModelViewer *viewer);
    void removeViewer(const QString &viewerName);

    void addViewerFlow(ModelViewerFlow *viewerFlow);
    void removeViewerFlow(const QString &viewerName, const QString &viewerFlowName);
};

#endif // CAMERAITEMMODEL_H
