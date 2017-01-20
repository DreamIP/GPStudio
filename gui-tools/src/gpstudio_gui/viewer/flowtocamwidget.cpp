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

#include "flowtocamwidget.h"

#include <QLayout>
#include <QLabel>
#include <QCoreApplication>
#include <QFileDialog>
#include <QStringList>
#include <QPushButton>
#include <QDebug>
#include <flowpackage.h>

FlowToCamWidget::FlowToCamWidget(QWidget *parent) : QWidget(parent)
{
    _camera = NULL;
    setupWidgets();
}

void FlowToCamWidget::setCamera(Camera *camera)
{
    _camera = camera;

    QWidget *widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(8,8,8,8);

    _signalMapper = new QSignalMapper(this);
    if(_camera)
    {
        const QList<ModelFlow *> &flowsOutCom = _camera->comBlock()->modelBlock()->flowsOut();
        foreach (ModelFlow *flow, flowsOutCom)
        {
            QPushButton *button = new QPushButton("Send to "+flow->name());
            button->setEnabled(false);
            connect(button, SIGNAL(clicked()), _signalMapper, SLOT(map()));
            connect(this, SIGNAL(sendAvailable(bool)), button, SLOT(setEnabled(bool)));
            _signalMapper->setMapping(button, flow->name());
            layout->addWidget(button);
        }
    }
    connect(_signalMapper, SIGNAL(mapped(QString)), this, SLOT(sendFlow(QString)));

    widget->setMinimumWidth(200);
    widget->setLayout(layout);

    _sendButtonArea->setWidget(widget);
}

void FlowToCamWidget::setPath(const QString &path)
{
    QString mpath = path;
    if(mpath.isEmpty())
        mpath = _pathLineEdit->text();
    QDir dir(mpath);
    _imagesListWidget->setRootIndex(_imagesSystemModelSorted->mapFromSource(_imagesSystemModel->index(dir.canonicalPath())));
    _pathLineEdit->setText(dir.canonicalPath()+"/");
    emit sendAvailable(false);
}

void FlowToCamWidget::selectPath()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select a directory of pictures", _pathLineEdit->text());
    if(!path.isEmpty())
        setPath(path);
}

void FlowToCamWidget::selectFile(QItemSelection selected, QItemSelection deselected)
{
    Q_UNUSED(deselected)
    emit sendAvailable(!selected.indexes().isEmpty());
}

void FlowToCamWidget::sendFlow(const QString &flowName)
{
    QString img;
    if(!_imagesListWidget->currentIndex().isValid())
        return;
    img = _imagesSystemModel->filePath(_imagesSystemModelSorted->mapToSource(_imagesListWidget->currentIndex()));

    QImage imageToSend(img);
    _camera->sendPackage(flowName, FlowPackage(imageToSend));
}

void FlowToCamWidget::setupWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    // path edit
    QLayout *layoutPath = new QHBoxLayout();
    layoutPath->setContentsMargins(0,0,0,0);
    layoutPath->addWidget(new QLabel("path:"));
    layoutPath->setSpacing(5);
    _pathLineEdit = new QLineEdit();
    layoutPath->addWidget(_pathLineEdit);
    connect(_pathLineEdit, SIGNAL(editingFinished()), this, SLOT(setPath()));
    _pathToolButton = new QToolButton();
    _pathToolButton->setText("...");
    connect(_pathToolButton, SIGNAL(clicked(bool)), this, SLOT(selectPath()));
    layoutPath->addWidget(_pathToolButton);
    layout->addItem(layoutPath);

    // image lists
    layout->addWidget(new QLabel("images:"));
    _imagesSystemModel = new QFileSystemModel();
    _imagesSystemModel->setRootPath(QCoreApplication::applicationDirPath());
    _imagesSystemModel->setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters<<"*.png"<<"*.jpg"<<"*.jpeg"<<"*.tif"<<"*.ico"<<"*.bmp";
    _imagesSystemModel->setNameFilters(filters);
    _imagesSystemModelSorted = new QSortFilterProxyModel();
    _imagesSystemModelSorted->setSourceModel(_imagesSystemModel);
    _imagesListWidget = new QTreeView();
    _imagesListWidget->setModel(_imagesSystemModelSorted);
    setPath(QCoreApplication::applicationDirPath()+"/..");
    _imagesListWidget->setSortingEnabled(true);
    _imagesListWidget->sortByColumn(0, Qt::AscendingOrder);
    layout->addWidget(_imagesListWidget);
    connect(_imagesListWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectFile(QItemSelection,QItemSelection)));

    // area for send button
    _sendButtonArea = new QScrollArea();
    layout->addWidget(_sendButtonArea);

    setLayout(layout);
}
