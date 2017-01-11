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

#ifndef CAMEXPLORERWIDGET_H
#define CAMEXPLORERWIDGET_H

#include "gpstudio_gui_common.h"

#include <QWidget>

#include "itemmodel/propertyitemmodel.h"
#include "itemmodel/cameraitemmodel.h"

#include "model/model_node.h"

#include <QTreeView>
#include <QBoxLayout>
#include <QScrollArea>

#include <nodeeditor/gpnodeproject.h>

class PropertyWidget;

class GPSTUDIO_GUI_EXPORT CamExplorerWidget : public QWidget
{
    Q_OBJECT
public:
    enum ModeView {
        TreeViewMode,
        WidgetsMode
    };

    CamExplorerWidget(QWidget *parent=0);
    CamExplorerWidget(Camera *camera, QWidget *parent=0);
    CamExplorerWidget(Camera *camera, ModeView modeView, QWidget *parent=0);

    void attachProject(GPNodeProject *project);
    GPNodeProject *project() const;

    void setCamera(Camera *camera); // remove this method

    ModeView modeView() const;
    void setModeView(const ModeView &modeView);

protected slots:
    void updateRootProperty();

public slots:
    void selectBlock(QString blocksName);
    void update();

signals:
    void blockSelected(QString blockName);
    void propertyChanged(const QString &blockName, const QString &propertyName, const QVariant &value);

private:
    void setupWidgets();
    void setRootProperty(const Property *property);

    QTreeView *_camTreeView;
    QSortFilterProxyModel *_camItemModelSorted;
    CameraItemModel *_camItemModel;

    QTreeView *_propertyTreeView;
    PropertyItemModelNoSorted *_propertyItemModel;
    QScrollArea *_propertyWidget;

    Camera *_camera;
    GPNodeProject *_project;

    ModeView _modeView;

    void connectProperty(const PropertyWidget *propertyWidget);
protected slots:
    void changePropertyValue();
};

#endif // CAMEXPLORERWIDGET_H
