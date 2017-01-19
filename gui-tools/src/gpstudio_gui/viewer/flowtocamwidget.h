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

#ifndef FLOWTOCAMWIDGET_H
#define FLOWTOCAMWIDGET_H

#include "gpstudio_gui_common.h"

#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QScrollArea>

#include "camera/camera.h"

class GPSTUDIO_GUI_EXPORT FlowToCamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FlowToCamWidget(QWidget *parent = 0);

    void setCamera(Camera *camera);

signals:
    void sendAvailable(bool);

public slots:
    void setPath(const QString &path);
    void selectPath();
    void selectFile(QItemSelection selected, QItemSelection deselected);

private:
    void setupWidgets();
    QLineEdit *_pathLineEnit;
    QToolButton *_pathToolButton;
    QFileSystemModel *_imagesSystemModel;
    QSortFilterProxyModel *_imagesSystemModelSorted;
    QTreeView *_imagesListWidget;
    QScrollArea *_sendButtonArea;

    Camera *_camera;
};

#endif // FLOWTOCAMWIDGET_H
