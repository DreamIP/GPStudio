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

#ifndef BLOCKEDITORWINDOW_H
#define BLOCKEDITORWINDOW_H

#include "gpstudio_gui_common.h"

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTabWidget>

#include "model/model_block.h"

class GPSTUDIO_GUI_EXPORT BlockEditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BlockEditorWindow(QWidget *parent = 0, ModelBlock *block = 0);
    ~BlockEditorWindow();

    static void showImplementationsFiles(const QString &driver);

public slots:
    void about();
    void aboutQt();

protected:
    void closeEvent(QCloseEvent *event);

signals:

protected slots:
    void openFile(const QModelIndex &indexFile);
    void closeTab(int id);

protected:
    void setupWidgets();
    QTreeView *_filesTreeView;
    QTabWidget *_tabFiles;

    void createToolBarAndMenu();
    QToolBar *_mainToolBar;

    void setBlock(ModelBlock *block);
    QStandardItemModel *_filesModel;
    ModelBlock *_block;

    QString _path;
};

#endif // BLOCKEDITORWINDOW_H
