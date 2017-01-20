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

#include "camexplorerwidget.h"

#include "camera/camera.h"

#include <QDebug>
#include <QFormLayout>
#include <QLabel>
#include <QModelIndexList>
#include <QSplitter>
#include <QStringList>

#include <propertywidgets/propertywidgets.h>

CamExplorerWidget::CamExplorerWidget(QWidget *parent)
    : QWidget(parent)
{
    setCamera(NULL);
    setModeView(WidgetsMode);
}

CamExplorerWidget::CamExplorerWidget(Camera *camera, QWidget *parent)
    : QWidget(parent)
{
    setCamera(camera);
    setModeView(WidgetsMode);
}

CamExplorerWidget::CamExplorerWidget(Camera *camera, CamExplorerWidget::ModeView modeView, QWidget *parent)
    : QWidget(parent)
{
    setCamera(camera);
    setModeView(modeView);
}

void CamExplorerWidget::attachProject(GPNodeProject *project)
{
    _project = project;

    connect(_project, SIGNAL(nodeChanged(ModelNode*)), this, SLOT(update()));

    connect(_project, SIGNAL(blockAdded(ModelBlock*)), this, SLOT(update()));
    connect(_project, SIGNAL(blockRemoved(QString)), this, SLOT(update()));
    connect(_project, SIGNAL(blockUpdated(ModelBlock*)), this, SLOT(update()));

    connect(_camItemModel, SIGNAL(nodeRenamed(QString,QString)), _project, SLOT(renameNode(QString,QString)));
    connect(_camItemModel, SIGNAL(blockRenamed(QString,QString)), _project, SLOT(renameBlock(QString,QString)));

    connect(this, SIGNAL(propertyChanged(QString,QString,QVariant)), _project, SLOT(blockSetParam(QString,QString,QVariant)));

    if(_project->camera())
        setCamera(_project->camera());
}

GPNodeProject *CamExplorerWidget::project() const
{
    return _project;
}

void CamExplorerWidget::setupWidgets()
{
    if(layout())
    {
        QLayoutItem *child;
        while ((child = layout()->takeAt(0)) != 0)
        {
            if(child->widget())
            {
                child->widget()->hide();
                child->widget()->deleteLater();
            }

            // don't delete _statusLayout
            delete child;
        }
        delete layout();
    }
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    layout->addWidget(splitter);

    _camTreeView = new QTreeView(this);
    _camItemModel = new CameraItemModel(this);
    _camItemModelSorted = new QSortFilterProxyModel(this);
    _camItemModelSorted->setSourceModel(_camItemModel);
    _camTreeView->setModel(_camItemModelSorted);
    _camTreeView->setSortingEnabled(true);
    _camTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    splitter->addWidget(_camTreeView);

    setMinimumWidth(300);

    connect(_camTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(updateRootProperty()));

    switch (_modeView)
    {
    case CamExplorerWidget::TreeViewMode:
        _propertyItemModel = new PropertyItemModelNoSorted();
        _propertyTreeView = new QTreeView();
        _propertyTreeView->setModel(_propertyItemModel);
        _propertyTreeView->setSortingEnabled(true);
        splitter->addWidget(_propertyTreeView);
        break;
    case CamExplorerWidget::WidgetsMode:
    {
        _propertyWidget = new QScrollArea();
        _propertyWidget->setWidgetResizable(true);
        splitter->addWidget(_propertyWidget);
        break;
    }
    default:
        break;
    }

    if(_camera)
        setCamera(_camera);
    setLayout(layout);
}

void CamExplorerWidget::setRootProperty(const Property *property)
{
    disconnect(this, SLOT(changePropertyValue()));
    switch (_modeView)
    {
    case CamExplorerWidget::WidgetsMode:
        {
            QWidget *widget = new QWidget();
            widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

            QBoxLayout *layout = new QVBoxLayout();
            layout->setContentsMargins(0,0,0,0);

            if(property)
            {
                PropertyWidget *propertyWidget = new PropertyGroupWidget(false);
                propertyWidget->setLinkedProperty(property);
                connectProperty(propertyWidget);
                layout->addWidget(propertyWidget);
            }

            // TODO get internal size
            widget->setMinimumWidth(_propertyWidget->viewport()->width()-30);
            widget->setLayout(layout);

            _propertyWidget->setWidget(widget);

            break;
        }
    case CamExplorerWidget::TreeViewMode:
        _propertyItemModel->setRootProperty(property);
        break;
    }
}

void CamExplorerWidget::connectProperty(const PropertyWidget *propertyWidget)
{
    if(!propertyWidget)
        return;

    connect(propertyWidget, SIGNAL(valueChanged(QVariant)), this, SLOT(changePropertyValue()));
    foreach (PropertyWidget *propertyWidget, propertyWidget->subPropertyWidgets())
        connectProperty(propertyWidget);
}

void CamExplorerWidget::changePropertyValue()
{
    PropertyWidget *propertyWidget = static_cast<PropertyWidget *>(QObject::sender());
    const Property *property = propertyWidget->linkedProperty();
    emit propertyChanged(property->blockName(), property->getPath(), property->value().toString());
}

CamExplorerWidget::ModeView CamExplorerWidget::modeView() const
{
    return _modeView;
}

void CamExplorerWidget::setModeView(const ModeView &modeView)
{
    _modeView = modeView;
    setupWidgets();
}

void CamExplorerWidget::setCamera(Camera *camera)
{
    _camera = camera;
    if(_camera)
    {
        _camItemModel->clearAll();
        _camItemModel->setCamera(camera);
        _camTreeView->expandToDepth(0);
        _camTreeView->resizeColumnToContents(0);

        //setRootProperty(NULL);
    }
}

void CamExplorerWidget::updateRootProperty()
{
    if(!_camTreeView->selectionModel()->hasSelection())
    {
        setRootProperty(NULL);
        emit blockSelected("");
        return;
    }

    const Property *rootProp = NULL;
    QStringList selectedBlocksName;
    foreach( QModelIndex index, _camTreeView->selectionModel()->selection().indexes())
    {
        if(!index.isValid() || index.model()!=_camItemModelSorted)
            continue;

        QModelIndex localMapToSource = _camItemModelSorted->mapToSource(index);
        if(!localMapToSource.isValid())
            continue;

        CameraItem *item = static_cast<CameraItem*>(localMapToSource.internalPointer());
        if(!item)
            continue;

        switch (item->type())
        {
        case CameraItem::CameraType:
            rootProp = &item->camera()->rootProperty();
            break;
        case CameraItem::BlockType:
            rootProp = item->block()->assocProperty();
            selectedBlocksName.append(item->block()->name());
            break;
        case CameraItem::ModelBlockType:
            selectedBlocksName.append(item->modelBlock()->name());
            break;
        case CameraItem::FlowType:
            rootProp = item->flow()->assocProperty();
            break;
        default:
            break;
        }
    }

    setRootProperty(rootProp);
    emit blockSelected(selectedBlocksName.join(";"));
}

void CamExplorerWidget::selectBlock(QString blocksName)
{
    //_camTreeView->selectionModel()->blockSignals(true);
    blockSignals(true);

    if(!blocksName.isEmpty())
    {
        _camTreeView->selectionModel()->clearSelection();
        foreach (QString blockName, blocksName.split(";"))
        {
            QModelIndexList items = _camTreeView->model()->match(_camTreeView->model()->index(0, 0), Qt::DisplayRole, QVariant(blockName), -1, Qt::MatchRecursive);
            if(items.count()>0)
                _camTreeView->selectionModel()->select(items.at(0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }
    else
    {
        _camTreeView->selectionModel()->clearSelection();
    }

    //_camTreeView->selectionModel()->blockSignals(false);
    blockSignals(false);
}

void CamExplorerWidget::update()
{
    if(_project->camera())
    {
        setCamera(_project->camera());
        return;
    }
    if(_camera)
        setCamera(_camera);
}

void CamExplorerWidget::switchModeView()
{
    if(_modeView == TreeViewMode)
       setModeView(WidgetsMode);
    else
       setModeView(TreeViewMode);
}
